/* StdPlaylist.cpp */

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

#include "StdPlaylist.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/DirectoryReader/DirectoryReader.h"
#include "HelperStructs/Parser/PlaylistParser.h"

#include <random>

StdPlaylist::StdPlaylist(int idx, QString name) :
	Playlist(idx, name)
{
    _playlist_type = PlaylistTypeStd;
}


bool StdPlaylist::change_track(int idx) {

	_v_md.setCurPlayTrack(idx);

	// ERROR: invalid idx
    if( idx >= _v_md.size() || idx < 0) {
        stop();
		return false;
    }

	// ERROR: track not available in file system anymore
    if( !Helper::checkTrack(_v_md[idx]) ) {

        _v_md[idx].is_disabled = true;

		return change_track(idx + 1);
    }

	return true;
}

int StdPlaylist::create_playlist(const MetaDataList& v_md) {

    CDatabaseConnector* db = CDatabaseConnector::getInstance();
	_v_md.setCurPlayTrack(-1);

    if(!_playlist_mode.append) {
        clear();
    }

    // no tracks in new playlist
	if(v_md.size() == 0){
		return 0;
	}

    // check if there, check if extern
	foreach(MetaData md, v_md) {

		MetaData md_tmp = db->getTrackByPath(md.filepath());

		md.is_extern = ( md_tmp.id < 0 );
        md.is_disabled = (! Helper::checkTrack(md) );
		md.pl_playing = false;

        _v_md.push_back(md);
    }

	return _v_md.size();
}

void StdPlaylist::play() {

	if( _v_md.isEmpty() ) {
        stop();
        return;
    }

	if(_v_md.getCurPlayTrack() == -1) {
		_v_md.setCurPlayTrack(0);
    }
}

void StdPlaylist::pause() {

}

void StdPlaylist::stop() {
	_v_md.setCurPlayTrack(-1);
}

void StdPlaylist::fwd() {

	bool rep1 = _playlist_mode.rep1;
	_playlist_mode.rep1 = false;

	next();

	_playlist_mode.rep1 = rep1;

}

void StdPlaylist::bwd() {

	int cur_idx = _v_md.getCurPlayTrack();
	change_track( cur_idx - 1 );
}


void StdPlaylist::next() {

	int cur_track = _v_md.getCurPlayTrack();
	int track_num = -1;

	// no track
	if(_v_md.size() == 0 ) {
		stop();
        return;
    }

	std::mt19937 rnd_engine;
	std::uniform_int_distribution<int> distr(0, _v_md.size() - 1);

	// stopped
	if(track_num == -1){
		track_num = 0;
	}

    // shuffle mode
	if(_playlist_mode.shuffle) {

		if(_v_md.size() > 1){
			int rnd = distr(rnd_engine) + 1;
			track_num = (cur_track + rnd) % _v_md.size();
		}

		else{
			track_num = (cur_track + 1) % _v_md.size();
		}
    }

	// normal track
    else {

		// last track
		if(cur_track == _v_md.size() - 1){

			if(_playlist_mode.repAll){
				track_num = 0;
			}

			else {
				stop();
				return;
			}
        }

        else{
			track_num = cur_track + 1;
        }
    }

	change_track(track_num);
}


void StdPlaylist::metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new) {

	for(int i=0; i<_v_md.size(); i++) {

		_v_md[i].pl_selected = false;

		for(int idx=0; idx<v_md_old.size(); idx++){

			if( v_md_old[idx].is_equal( _v_md[i]) ){
				_v_md[i] = v_md_new[idx];
				_v_md[i].pl_selected = true;

				break;
			}
        }
    }
}
