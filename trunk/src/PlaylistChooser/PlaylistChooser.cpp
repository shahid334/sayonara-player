/* PlaylistChooser.cpp */

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


#include "PlaylistChooser/PlaylistChooser.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/MetaData.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QMap>
#include <QDebug>
#include <QMessageBox>

PlaylistChooser::PlaylistChooser(PlaylistHandler* playlist_handler) {

	_playlist_handler = playlist_handler;

	connect (_playlist_handler, SIGNAL(sig_playlist_prepared(int, const MetaDataList&)),
			 this,	SLOT(save_playlist_as_custom(int, const MetaDataList&)));

	connect (_playlist_handler, SIGNAL(sig_playlist_prepared(QString, const MetaDataList&)),
			 this,	SLOT(save_playlist_as_custom(QString, const MetaDataList&)));

	connect(_playlist_handler, SIGNAL(sig_playlist_created(const MetaDataList&, int, PlaylistType)),
			this, SLOT(playlist_created(const MetaDataList&, int, PlaylistType)));
}

PlaylistChooser::~PlaylistChooser() {

}

void PlaylistChooser::load_all_playlists() {

	_mapping.clear();
	bool success = CDatabaseConnector::getInstance()->getAllPlaylistChooser(_mapping);

	if(success) {
		emit sig_all_playlists_loaded(_mapping);
	}
}

void PlaylistChooser::load_single_playlist(int id, QString name) {

	CustomPlaylist pl;

    // empty loaded (load old playlist)
    if(id <= 0) {
        pl.is_valid = false;
        return;
    }

    bool success = CDatabaseConnector::getInstance()->getPlaylistById(id, pl);

    MetaDataList v_md;
    pl.length = 0;
    pl.num_tracks = 0;
    foreach(MetaData md, pl.tracks) {
        if(md.is_extern) {
            if(!ID3::getMetaDataOfFile(md)) {
                continue;
            }
        }

        pl.length += md.length_ms / 1000;
        pl.num_tracks ++;
        v_md.push_back(md);
    }

    pl.tracks = v_md;
    pl.is_valid = success;

	if(success) {
		int idx = _playlist_handler->add_new_playlist(name);
		_playlist_handler->change_playlist_index(idx);
		_playlist_handler->create_playlist(pl);
	}
}



void PlaylistChooser::save_playlist_as_custom(int id, const MetaDataList& vec_md) {

	CDatabaseConnector* db = CDatabaseConnector::getInstance();

    bool success = db->storePlaylist(vec_md, id);
	if(success) {

		_mapping.clear();
		db->getAllPlaylistChooser(_mapping);
		emit sig_all_playlists_loaded(_mapping);
	}

	else{
		qDebug() << "Could not save playlist " << id;
	}
}



void PlaylistChooser::save_playlist_as_custom(QString name, const MetaDataList& vec_md) {

	CDatabaseConnector* db = CDatabaseConnector::getInstance();

    bool success = db->storePlaylist(vec_md, name);
	if(success) {
		_mapping.clear();
		CDatabaseConnector::getInstance()->getAllPlaylistChooser(_mapping);
		emit sig_all_playlists_loaded(_mapping);
	}

	else{
		qDebug() << "Could not save playlist " << name;
	}
}

void PlaylistChooser::delete_playlist(int id) {

	QString playlist_name = CDatabaseConnector::getInstance()->getPlaylistNameById(id);
	bool success = CDatabaseConnector::getInstance()->deletePlaylist(id);

	if(!success) {
		qDebug() << "playlist " << playlist_name << " could not be deleted";
	}

	_mapping.clear();
	CDatabaseConnector::getInstance()->getAllPlaylistChooser(_mapping);
	emit sig_all_playlists_loaded(_mapping);
}


void PlaylistChooser::save_playlist(int id){
	_playlist_handler->psl_prepare_playlist_for_save(id);
}


void PlaylistChooser::save_playlist(QString playlist_name){
	_playlist_handler->psl_prepare_playlist_for_save(playlist_name);
}


void PlaylistChooser::save_playlist_file(QString filename, bool relative_paths){
	_playlist_handler->psl_save_playlist(filename, relative_paths);
}


void PlaylistChooser::clear_playlist(){
	_playlist_handler->psl_clear_playlist();
}


void PlaylistChooser::playlist_files_selected(const QStringList& lst){
	_playlist_handler->create_playlist(lst);
}


void PlaylistChooser::playlist_created(const MetaDataList& v_md, int cur_idx, PlaylistType type){
	emit sig_playlist_created(v_md, cur_idx, type);
}
