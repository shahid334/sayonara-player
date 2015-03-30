/* DBusMediaKeysInterfaceMate.cpp */

/* Copyright (C) 2011-2014  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "DBus/DBusMediaKeysInterfaceMate.h"

DBusMediaKeysInterfaceMate::DBusMediaKeysInterfaceMate(QObject *parent) :
	DBusMediaKeysInterface(parent)
{

	_media_key_interface = new OrgMateSettingsDaemonMediaKeysInterface(
				"org.mate.SettingsDaemon",
				"/org/mate/SettingsDaemon/MediaKeys",
				QDBusConnection::sessionBus(),
				this);



	if (!QDBusConnection::sessionBus().interface()->isServiceRegistered("org.mate.SettingsDaemon"))
	{
		qDebug() << "DBus org.mate.SettingsDaemon registered";
		return;
	}

	QDBusPendingReply<> reply = _media_key_interface->GrabMediaPlayerKeys("sayonara", 0);
	QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(reply, this);

	connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this,
			SLOT(sl_register_finished(QDBusPendingCallWatcher*)));
}



void DBusMediaKeysInterfaceMate::sl_register_finished(QDBusPendingCallWatcher* watcher){

	DBusMediaKeysInterface::sl_register_finished(watcher);

	connect( _media_key_interface, SIGNAL(MediaPlayerKeyPressed(const QString&, const QString&)),
			 this, SLOT(sl_media_key_pressed(const QString&, const QString&)));

}
