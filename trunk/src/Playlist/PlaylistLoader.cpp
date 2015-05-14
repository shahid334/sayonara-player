/* PlaylistLoader.cpp */

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



#include "Playlist/PlaylistLoader.h"
#include "HelperStructs/MetaData/MetaData.h"
#include "HelperStructs/Tagging/id3.h"
#include "Soundcloud/SoundcloudHelper.h"

#include <QDir>

PlaylistLoader::PlaylistLoader(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{

	_playlist_handler = PlaylistHandler::getInstance();
	_playlist_db_connector = PlaylistDBConnector::getInstance();
}


int PlaylistLoader::load_old_playlists() {

	QList<CustomPlaylist> playlists;

	bool success;
	int saved_playlist_id = _settings->get(Set::PL_LastPlaylist);
	int saved_track_idx = _settings->get(Set::PL_LastTrack);
	bool last_playlist_found = false;

	int playlist_idx = -1;
	int track_idx = -1;

	bool load_saved_playlists = _settings->get(Set::PL_LoadSavedPlaylists);

	success = _playlist_db_connector->get_all_playlists(playlists);
	if(!success){
		return 0;
	}

	int n_playlists=0;
	for(const CustomPlaylist& pl : playlists){

		bool add_playlist = true;

		if(!success){
			continue;
		}

		if(!load_saved_playlists && !pl.is_temporary){
			add_playlist = false;
		}

		if(pl.id == saved_playlist_id){

			playlist_idx = n_playlists;

			if( saved_track_idx >= 0 && saved_track_idx < pl.tracks.size()){
				track_idx = saved_track_idx;
				add_playlist = true;
				last_playlist_found = true;
			}
		}

		if(add_playlist){

			n_playlists++;
			_playlist_handler->create_playlist(pl);
		}
	}

	if(!last_playlist_found){
		_settings->set(Set::Engine_CurTrackPos_s, 0);
		PlayManager::getInstance()->seek_abs_ms(0);
	}

	else{

	}

	if( n_playlists == 0 ){
		// do nothing
		;
	}


	else if(playlist_idx != -1){
		_playlist_handler->change_playlist_index(playlist_idx);

		// perfect
		if(track_idx != -1){
			_playlist_handler->change_track(track_idx, playlist_idx);
			PlayManager::getInstance()->seek_abs_ms(_settings->get(Set::Engine_CurTrackPos_s) * 1000);
			_settings->shout(Set::Engine_CurTrackPos_s);
		}

		// playlist, but no track...
		else{
			_playlist_handler->change_track(0, playlist_idx);
		}
	}

	// playlist not found -> track not found
	else{
		_playlist_handler->change_playlist_index(0);
		_playlist_handler->change_track(0, 0);
	}

	return n_playlists;
}
