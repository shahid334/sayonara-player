/* NotificationPluginLoader.cpp */

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



#include "Settings/Settings.h"
#include "Notification/NotificationPluginLoader.h"
#include "Notification/Notification.h"
#include "HelperStructs/Helper.h"

#include <QDir>
#include <QPluginLoader>


NotificationPluginLoader* NotificationPluginLoader::getInstance() {
    static NotificationPluginLoader instance;
    return &instance;
}


NotificationPluginLoader::NotificationPluginLoader() : SayonaraClass() {

    QString app_dir;
    #ifdef Q_OS_UNIX
        app_dir = Helper::getLibPath();
    #else
        app_dir = app->applicationDirPath();
    #endif

    _cur_idx = -1;
	QString preferred_plugin = _settings->get(Set::Notification_Name);

    QDir plugin_dir = QDir(app_dir);
    QStringList entry_list = plugin_dir.entryList(QDir::Files);

    for(int i=0; i<entry_list.size(); i++) {

        QString filename = entry_list[i];
        QPluginLoader loader(plugin_dir.absoluteFilePath(filename));

        QObject* plugin = loader.instance();
        if(!plugin) continue;

        Notification* notification = qobject_cast<Notification*>(plugin);
        if(!notification) continue;

        qDebug() << "Found plugin " << notification->get_name();
        _notification_plugins.push_back(notification);

		if(!preferred_plugin.compare(notification->get_name(), Qt::CaseInsensitive)){
			_cur_idx = _notification_plugins.size() -1;
		}
    }

	REGISTER_LISTENER(Set::Notification_Name, _sl_cur_plugin_changed);
}



QList<Notification*> NotificationPluginLoader::get_plugins() {
	return _notification_plugins;
}

void NotificationPluginLoader::_sl_cur_plugin_changed(){
	QString cur_plugin = _settings->get(Set::Notification_Name);
	set_cur_plugin(cur_plugin);
}

void NotificationPluginLoader::set_cur_plugin(QString name) {

	_cur_idx = -1;

    for(int i=0; i<_notification_plugins.size(); i++) {
        QString name_tmp = _notification_plugins[i]->get_name();
        if(!name_tmp.compare(name, Qt::CaseInsensitive)) {
            _cur_idx = i;
        }
    }
}



void NotificationPluginLoader::set_cur_plugin(Notification* n) {

    _cur_idx = -1;
    if(!n) return;

    set_cur_plugin(n->get_name());
}

Notification* NotificationPluginLoader::get_cur_plugin() {
    if(_notification_plugins.size() == 0) return NULL;
    if(_cur_idx < 0 || _cur_idx > _notification_plugins.size()) return NULL;

    return _notification_plugins[_cur_idx];
}

        

        

