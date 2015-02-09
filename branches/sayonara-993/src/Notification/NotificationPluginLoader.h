/* NotificationPluginLoader.h */

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



#ifndef NOTIFICATION_PLUGIN_LOADER_H_
#define NOTIFICATION_PLUGIN_LOADER_H_

#include "Notification/Notification.h"
#include "HelperStructs/SayonaraClass.h"

#include <QList>

class NotificationPluginLoader;

class NotificationPluginLoader : public QObject, protected SayonaraClass {

    Q_OBJECT

	SINGLETON(NotificationPluginLoader)

public:
	
    NotificationPluginLoader(NotificationPluginLoader&);
    QList<Notification*> get_plugins();
    Notification* get_cur_plugin();
    void set_cur_plugin(Notification* n);
    void set_cur_plugin(QString name);

private slots:
	void _sl_cur_plugin_changed();

private:
    QString app_dir;

    QList<Notification*> _notification_plugins;
    Notification*       _cur_notification;
    int                 _cur_idx;

};

#endif

