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
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/MetaData.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QMap>
#include <QDebug>
#include <QMessageBox>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif



Playlists::Playlists() {

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

    // empty loaded (load old playlist)
    if(id <= 0) {
        pl.is_valid = false;
        emit sig_single_playlist_loaded(pl);
        return;
    }

    bool success = CDatabaseConnector::getInstance()->getPlaylistById(id, pl);
    qDebug() << "in my stommack are " << pl.tracks.size() << "files";

    MetaDataList v_md;
    pl.length = 0;
    pl.num_tracks = 0;
    foreach(MetaData md, pl.tracks){
        if(md.is_extern){
            if(!ID3::getMetaDataOfFile(md)){
                continue;
            }
        }

        pl.length += md.length_ms / 1000;
        pl.num_tracks ++;
        v_md.push_back(md);
    }

    qDebug() << "loaded " << v_md.size() << " tracks";

    pl.tracks = v_md;
    pl.is_valid = success;
	if(success){
		emit sig_single_playlist_loaded(pl);
	}
}



void Playlists::save_playlist_as_custom(int id, MetaDataList& vec_md){

	CDatabaseConnector* db = CDatabaseConnector::getInstance();
    qDebug() << "save " << vec_md.size() << " files to " << id;

    bool success = db->storePlaylist(vec_md, id);
	if(success){
		qDebug() << "Saved playlist as " << id;
		_mapping.clear();
		CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);
		emit sig_all_playlists_loaded(_mapping);
	}

	else{
		qDebug() << "Could not save playlist " << id;
	}
}



void Playlists::save_playlist_as_custom(QString name, MetaDataList& vec_md){

	CDatabaseConnector* db = CDatabaseConnector::getInstance();

    qDebug() << "save " << vec_md.size() << " files to " << name;
    bool success = db->storePlaylist(vec_md, name);
	if(success){
		_mapping.clear();
		CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);
		emit sig_all_playlists_loaded(_mapping);
	}

	else{
		qDebug() << "Could not save playlist " << name;
	}
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
