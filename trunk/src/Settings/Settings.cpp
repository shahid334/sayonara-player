/* Settings.cpp */

/* Copyright (C) 2011  Lucio Carreras
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


#include "HelperStructs/Helper.h"
#include "HelperStructs/PlaylistMode.h"
#include "Settings/Settings.h"
#include "DatabaseAccess/CDatabaseConnector.h"

Settings* Settings::getInstance() {
	static Settings inst;
    return &inst;
}

Settings::Settings(){
	_db_file = "player.db";
	memset(_settings, 0, SK::Num_Setting_Keys + 1);
}


Settings::~Settings () {
}


AbstrSetting** Settings::get_settings(){
	return _settings;
}


void Settings::register_setting(AbstrSetting* s){

	SK::SettingKey key  = s->get_key();
	_settings[ (int) key ] = s;
}


bool Settings::check_settings(){

	QList<int> un_init;
	for(int i=0; i<SK::Num_Setting_Keys; i++){
		if(! _settings[i] ){
			un_init << i;
		}
	}

	if( !un_init.empty() ){
		qDebug() << "**** Settings " << un_init << " are not initialized ****";
		return false;
	}

	else{
		qDebug() << "**** All settings initialized ****";
	}

	return true;
}
