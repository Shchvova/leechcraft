/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2009  Georg Rudoy
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

#ifndef PLUGINS_AGGREGATOR_AGGREGATOR_H
#define PLUGINS_AGGREGATOR_AGGREGATOR_H
#include <memory>
#include <QWidget>
#include <QItemSelection>
#include <interfaces/iinfo.h>
#include <interfaces/iembedtab.h>
#include <interfaces/ijobholder.h>
#include <interfaces/ihavesettings.h>
#include <interfaces/ihaveshortcuts.h>
#include <interfaces/ientityhandler.h>
#include <interfaces/structures.h>
#include <interfaces/iactionsexporter.h>
#include <interfaces/istartupwizard.h>

class QSystemTrayIcon;
class QTranslator;
class QToolBar;
class IDownload;

namespace LeechCraft
{
	namespace Plugins
	{
		namespace Aggregator
		{
			struct Enclosure;

			struct Aggregator_Impl;

			class Aggregator : public QWidget
							 , public IInfo
							 , public IEmbedTab
							 , public IHaveSettings
							 , public IJobHolder
							 , public IEntityHandler
							 , public IHaveShortcuts
							 , public IActionsExporter
							 , public IStartupWizard
			{
				Q_OBJECT
				Q_INTERFACES (IInfo IEmbedTab IHaveSettings IJobHolder IEntityHandler IHaveShortcuts IStartupWizard IActionsExporter)

				Aggregator_Impl *Impl_;
			public:
				virtual ~Aggregator ();
				void Init (ICoreProxy_ptr);
				void SecondInit ();
				void Release ();
				QByteArray GetUniqueID () const;
				QString GetName () const;
				QString GetInfo () const;
				QStringList Provides () const;
				QStringList Needs () const;
				QStringList Uses () const;
				void SetProvider (QObject*, const QString&);
				QIcon GetIcon () const;

				QWidget* GetTabContents ();
				QToolBar* GetToolBar () const;
				QAbstractItemModel* GetRepresentation () const;

				boost::shared_ptr<LeechCraft::Util::XmlSettingsDialog> GetSettingsDialog () const;

				bool CouldHandle (const LeechCraft::Entity&) const;
				void Handle (LeechCraft::Entity);

				void SetShortcut (const QString&, const QKeySequences_t&);
				QMap<QString, LeechCraft::ActionInfo> GetActionInfo () const;

				QList<QWizardPage*> GetWizardPages () const;

				QList<QAction*> GetActions (ActionsEmbedPlace) const;
			protected:
				virtual void keyPressEvent (QKeyEvent*);
			private:
				void ScheduleShowError ();
				bool IsRepr () const;
				QModelIndex GetRelevantIndex () const;
				void BuildID2ActionTupleMap ();
			public slots:
				void handleTasksTreeSelectionCurrentRowChanged (const QModelIndex&, const QModelIndex&);
			private slots:
				void on_ActionAddFeed__triggered ();
				void on_ActionRemoveFeed__triggered ();
				void on_ActionUpdateSelectedFeed__triggered ();
				void on_ActionRegexpMatcher__triggered ();
				void on_ActionImportOPML__triggered ();
				void on_ActionExportOPML__triggered ();
				void on_ActionImportBinary__triggered ();
				void on_ActionExportBinary__triggered ();
				void on_ActionExportFB2__triggered ();
				void on_ActionMarkChannelAsRead__triggered ();
				void on_ActionMarkChannelAsUnread__triggered ();
				void on_ActionChannelSettings__triggered ();
				void on_MergeItems__toggled (bool);
				void currentChannelChanged ();
				void unreadNumberChanged (int);
				void trayIconActivated ();
				void handleGroupChannels ();
			signals:
				void gotEntity (const LeechCraft::Entity&);
				void delegateEntity (const LeechCraft::Entity&,
						int*, QObject**);
				void bringToFront ();
				void changeTabName (QWidget*, const QString&);
				void changeTabIcon (QWidget*, const QIcon&);
				void changeTooltip (QWidget*, QWidget*);
				void statusBarChanged (QWidget*, const QString&);
				void raiseTab (QWidget*);
			};
		};
	};
};

#endif

