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
#include "HelperStructs/MetaData/MetaData.h"

#include <QMessageBox>

PlaylistChooser::PlaylistChooser() {

	_playlist_handler = PlaylistHandler::getInstance();

	_playlist_db_connector = PlaylistDBConnector::getInstance();

	connect(_playlist_handler, SIGNAL(sig_playlists_changed()),
			this, SLOT(load_all_playlists()));
}

PlaylistChooser::~PlaylistChooser() {

}

void PlaylistChooser::load_all_playlists() {

	bool success;
	_skeletons.clear();
	success = _playlist_db_connector->get_non_temporary_skeletons(_skeletons, Sort::NameAsc);

	if(success) {
		emit sig_all_playlists_loaded(_skeletons);
	}
}

CustomPlaylist PlaylistChooser::find_custom_playlist(int id){

	CustomPlaylist pl = _playlist_db_connector->get_playlist_by_id(id);
	return pl;

}

void PlaylistChooser::load_single_playlist(int id) {

	if(id < 0) {
		return;
	}

	CustomPlaylist pl = find_custom_playlist(id);
	if(!pl.is_valid) {
		return;
	}

	_playlist_handler->create_playlist(pl);
}


void PlaylistChooser::delete_playlist(int id) {

	bool success;

	success = _playlist_db_connector->delete_playlist(id);

	if(!success) {
		qDebug() << "playlist " << id << " could not be deleted";
	}

	load_all_playlists();
}


void PlaylistChooser::save_playlist(QString name){
	_playlist_handler->save_cur_playlist_as(name, true);
}

void PlaylistChooser::save_playlist_file(QString filename, bool relative_paths){
	_playlist_handler->save_playlist(filename, relative_paths);
}


void PlaylistChooser::clear_playlist(){
	_playlist_handler->clear_playlist();
}


void PlaylistChooser::playlist_files_selected(const QStringList& lst){
	_playlist_handler->create_playlist(lst, "", false);
}

