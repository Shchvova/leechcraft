/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2010  Georg Rudoy
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

#ifndef PLUGINS_AZOTH_PLUGINS_XOOX_GLOOXCLENTRY_H
#define PLUGINS_AZOTH_PLUGINS_XOOX_GLOOXCLENTRY_H
#include <QObject>
#include "entrybase.h"

namespace gloox
{
	class RosterItem;
	class MessageSession;
}

namespace LeechCraft
{
	namespace Plugins
	{
		namespace Azoth
		{
			namespace Plugins
			{
				class IAccount;

				namespace Xoox
				{
					class GlooxAccount;

					class GlooxCLEntry : public EntryBase
					{
						Q_OBJECT

						Q_INTERFACES (LeechCraft::Plugins::Azoth::Plugins::ICLEntry)

						GlooxAccount *ParentAccountObject_;
						gloox::RosterItem *RI_;
						EntryStatus CurrentStatus_;
					public:
						GlooxCLEntry (gloox::RosterItem*, GlooxAccount*);

						void UpdateRI (gloox::RosterItem*);

						// ICLEntry
						QObject* GetParentAccount () const;
						Features GetEntryFeatures () const;
						EntryType GetEntryType () const;
						QString GetEntryName () const;
						void SetEntryName (const QString&);
						QByteArray GetEntryID () const;
						QStringList Groups () const;
						QStringList Variants () const;
						QObject* CreateMessage (IMessage::MessageType,
								const QString&, const QString&);
					signals:
						void availableVariantsChanged (const QStringList&);
					};
				}
			}
		}
	}
}

#endif
