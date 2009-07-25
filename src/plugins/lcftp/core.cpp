#include "core.h"
#include <QUrl>
#include <QMutexLocker>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QPushButton>
#include <QTimer>
#include <curl/curl.h>
#include <plugininterface/proxy.h>
#include "inactiveworkersfilter.h"
#include "xmlsettingsmanager.h"
#include "watchthread.h"

namespace LeechCraft
{
	namespace Plugins
	{
		namespace LCFTP
		{
			Core* Core::Instance_ = 0;
			QMutex Core::InstanceMutex_;

			Core::Core ()
			: WatchThread_ (new WatchThread (this))
			, Quitting_ (false)
			, NumScheduledWorkers_ (0)
			, RunningHandles_ (0)
			{
				qRegisterMetaType<TaskData> ("TaskData");
				qRegisterMetaType<FetchedEntry> ("FetchedEntry");

				WorkersFilter_.reset (new InactiveWorkersFilter (this));

				{
					QMutexLocker l (&MultiHandleMutex_);
					MultiHandle_.reset (curl_multi_init (), curl_multi_cleanup);
				}
				ShareHandle_.reset (curl_share_init (), curl_share_cleanup);

				handleTotalNumWorkersChanged ();

				XmlSettingsManager::Instance ().RegisterObject ("TotalNumWorkers",
						this, "handleTotalNumWorkersChanged");
				XmlSettingsManager::Instance ().RegisterObject ("WorkersPerDomain",
						this, "handleWorkersPerDomainChanged");

				handleUpdateInterface ();

				WatchThread_->start (QThread::IdlePriority);
				connect (WatchThread_,
						SIGNAL (shouldPerform ()),
						this,
						SLOT (handlePerform ()),
						Qt::QueuedConnection);

				QTimer *timer = new QTimer (this);
				timer->setInterval (500);
				connect (timer,
						SIGNAL (timeout ()),
						this,
						SLOT (handleUpdateInterface ()));
				timer->start ();
			}

			Core& Core::Instance ()
			{
				if (!Instance_)
				{
					QMutexLocker locker (&InstanceMutex_);
					if (!Instance_)
						Instance_ = new Core ();
				}
				return *Instance_;
			}

			void Core::Release ()
			{
				Quitting_ = true;
				WatchThread_->SetExit ();

				deleteLater ();

				QMutexLocker l (&MultiHandleMutex_);
				Q_FOREACH (Worker_ptr w, Workers_)
					curl_multi_remove_handle (MultiHandle_.get (),
							w->GetHandle ().get ());

				if (!WatchThread_->wait (100))
					WatchThread_->terminate ();
			}

			void Core::SetCoreProxy (ICoreProxy_ptr proxy)
			{
				Proxy_ = proxy;
			}

			QAbstractItemModel* Core::GetModel () const
			{
				return WorkersFilter_.get ();
			}

			qint64 Core::GetDownloadSpeed () const
			{
				qint64 result = 0;
				Q_FOREACH (Worker::TaskState st, States_)
					result += st.DLSpeed_;
				return result;
			}

			qint64 Core::GetUploadSpeed () const
			{
				qint64 result = 0;
				Q_FOREACH (Worker::TaskState st, States_)
					result += st.ULSpeed_;
				return result;
			}

			int Core::columnCount (const QModelIndex&) const
			{
				return 3;
			}

			QVariant Core::data (const QModelIndex& index, int role) const
			{
				if (!index.isValid ())
					return QVariant ();

				int working = Workers_.size ();
				int r = index.row ();
				int c = index.column ();

				if (role == Qt::DisplayRole)
				{
					switch (c)
					{
						case 0:
							if (r < working)
								return States_.at (r).URL_.toString ();
							else
								return Tasks_ [r - working].URL_.toString ();
						case 1:
							return r < working ?
								tr ("Downloading at %1")
									.arg (Util::Proxy::Instance ()->
											MakePrettySize (States_.at (r).DLSpeed_)) :
								tr ("Waiting");
						case 2:
							if (r >= working)
								return QVariant ();
							else
							{
								QPair<quint64, quint64> s = States_.at (r).DL_;
								if (s.second)
									return tr ("%1 of %2 (%3%)")
										.arg (Util::Proxy::Instance ()->
												MakePrettySize (s.first))
										.arg (Util::Proxy::Instance ()->
												MakePrettySize (s.second))
										.arg (s.first * 100 / s.second);
								else
									return tr ("%1 of %2")
										.arg (Util::Proxy::Instance ()->
												MakePrettySize (s.first))
										.arg (Util::Proxy::Instance ()->
												MakePrettySize (s.second));
							}
						default:
							return QVariant ();
					}
				}
				else
					return QVariant ();
			}

			QModelIndex Core::index (int row, int column, const QModelIndex& parent) const
			{
				if (!hasIndex (row, column, parent))
					return QModelIndex ();
			
				return createIndex (row, column);
			}

			QModelIndex Core::parent (const QModelIndex&) const
			{
				return QModelIndex ();
			}

			int Core::rowCount (const QModelIndex& parent) const
			{
				if (parent.isValid ())
					return 0;

				int result = 0;
				result += Workers_.size ();
				result += Tasks_.size ();
				return result;
			}

			QStringList Core::Provides () const
			{
				curl_version_info_data *data = curl_version_info (CURLVERSION_NOW);
				QStringList result ("ftp");
				if (data->age > 0)
				{
					if (data->features & CURL_VERSION_SSL)
						result << "ftps";
				}
				return result;
			}

			bool Core::IsOK (const DownloadEntity& e) const
			{
				if (!e.Entity_.canConvert<QUrl> ())
					return false;

				QUrl url = e.Entity_.toUrl ();
				if (Provides ().contains (url.scheme ()))
					return true;
				else
					return false;
			}

			namespace
			{
				QString CheckName (const QUrl& url, const QString& location)
				{
					QFileInfo fi (location);
					QString dir, file;
					if (fi.isDir ())
						dir = fi.path ();
					else
					{
						dir = fi.dir ().path ();
						file = fi.fileName ();
					}

					if (fi.isDir ())
					{
						dir = location;
						if (file.isEmpty ())
							file = QFileInfo (url.toString (QUrl::RemoveFragment)).fileName ();

						QDir fd (dir);

						if (!file.isEmpty () &&
								fd.exists (file))
						{
							QMessageBox box (QMessageBox::Question,
									Core::tr ("LeechCraft"),
									Core::tr ("%1 already exists. What do you want to do?")
										.arg (QDir::toNativeSeparators (dir + "/" + file)));
							QPushButton *resume = box.addButton (Core::tr ("Resume"),
									QMessageBox::AcceptRole);
							QPushButton *overwrite = box.addButton (Core::tr ("Overwrite"),
									QMessageBox::DestructiveRole);
							QPushButton *rename = box.addButton (Core::tr ("Rename"),
									QMessageBox::ActionRole);
							QPushButton *cancel = box.addButton (Core::tr ("Cancel"),
									QMessageBox::RejectRole);
							box.setDefaultButton (resume);
							box.setEscapeButton (cancel);

							box.exec ();

							QAbstractButton *clicked = box.clickedButton ();
							if (clicked == overwrite)
							{
								if (!fd.remove (file))
								{
									QMessageBox::critical (0,
											Core::tr ("LeechCraft"),
											Core::tr ("Error removing %1")
												.arg (QDir::toNativeSeparators (dir + "/" + file)));
									return QString ();
								}
							}
							else if (clicked == resume)
							{
								// Do nothing
							}
							else if (clicked == rename)
							{
								while (fd.exists (file))
								{
									QString filename = QFileDialog::getSaveFileName (0,
											Core::tr ("Choose new file name"),
											dir);
									if (filename.isEmpty ())
										return QString ();
									fi = QFileInfo (filename);
									dir = fi.dir ().path ();
									file = fi.fileName ();
									fd = QDir (dir);
								}
							}
							else
								return QString ();
						}
					}

					return dir + "/" + file;
				}
			};

			int Core::Add (DownloadEntity e)
			{
				if (!e.Entity_.canConvert<QUrl> ())
					return -1;

				QUrl url = e.Entity_.toUrl ();

				QFileInfo fi (e.Location_);
				QString dir, file;
				if (fi.isDir ())
					dir = fi.path ();
				else
				{
					dir = fi.dir ().path ();
					file = fi.fileName ();
				}
	
				QString tfn = dir + "/" + file;
				if (!(e.Parameters_ & LeechCraft::Internal))
				{
					tfn = CheckName (e.Entity_.toUrl (), e.Location_);
					if (tfn.isEmpty ())
						return -1;
				}
				else
					tfn = dir + "/" + file;

				TaskData td =
				{
					Proxy_->GetID (),
					url,
					tfn
				};
				QueueTask (td);
				return td.ID_;
			}

			bool Core::IsAcceptable (int index) const
			{
				return index >= States_.size () ||
					States_.at (index).IsWorking_;
			}

			bool Core::SelectSuitableTask (TaskData *result)
			{
				int limit = XmlSettingsManager::Instance ()
					.property ("WorkersPerDomain").toInt ();
				Q_FOREACH (TaskData td, Tasks_)
				{
					QString domain = td.URL_.host ();

					if (!WorkersPerDomain_.contains (domain))
						WorkersPerDomain_ [domain] = 0;

					if (WorkersPerDomain_ [domain] > limit)
						continue;

					WorkersPerDomain_ [domain]++;
					*result = td;
					Tasks_.removeAll (td);
					return true;
				}
				return false;
			}

			void Core::QueueTask (const TaskData& td)
			{
				beginInsertRows (QModelIndex (),
						Tasks_.size (), Tasks_.size ());
				Tasks_ << td;
				endInsertRows ();
				Reschedule ();
			}

			void Core::AddWorker (int i)
			{
				Worker_ptr w (new Worker (i));
				connect (w.get (),
						SIGNAL (error (const QString&, const TaskData&)),
						this,
						SLOT (handleError (const QString&, const TaskData&)));
				connect (w.get (),
						SIGNAL (finished (const TaskData&)),
						this,
						SLOT (handleFinished (const TaskData&)));
				connect (w.get (),
						SIGNAL (fetchedEntry (const FetchedEntry&)),
						this,
						SLOT (handleFetchedEntry (const FetchedEntry&)),
						Qt::QueuedConnection);

				curl_easy_setopt (w->GetHandle ().get (),
						CURLOPT_SHARE, ShareHandle_.get ());

				beginInsertRows (QModelIndex (), i, i);
				Workers_ << w;
				States_ << w->GetState ();
				endInsertRows ();

				Reschedule ();
			}

			void Core::Reschedule ()
			{
				int inQueue = 1;
				do
				{
					QMutexLocker l (&MultiHandleMutex_);
					CURLMsg *info = curl_multi_info_read (MultiHandle_.get (),
							&inQueue);
					if (!info)
						continue;

					switch (info->msg)
					{
						case CURLMSG_DONE:
							{
								Worker_ptr w = FindWorker (info->easy_handle);
								curl_multi_remove_handle (MultiHandle_.get (),
										info->easy_handle);
								w->NotifyFinished (info->data.result);

								if (NumScheduledWorkers_)
								{
									for (int i = 0; i < Workers_.size (); ++i)
									{
										if (Workers_.at (i) != w)
											continue;

										beginRemoveRows (QModelIndex (), i, i);
										Workers_.removeAt (i);
										States_.removeAt (i);
										for (int j = i; j < Workers_.size (); ++j)
											Workers_ [j]->SetID (j - 1);
										--NumScheduledWorkers_;
										endRemoveRows ();
										break;
									}
								}
							}
							break;
						default:
							qWarning () << Q_FUNC_INFO
								<< "unhandled message"
								<< info->msg;
							break;
					}
				}
				while (inQueue);

				while (RunningHandles_ < Workers_.size () &&
						Tasks_.size ())
				{
					TaskData td;
					if (!SelectSuitableTask (&td))
						break;
					Q_FOREACH (Worker_ptr w, Workers_)
						if (!w->IsWorking ())
						{
							{
								QMutexLocker l (&MultiHandleMutex_);
								curl_multi_add_handle (MultiHandle_.get (),
										w->Start (td).get ());
							}
							handlePerform ();
							break;
						}
				}
			}

			Worker_ptr Core::FindWorker (CURL *handle) const
			{
				Q_FOREACH (Worker_ptr w, Workers_)
					if (w->GetHandle ().get () == handle)
						return w;
				qWarning () << Q_FUNC_INFO
					<< "not found handle"
					<< handle;
				return Worker_ptr ();
			}

			void Core::handlePerform ()
			{
				bool reschedule = false;
				int prev = RunningHandles_;
				{
					QMutexLocker l (&MultiHandleMutex_);
					while (curl_multi_perform (MultiHandle_.get (),
								&RunningHandles_) == CURLM_CALL_MULTI_PERFORM)
					{
						if (prev != RunningHandles_)
						{
							reschedule = true;
							prev = RunningHandles_;
						}
					}
				}
				if (reschedule ||
						prev != RunningHandles_ ||
						(Tasks_.size () &&
						 RunningHandles_ < Workers_.size ()))
					Reschedule ();
			}
			
			void Core::handleError (const QString& msg, const TaskData& td)
			{
				--WorkersPerDomain_ [td.URL_.host ()];

				if (td.ID_ >= 0)
					emit taskError (td.ID_, IDownload::EUnknown);

				QMessageBox::critical (0,
						tr ("LeechCraft"),
						msg);
			}

			void Core::handleFinished (const TaskData& data)
			{
				--WorkersPerDomain_ [data.URL_.host ()];

				emit downloadFinished (tr ("Download finished: %1")
						.arg (data.Filename_));

				if (data.ID_ >= 0)
					emit taskFinished (data.ID_);
			}

			void Core::handleFetchedEntry (const FetchedEntry& entry)
			{
				QString name = entry.PreviousTask_.Filename_ + entry.Name_;
				if (entry.IsDir_)
				{
					QDir dir (entry.PreviousTask_.Filename_);
					if (!dir.exists (entry.Name_))
						dir.mkdir (entry.Name_);
					else if (!QFileInfo (name).isDir ())
					{
						QMessageBox::critical (0,
								tr ("LeechCraft"),
								tr ("While mirroring<br />%1<br />to<br />%2<br />"
									"an error occured:<br />%3<br /> already exists.")
									.arg (entry.PreviousTask_.URL_.toString ())
									.arg (QDir::toNativeSeparators (entry.PreviousTask_.Filename_))
									.arg (QDir::toNativeSeparators (name)));
						return;
					}
					name += "/";
				}
				else
					name = CheckName (entry.URL_, entry.PreviousTask_.Filename_);

				TaskData td =
				{
					entry.PreviousTask_.ID_ >= 0 ? Proxy_->GetID () : -1,
					entry.URL_,
					name
				};
				QueueTask (td);
			}

			void Core::handleUpdateInterface ()
			{
				States_.clear ();
				for (int i = 0; i < Workers_.size (); ++i)
					States_ << Workers_.at (i)->GetState ();

				emit dataChanged (index (0, 0),
						index (States_.size () - 1, 2));
			}

			void Core::handleTotalNumWorkersChanged ()
			{
				int numWorkers = XmlSettingsManager::Instance ()
					.property ("TotalNumWorkers").toInt ();
				int current = Workers_.size ();
				int delta = numWorkers - current - NumScheduledWorkers_;
				if (delta >= 0)
				{
					NumScheduledWorkers_ = 0;
					for (int i = 0; i < delta; ++i)
						AddWorker (i + current);
				}
				else
					NumScheduledWorkers_ = -delta;
			}

			void Core::handleWorkersPerDomainChanged ()
			{
				Reschedule ();
			}
		};
	};
};
