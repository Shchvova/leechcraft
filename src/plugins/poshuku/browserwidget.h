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

#ifndef PLUGINS_POSHUKU_BROWSERWIDGET_H
#define PLUGINS_POSHUKU_BROWSERWIDGET_H
#include <boost/shared_ptr.hpp>
#include <QWidget>
#include <QTime>
#include <interfaces/imultitabs.h>
#include <interfaces/iwebbrowser.h>
#include <interfaces/ihaveshortcuts.h>
#include <interfaces/structures.h>
#include "interfaces/ibrowserwidget.h"
#include "ui_browserwidget.h"

class QToolBar;
class QDataStream;

namespace LeechCraft
{
	namespace Plugins
	{
		namespace Poshuku
		{
			class FindDialog;
			class PasswordRemember;
			struct BrowserWidgetSettings;

			class BrowserWidget : public QWidget
								, public IBrowserWidget
								, public IWebWidget
								, public IMultiTabsWidget
			{
				Q_OBJECT
				Q_INTERFACES (LeechCraft::Plugins::Poshuku::IBrowserWidget IWebWidget IMultiTabsWidget)

				Ui::BrowserWidget Ui_;

				QToolBar *ToolBar_;
				QAction *Add2Favorites_;
				QAction *Find_;
				QAction *Print_;
				QAction *PrintPreview_;
				QAction *ScreenSave_;
				QAction *ViewSources_;
				QAction *ContentsEditable_;
				QAction *ZoomIn_;
				QAction *ZoomOut_;
				QAction *ZoomReset_;
				QAction *Cut_;
				QAction *Copy_;
				QAction *Paste_;
				QAction *Back_;
				QAction *Forward_;
				QAction *Reload_;
				QAction *Stop_;
				QAction *ReloadStop_;
				QAction *ReloadPeriodically_;
				QAction *NotifyWhenFinished_;
				QAction *RecentlyClosedAction_;
				QPoint OnLoadPos_;
				QMenu *ChangeEncoding_;
				QMenu *RecentlyClosed_;
				QMenu *ExternalLinks_;
				FindDialog *FindDialog_;
				PasswordRemember *RememberDialog_;
				QTimer *ReloadTimer_;
				QString PreviousFindText_;
				bool HtmlMode_;
				bool Own_;
				QMap<QString, QList<QAction*> > WindowMenus_;

				static QObject* S_MultiTabsParent_;

				friend class CustomWebView;
			public:
				BrowserWidget (QWidget* = 0);
				virtual ~BrowserWidget ();
				static void SetParentMultiTabs (QObject*);

				void Deown ();
				void InitShortcuts ();

				void SetUnclosers (const QList<QAction*>&);
				CustomWebView* GetView () const;
				// This is the same as above but to satisfy the IBrowserWidget.
				QWebView* GetWebView () const;
				QLineEdit* GetURLEdit () const;

				BrowserWidgetSettings GetWidgetSettings () const;
				void SetWidgetSettings (const BrowserWidgetSettings&);
				void SetURL (const QUrl&);

				void Load (const QString&);
				void SetHtml (const QString&, const QUrl& = QUrl ());
				void SetNavBarVisible (bool);
				void SetEverythingElseVisible (bool);
				QWidget* Widget ();

				void SetShortcut (const QString&, const QKeySequences_t&);
				QMap<QString, ActionInfo> GetActionInfo () const;

				void Remove ();
				QToolBar* GetToolBar () const;
				void NewTabRequested ();
				QList<QAction*> GetTabBarContextMenuActions () const;
				QMap<QString, QList<QAction*> > GetWindowMenus () const;
				QObject* ParentMultiTabs () const;

				void SetOnLoadScrollPoint (const QPoint&);
			private:
				void PrintImpl (bool, QWebFrame*);
				void SetActualReloadInterval (const QTime&);
			public slots:
				void notificationActionTriggered (int);
				void focusLineEdit ();
				QWebView* getWebView () const;
				QLineEdit* getAddressBar () const;
				QWidget* getSideBar () const;
			private slots:
				void handleIconChanged ();
				void handleStatusBarMessage (const QString&);
				void handleURLFrameLoad (const QString&);
				void handleReloadPeriodically ();
				void handleAdd2Favorites ();
				void handleFind ();
				void findText (const QString&, QWebPage::FindFlags);
				void handleViewPrint (QWebFrame*);
				void handlePrinting ();
				void handlePrintingWithPreview ();
				void handleScreenSave ();
				void handleViewSources ();
				void handleNewUnclose (QAction*);
				void handleUncloseDestroyed ();
				void updateTooltip ();
				void enableActions ();
				void handleEntityAction ();
				void checkLinkRels ();
				void setScrollPosition ();
				void pageFocus ();
				void handleLoadProgress (int);
				void notifyLoadFinished (bool);
				void handleChangeEncodingAboutToShow ();
				void handleChangeEncodingTriggered (QAction*);
				void updateLogicalPath ();
				void showSendersMenu ();
				void handleUrlChanged (const QString&);
			signals:
				void titleChanged (const QString&);
				void urlChanged (const QString&);
				void iconChanged (const QIcon&);
				void needToClose ();
				void tooltipChanged (QWidget*);
				void addToFavorites (const QString&, const QString&);
				void statusBarChanged (const QString&);
				void gotEntity (const LeechCraft::Entity&);
				void delegateEntity (const LeechCraft::Entity&, int*, QObject**);
				void couldHandle (const LeechCraft::Entity&, bool*);
				void invalidateSettings ();
				void raiseTab (QWidget*);

				// Hook support
				void hookFindText (LeechCraft::IHookProxy_ptr proxy,
						QObject *browserWidget,
						QString *findText,
						QWebPage::FindFlags *findFlags);
				void hookIconChanged (LeechCraft::IHookProxy_ptr proxy,
						QObject *browserWidget);
				void hookLoadProgress (LeechCraft::IHookProxy_ptr proxy,
						QObject *browserWidget,
						int *progress);
				void hookMoreMenuFillBegin (LeechCraft::IHookProxy_ptr proxy,
						QMenu *menu,
						QWebView *webView,
						QObject *browserWidget);
				void hookMoreMenuFillEnd (LeechCraft::IHookProxy_ptr proxy,
						QMenu *menu,
						QWebView *webView,
						QObject *browserWidget);
				void hookNotificationActionTriggered (LeechCraft::IHookProxy_ptr proxy,
						QObject *browserWidget,
						int *index);
				void hookNotifyLoadFinished (LeechCraft::IHookProxy_ptr proxy,
						QObject *browserWidget,
						bool *ok,
						bool notifyWhenFinished,
						bool own,
						bool htmlMode);
				void hookPrint (LeechCraft::IHookProxy_ptr proxy,
						QObject *browserWidget,
						bool *preview,
						QWebFrame *frame);
				void hookSetURL (LeechCraft::IHookProxy_ptr proxy,
						QObject *browserWidget,
						QUrl *url);
				void hookStatusBarMessage (LeechCraft::IHookProxy_ptr proxy,
						QObject *browserWidget,
						QString *message);
				void hookTabBarContextMenuActions (LeechCraft::IHookProxy_ptr proxy,
						const QObject *browserWidget,
						QList<QAction*>*) const;
				void hookUpdateLogicalPath (LeechCraft::IHookProxy_ptr proxy,
						QObject *browserWidget);
				void hookURLEditReturnPressed (LeechCraft::IHookProxy_ptr proxy,
						QObject *browserWidget);
			};
		};
	};
};

#endif

