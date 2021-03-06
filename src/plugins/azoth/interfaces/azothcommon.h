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

#ifndef PLUGINS_AZOTH_INTERFACES_AZOTHCOMMON_H
#define PLUGINS_AZOTH_INTERFACES_AZOTHCOMMON_H

namespace LeechCraft
{
	namespace Plugins
	{
		namespace Azoth
		{
			namespace Plugins
			{
				enum State
				{
					SOnline,
					SChat,
					SAway,
					SDND,
					SXA,
					SOffline,
					SProbe,
					SError,
					SInvalid
				};

				inline bool IsLess (State s1, State s2)
				{
					return static_cast<int> (s1) < static_cast<int> (s2);
				}
			}
		}
	}
}

#endif
