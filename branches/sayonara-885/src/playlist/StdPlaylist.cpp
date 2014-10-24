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

StdPlaylist::StdPlaylist(QObject *parent) :
    Playlist(parent)
{
    _playlist_type = PlaylistTypeStd;
    _last_track = -1;
}


void StdPlaylist::change_track(int idx) {

    _last_track = _cur_play_idx;

    if( idx >= _v_md.size() || idx < 0) {

        _cur_play_idx = -1;
        _v_md.setCurPlayTrack(_cur_play_idx);

        stop();
        return;
    }

    // track not available in file system anymore
    if( !Helper::checkTrack(_v_md[idx]) ) {

        _v_md[idx].is_disabled = true;

        change_track(idx + 1);

        return;
    }

    else{
        _cur_play_idx = idx;
        _v_md.setCurPlayTrack(_cur_play_idx);
    }

    _last_track = _cur_play_idx;

    report_changes(false, true);
}

void StdPlaylist::create_playlist(const MetaDataList& lst, bool start_playing) {

    CDatabaseConnector* db = CDatabaseConnector::getInstance();

    if(!_playlist_mode.append) {
        _cur_play_idx = -1;
        disable_reports();
        clear();
        enable_reports();
    }

    // no tracks in new playlist
    if(lst.size() == 0)
        return;

    // check if there, check if extern
    foreach(MetaData md, lst) {

        MetaData md_tmp = db->getTrackByPath(md.filepath);

        md.is_extern = (md_tmp.id < 0);
        md.is_disabled = (! Helper::checkTrack(md) );

        _v_md.push_back(md);
    }

    if(!_playlist_mode.append){
        _cur_play_idx = -1;
    }

    report_changes(true, start_playing);
}

void StdPlaylist::create_playlist(const QStringList& lst, bool start_playing) {

    MetaDataList v_md;

    CDirectoryReader reader;
    reader.getMetadataFromFileList(lst, v_md);

    create_playlist(v_md, start_playing);
}

void StdPlaylist:: play() {

    if(_v_md.size() == 0) {
        _cur_play_idx = -1;
        stop();
        return;
    }

    if(_cur_play_idx == -1) {
        _cur_play_idx = 0;
        _v_md.setCurPlayTrack(_cur_play_idx);
    }

    report_changes(false, true);
}

void StdPlaylist::pause() {

}

void StdPlaylist::stop() {

    _cur_play_idx = -1;
    for(int i=0; i<(int) _v_md.size(); i++) {
        _v_md[i].pl_playing = false;
    }

    report_changes(true, true);
}

void StdPlaylist::fwd() {
    this->next();
}

void StdPlaylist::bwd() {

    int track_num;
    if(_playlist_mode.shuffle && _last_track != -1 ) {
        track_num = _last_track;
    }

    else {
        track_num = _cur_play_idx - 1;
    }

    change_track(track_num);
}


void StdPlaylist::next() {

    _last_track = _cur_play_idx;
    int track_num = -1;
    if(_v_md.size() == 0) {
		stop();
        return;
    }

    // shuffle mode
    if(_playlist_mode.shuffle) {
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


void StdPlaylist::metadata_changed(const MetaDataList& md_list) {

    bool sth_changed = false;
    for(int i=0; i<(int) _v_md.size(); i++) {

        MetaData md_old = _v_md[i];

        foreach(MetaData md_new, md_list) {
            QString old_path = QDir(md_old.filepath).absolutePath();
            QString new_path = QDir(md_new.filepath).absolutePath();
            int cmp = old_path.compare(new_path, Qt::CaseInsensitive);
            if(cmp == 0) {

                _v_md[i] = md_new;
                sth_changed = true;
            }
        }
    }

    if(sth_changed) report_changes(true, false);
}


void StdPlaylist::save_for_reload() {

    QStringList playlist_lst;
    foreach(MetaData md, _v_md) {

        if(md.id >= 0) {
            playlist_lst << QString::number(md.id);
        }

        else{
            playlist_lst << md.filepath;
        }
    }

    CSettingsStorage::getInstance()->setPlaylist(playlist_lst);
}

void StdPlaylist::save_to_m3u_file(QString filepath, bool relative) {
    PlaylistParser::save_playlist(filepath, _v_md, relative);
}
