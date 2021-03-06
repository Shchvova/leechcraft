/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2010  Georg Rudoy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include "lackman.h"
#include <QSortFilterProxyModel>
#include <QIcon>
#include <plugininterface/util.h>
#include "core.h"
#include "packagesdelegate.h"
#include "pendingmanager.h"
#include "typefilterproxymodel.h"
#include "xmlsettingsmanager.h"

namespace LeechCraft
{
	namespace Plugins
	{
		namespace LackMan
		{
			void Plugin::Init (ICoreProxy_ptr proxy)
			{
				Translator_.reset (Util::InstallTranslator ("lackman"));

				Ui_.setupUi (this);

				SettingsDialog_.reset (new Util::XmlSettingsDialog ());
				SettingsDialog_->RegisterObject (XmlSettingsManager::Instance (),
						"lackmansettings.xml");

				Core::Instance ().SetProxy (proxy);
				Core::Instance ().FinishInitialization ();

				SettingsDialog_->SetDataSource ("RepositoryList",
						Core::Instance ().GetRepositoryModel ());

				connect (&Core::Instance (),
						SIGNAL (delegateEntity (const LeechCraft::Entity&,
								int*, QObject**)),
						this,
						SIGNAL (delegateEntity (const LeechCraft::Entity&,
								int*, QObject**)));
				connect (&Core::Instance (),
						SIGNAL (gotEntity (const LeechCraft::Entity&)),
						this,
						SIGNAL (gotEntity (const LeechCraft::Entity&)));
				connect (&Core::Instance (),
						SIGNAL (tagsUpdated ()),
						this,
						SLOT (handleTagsUpdated ()));

				TypeFilter_ = new TypeFilterProxyModel (this);
				TypeFilter_->setDynamicSortFilter (true);
				TypeFilter_->setSourceModel (Core::Instance ().GetPluginsModel ());
				FilterByTags_ = new QSortFilterProxyModel (this);
				FilterByTags_->setDynamicSortFilter (true);
				FilterByTags_->setSourceModel (TypeFilter_);
				FilterString_ = new QSortFilterProxyModel (this);
				FilterString_->setDynamicSortFilter (true);
				FilterString_->setFilterCaseSensitivity (Qt::CaseInsensitive);
				FilterString_->setSourceModel (FilterByTags_);

				Ui_.Plugins_->setModel (FilterString_);
				PackagesDelegate *pd = new PackagesDelegate (Ui_.Plugins_);
				Ui_.Plugins_->setItemDelegate (pd);

				Ui_.PendingTree_->setModel (Core::Instance ()
						.GetPendingManager ()->GetPendingModel ());

				connect (Ui_.Plugins_->selectionModel (),
						SIGNAL (currentRowChanged (const QModelIndex&, const QModelIndex&)),
						pd,
						SLOT (handleRowChanged (const QModelIndex&, const QModelIndex&)),
						Qt::QueuedConnection);
				connect (Ui_.SearchLine_,
						SIGNAL (textEdited (const QString&)),
						FilterString_,
						SLOT (setFilterFixedString (const QString&)));

				BuildActions ();

				handleTagsUpdated ();
			}

			void Plugin::SecondInit ()
			{
				Core::Instance ().SecondInit ();
			}

			void Plugin::Release ()
			{
				Core::Instance ().Release ();
			}

			QByteArray Plugin::GetUniqueID () const
			{
				return "org.LeechCraft.LackMan";
			}

			QString Plugin::GetName () const
			{
				return "LackMan";
			}

			QString Plugin::GetInfo () const
			{
				return tr ("LeechCraft Package Manager.");
			}

			QIcon Plugin::GetIcon () const
			{
				return QIcon (":/resources/images/lackman.svg");
			}

			QStringList Plugin::Provides () const
			{
				return QStringList ();
			}

			QStringList Plugin::Needs () const
			{
				return QStringList ();
			}

			QStringList Plugin::Uses () const
			{
				return QStringList ();
			}

			void Plugin::SetProvider (QObject*, const QString&)
			{
			}

			QWidget* Plugin::GetTabContents ()
			{
				return this;
			}

			QToolBar* Plugin::GetToolBar () const
			{
				return Toolbar_;
			}

			Util::XmlSettingsDialog_ptr Plugin::GetSettingsDialog () const
			{
				return SettingsDialog_;
			}

			QList<QAction*> Plugin::GetActions (ActionsEmbedPlace place) const
			{
				QList<QAction*> result;
				if (place == AEPToolsMenu)
				{
					result << UpdateAll_;
					result << UpgradeAll_;
				}
				return result;
			}

			void Plugin::handleTagsUpdated ()
			{
			}

			void Plugin::on_Apply__released ()
			{
				Core::Instance ().AcceptPending ();
			}

			void Plugin::on_Cancel__released ()
			{
				Core::Instance ().CancelPending ();
			}

			void Plugin::on_PackageStatus__currentIndexChanged (int index)
			{
				TypeFilter_->SetFilterMode (static_cast<TypeFilterProxyModel::FilterMode> (index));
			}

			void Plugin::BuildActions ()
			{
				UpdateAll_ = new QAction (tr ("Update all repos"), this);
				UpdateAll_->setProperty ("ActionIcon", "refresh");
				connect (UpdateAll_,
						SIGNAL (triggered ()),
						&Core::Instance (),
						SLOT (updateAllRequested ()));
				UpgradeAll_ = new QAction (tr ("Upgrade all packages"), this);
				UpgradeAll_->setProperty ("ActionIcon", "fetchall");
				connect (UpgradeAll_,
						SIGNAL (triggered ()),
						&Core::Instance (),
						SLOT (upgradeAllRequested ()));

				Toolbar_ = new QToolBar (GetName ());
				Toolbar_->addAction (UpdateAll_);
				Toolbar_->addAction (UpgradeAll_);
			}
		};
	};
};

Q_EXPORT_PLUGIN2 (leechcraft_lackman, LeechCraft::Plugins::LackMan::Plugin);

