/* Playlists.cpp */

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



#include "playlists/Playlists.h"
#include "HelperStructs/MetaData.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QMap>
#include <QDebug>
#include <QMessageBox>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif



Playlists::Playlists() {
	_import_state = STATE_IMPORT_SUCCESS;
}

Playlists::~Playlists() {
	// TODO Auto-generated destructor stub
}

void Playlists::load_all_playlists(){

	_mapping.clear();
	bool success = CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);

	if(success){
		emit sig_all_playlists_loaded(_mapping);
	}
}

void Playlists::load_single_playlist(int id){
	CustomPlaylist pl;
    bool success = CDatabaseConnector::getInstance()->getPlaylistById(id, pl);

	if(success){
		emit sig_single_playlist_loaded(pl);
	}
}

bool Playlists::check_for_extern_track(const MetaDataList& src, MetaDataList& tgt){

	tgt = src;

	bool extern_track_found = false;
	foreach(MetaData data, src){
		if(data.is_extern){
			extern_track_found = true;
			break;
		}
	}


	if(extern_track_found){
		MetaDataList v_md_save = src;
		_import_state = STATE_WAIT;

		int info = QMessageBox::information(NULL,
				"Some tracks are not in library",
				QString("All tracks must be located in library.\n") +
				"Would you like to import missing tracks?",
				QMessageBox::Yes | QMessageBox::No);

		if(info == QMessageBox::Yes){
			emit sig_import_tracks(src);
		}

		else{
			return false;
		}

		// wait until importing is finished
		// 2 mins
		long int max = 30000000;
		while(_import_state == STATE_WAIT){
			qDebug() << "waiting..." << _import_state;
			usleep(1000000);
			max -= 1000000;
			if(max <= 0){
				_import_state = STATE_IMPORT_FAIL;
				return false;
			}
		}

		if(_import_state == STATE_IMPORT_FAIL) return -1;
		else if(_import_state == STATE_IMPORT_SUCCESS){

			tgt.clear();
			CDatabaseConnector* db = CDatabaseConnector::getInstance();
			for(uint i=0; i<v_md_save.size(); i++){

				MetaData md_tmp = db->getTrackByPath(v_md_save[i].filepath);
				if(md_tmp.id > 0){
					MetaData md = v_md_save.at(i);
					md.id = md_tmp.id;
					tgt.push_back(md);
				}
			}
		}
	}

	return true;
}

void Playlists::import_result(bool success){

	if(_import_state != STATE_WAIT) return;

	if(success) _import_state = STATE_IMPORT_SUCCESS;
	else _import_state = STATE_IMPORT_FAIL;
}


void Playlists::save_playlist_as_custom(int id, MetaDataList& vec_md){

	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	MetaDataList vec_md_copy;
	bool success = check_for_extern_track(vec_md, vec_md_copy);
	if(!success)
		return;

	success = db->storePlaylist(vec_md_copy, id);
	if(success){
		qDebug() << "Saved playlist as " << id;
		_mapping.clear();
		CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);
		emit sig_all_playlists_loaded(_mapping);
	}

	else{
		qDebug() << "Could not save playlist " << id;
	}

	_import_state = STATE_IMPORT_SUCCESS;
}



void Playlists::save_playlist_as_custom(QString name, MetaDataList& vec_md){

	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	MetaDataList vec_md_copy;
	bool success = check_for_extern_track(vec_md, vec_md_copy);
	if(!success) return;

	success = db->storePlaylist(vec_md_copy, name);
	if(success){
		_mapping.clear();
		CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);
		emit sig_all_playlists_loaded(_mapping);
	}

	else{
		qDebug() << "Could not save playlist " << name;
	}

	_import_state = STATE_IMPORT_SUCCESS;
}

void Playlists::delete_playlist(int id){

	QString playlist_name = CDatabaseConnector::getInstance()->getPlaylistNameById(id);
	bool success = CDatabaseConnector::getInstance()->deletePlaylist(id);

	if(success){
		qDebug() << "playlist " << playlist_name << " deleted";
	}

	else {
		qDebug() << "playlist " << playlist_name << " could not be deleted";
	}

	_mapping.clear();
	CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);
	emit sig_all_playlists_loaded(_mapping);
}

void Playlists::ui_loaded(){
	bool success = CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);
	if(success){
		sig_all_playlists_loaded(_mapping);
	}
}
