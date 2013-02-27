/* SoundPluginLoader.cpp */

/* Copyright (C) 2012  Lucio Carreras
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

SoundPluginLoader::SoundPluginLoader(QString app_dir){
	_cur_engine = -1;
	_cur_engine_name = "";
	bool success = load_plugins(app_dir);
	if(!success){
		qDebug() << "No sound engine available";
	}
}

SoundPluginLoader::~SoundPluginLoader(){

}

bool SoundPluginLoader::load_plugins(QString app_dir){

	QString target_engine = CSettingsStorage::getInstance()->getSoundEngine().toLower();

	QDir plugin_dir = QDir(app_dir);

	int i=0;

	foreach(QObject* plugin, QPluginLoader::staticInstances() ){

		if(plugin){
			Engine* plugin_eng =  qobject_cast<Engine*>(plugin);
			if(plugin_eng){
				QString name = plugin_eng->getName().toLower();
                qDebug() << "Found plugin " << plugin_eng->getName();
				_lst_engines.push_back(plugin_eng);
				if(name == target_engine){
					_cur_engine = i;
					_cur_engine_name = name;
				}

				i++;
			}
		}
	}


	// dynamic plugins
	foreach(QString filename, plugin_dir.entryList(QDir::Files)){
		QPluginLoader loader(plugin_dir.absoluteFilePath(filename));
		QObject* plugin = loader.instance();
		if(plugin){
			Engine* plugin_eng =  qobject_cast<Engine*>(plugin);
			if(plugin_eng){
				QString name = plugin_eng->getName().toLower();
				qDebug() << "Found plugin " << plugin_eng->getName();
				_lst_engines.push_back(plugin_eng);
				if(name == target_engine){
					_cur_engine = i;
					_cur_engine_name = name;
				}

				i++;
			}
		}
	}

	qDebug() << "Num plugins found: " << _lst_engines.size();

	if(_lst_engines.size() == 0) {
		_cur_engine_name = "";
		return false;
	}

	if(_cur_engine < 0){
		_cur_engine = 0;
		_cur_engine_name = _lst_engines.at(0)->getName().toLower();
	}

	return true;
}


Engine* SoundPluginLoader::get_cur_engine(){
	if(_cur_engine < 0) return NULL;
	return _lst_engines.at(_cur_engine);
}


void SoundPluginLoader::psl_switch_engine(QString& new_engine){

	if(new_engine.toLower() == _cur_engine_name.toLower()) return;

	qDebug() << "Change Engine to " << new_engine;

	bool engine_found = false;
	for(int i=0; i<_lst_engines.size(); i++){
		Engine* engine = _lst_engines.at(i);

		if(new_engine == engine->getName()){
			_cur_engine_name = engine->getName();
			_cur_engine = i;
			engine_found = true;
			break;
		}
	}

	if(engine_found){
		QMessageBox::information(NULL, "Info", QString("Please restart in order to change engine to ") + new_engine);
		CSettingsStorage::getInstance()->setSoundEngine(_cur_engine_name);
	}

}
