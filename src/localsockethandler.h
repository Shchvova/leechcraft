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

#ifndef LOCALSOCKETHANDLER_H
#define LOCALSOCKETHANDLER_H
#include <memory>
#include <QLocalServer>

namespace LeechCraft
{
	class DownloadEntity;
	class MainWindow;

	class LocalSocketHandler : public QObject
	{
		Q_OBJECT

		std::auto_ptr<QLocalServer> Server_;
		MainWindow *Window_;
	public:
		LocalSocketHandler (MainWindow* = 0);
	private slots:
		void handleNewLocalServerConnection ();
		void pullCommandLine ();
	private:
		void DoLine (const QStringList&);
	signals:
		void gotEntity (const LeechCraft::DownloadEntity&);
	};
};

#endif