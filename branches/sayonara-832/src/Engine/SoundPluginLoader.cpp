/* SoundPluginLoader.cpp */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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


#include "Engine/SoundPluginLoader.h"
#include "Engine/Engine.h"
#include "Engine/GStreamer/GSTPlaybackEngine.h"
#include "HelperStructs/CSettingsStorage.h"

#include <QObject>
#include <QDir>
#include <QString>
#include <QDebug>
#include <QPluginLoader>
#include <QMessageBox>
#include <QList>
#include <qplugin.h>


using namespace std;

//Q_IMPORT_PLUGIN(sayonara_gstreamer)

SoundPluginLoader::SoundPluginLoader(QString app_dir) {
	bool success = load_plugins(app_dir);
	if(!success) {
		qDebug() << "No sound engine available";
	}
}

SoundPluginLoader::~SoundPluginLoader() {

}

bool SoundPluginLoader::load_plugins(QString app_dir) {

	QString target_engine = CSettingsStorage::getInstance()->getSoundEngine().toLower();

	QDir plugin_dir = QDir(app_dir);
	QStringList entry_list = plugin_dir.entryList(QDir::Files);


	foreach(QObject* plugin, QPluginLoader::staticInstances() ) {

		if(plugin) {
			Engine* plugin_eng =  qobject_cast<Engine*>(plugin);
			if(plugin_eng) {
				QString name = plugin_eng->getName().toLower();
				_vec_engines.push_back(plugin_eng);

			}
		}
	}


	// dynamic plugins
	foreach(QString filename, entry_list) {
		qDebug() << "possible plugin: " << plugin_dir.absoluteFilePath(filename);
		QPluginLoader loader(plugin_dir.absoluteFilePath(filename));

		QObject* plugin = loader.instance();

		if(!plugin) {
			qDebug() << loader.errorString();
			continue;
		}

		qDebug() << "Plugin looks useful... ";

		Engine* plugin_eng =  qobject_cast<Engine*>(plugin);


		if(plugin_eng) {
			QString name = plugin_eng->getName().toLower();
			qDebug() << "Found engine " << plugin_eng->getName();
			_vec_engines.push_back(plugin_eng);
		}

		else {
			qDebug() << "Cannot convert object to Engine ";
		}

	}

	qDebug() << "Num engines found: " << _vec_engines.size();

	if(_vec_engines.size() == 0) {
		return false;
	}

	return true;
}

vector<Engine*> SoundPluginLoader::get_engines() {
	return _vec_engines;
}

Engine* SoundPluginLoader::get_first_engine() {
	if(_vec_engines.size() == 0) return NULL;

	return _vec_engines[0];

}

