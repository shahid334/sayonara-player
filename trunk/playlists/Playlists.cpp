/*
 * Playlists.cpp
 *
 *  Created on: Jan 2, 2012
 *      Author: luke
 */


#include "playlists/Playlists.h"
#include "HelperStructs/MetaData.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QMap>
#include <QDebug>
#include <QMessageBox>

#include <vector>
using namespace std;

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

	else{
		qDebug() << "playlists could not be fetched";
	}
}

void Playlists::load_single_playlist(int id){
	CustomPlaylist pl;
	QString playlist_name = CDatabaseConnector::getInstance()->getPlaylistNameById(id);
	bool success = CDatabaseConnector::getInstance()->getPlaylistById(id, pl);

	if(success){
		qDebug() << "Playlist " << playlist_name << " loaded";
		emit sig_single_playlist_loaded(pl);
	}

	else{
		qDebug() << "Could not load playlist " << playlist_name;
	}
}

bool Playlists::check_for_extern_track(const vector<MetaData>& src, vector<MetaData>& tgt){

	tgt = src;

	bool extern_track_found = false;
	foreach(MetaData data, src){
		if(data.is_extern){
			extern_track_found = true;
			break;
		}
	}


	if(extern_track_found){
		vector<MetaData> v_md_save = src;
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

				int id = db->getTrackByPath(v_md_save[i].filepath);
				if(id > 0){
					MetaData md = v_md_save.at(i);
					md.id = id;
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


void Playlists::save_playlist_as_custom(int id, vector<MetaData>& vec_md){

	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	vector<MetaData> vec_md_copy;
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



void Playlists::save_playlist_as_custom(QString name, vector<MetaData>& vec_md){

	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	vector<MetaData> vec_md_copy;
	bool success = check_for_extern_track(vec_md, vec_md_copy);
	if(!success) return;

	success = db->storePlaylist(vec_md_copy, name);
	if(success){
		qDebug() << "Saved playlist as " << name;
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
