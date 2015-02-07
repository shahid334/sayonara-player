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
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/PlaylistParser.h"

StdPlaylist::StdPlaylist(int idx) :
	Playlist(idx)
{
    _playlist_type = PlaylistTypeStd;
}


bool StdPlaylist::change_track(int idx) {

	// ERROR: invalid idx
    if( idx >= _v_md.size() || idx < 0) {

        _cur_play_idx = -1;
        _v_md.setCurPlayTrack(_cur_play_idx);

        stop();
		return false;
    }

	// ERROR: track not available in file system anymore
    if( !Helper::checkTrack(_v_md[idx]) ) {

        _v_md[idx].is_disabled = true;

		return change_track(idx + 1);
    }

	// OK: update track
    else{
        _cur_play_idx = idx;
        _v_md.setCurPlayTrack(_cur_play_idx);
    }

	return true;
}

int StdPlaylist::create_playlist(const MetaDataList& v_md) {

    CDatabaseConnector* db = CDatabaseConnector::getInstance();

	_cur_play_idx = -1;

    if(!_playlist_mode.append) {
        _cur_play_idx = -1;

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

        _v_md.push_back(md);
    }

    if(!_playlist_mode.append){
        _cur_play_idx = -1;
    }

	return _v_md.size();
}

void StdPlaylist::play() {

	if( _v_md.isEmpty() ) {
        _cur_play_idx = -1;
        stop();
        return;
    }

    if(_cur_play_idx == -1) {
        _cur_play_idx = 0;
        _v_md.setCurPlayTrack(_cur_play_idx);
    }
}

void StdPlaylist::pause() {

}

void StdPlaylist::stop() {

    _cur_play_idx = -1;
    for(int i=0; i<(int) _v_md.size(); i++) {
        _v_md[i].pl_playing = false;
    }
}

void StdPlaylist::fwd() {

	bool rep1 = _playlist_mode.rep1;
	_playlist_mode.rep1 = false;

	next();

	_playlist_mode.rep1 = rep1;

}

void StdPlaylist::bwd() {

	change_track( _cur_play_idx - 1 );
}


void StdPlaylist::next() {

    int track_num = -1;

    if(_v_md.size() == 0) {
		stop();
        return;
    }

	if(_playlist_mode.rep1){
		track_num = _cur_play_idx;
	}

    // shuffle mode
	else if(_playlist_mode.shuffle) {
        track_num = rand() % _v_md.size();
        if(track_num == _cur_play_idx) {
            track_num = (_cur_play_idx + 1) % _v_md.size();
        }
    }

    else {

		if(_cur_play_idx >= (int) _v_md.size() -1) {

			if(_playlist_mode.repAll){
				track_num = 0;
			}

			else {
				stop();
				return;
			}
        }

        else{
            track_num = _cur_play_idx + 1;
            if(track_num == _cur_play_idx) {
                stop();
                return;
            }
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


void StdPlaylist::save_to_m3u_file(QString filepath, bool relative) {
    PlaylistParser::save_playlist(filepath, _v_md, relative);
}
