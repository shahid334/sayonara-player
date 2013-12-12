/* Playlist.cpp */

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
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */

#include "Playlist.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/PlaylistParser.h"
#include "HelperStructs/PodcastParser/PodcastParser.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/CDirectoryReader.h"

#include <QFile>
#include <QList>
#include <QObject>
#include <QDate>
#include <QTime>
#include <QDebug>

#include <iostream>
#include <ctime>

using namespace std;

Playlist::Playlist(QObject * parent) : QObject (parent){

    _radio_active = RadioOff;
    _settings = CSettingsStorage::getInstance();
    _playlist_mode = _settings->getPlaylistMode();
	_db = CDatabaseConnector::getInstance();
	_v_meta_data.clear();
    _is_playing = false;
    _pause = false;


}

Playlist::~Playlist() {

}


// create a playlist, where metadata is already available
void Playlist::psl_createPlaylist(MetaDataList& v_meta_data){


    bool instant_play = ((_v_meta_data.size() == 0) && (!_is_playing));

    if(!_playlist_mode.append){
        _v_meta_data.clear();
        _cur_play_idx = -1;
    }

    // no tracks in new playlist
    if(v_meta_data.size() == 0) {

        emit sig_playlist_created(_v_meta_data, _cur_play_idx, RadioOff);
        return;
    }

    int old_radio_active = _radio_active;
    // check if there, check if extern
    foreach(MetaData md, v_meta_data){

        if( !checkTrack(md) ) return;

        MetaData md_tmp = _db->getTrackByPath(md.filepath);

        if(md.radio_mode == RadioOff){
            md.is_extern = (md_tmp.id < 0 && !Helper::is_www(md.filepath));
            _radio_active = RadioOff;
        }

        else
            md.is_extern = false;

        _v_meta_data.push_back(md);

    }

    if(_radio_active != old_radio_active){
        _cur_play_idx = 0;
        send_cur_playing_signal(_cur_play_idx);
    }

    if(_backup_playlist.is_valid)
        _cur_play_idx = _backup_playlist.cur_play_idx;

    emit sig_playlist_created(_v_meta_data, _cur_play_idx, _radio_active);

    if(instant_play)
        send_cur_playing_signal(0);

    psl_save_playlist_to_storage();
}




// create a new playlist, where only filepaths are given
// Load Folder, Load File...
void Playlist::psl_createPlaylist(QStringList& pathlist){

    _radio_active = RadioOff;
    MetaDataList v_md;

    CDirectoryReader reader;
    reader.getMetadataFromFileList(pathlist, v_md);

    psl_createPlaylist(v_md);
}


// create playlist from saved custom playlist
void Playlist::psl_createPlaylist(CustomPlaylist& pl){

    // save old playlist
    if(pl.is_valid){
        _backup_playlist.v_md = _v_meta_data;
        _backup_playlist.radio_mode = _radio_active;
        _backup_playlist.is_valid = true;
        _backup_playlist.cur_play_idx = _cur_play_idx;
        _backup_playlist.is_valid = false;
        _radio_active = RadioOff;

        psl_createPlaylist(pl.tracks);
    }

    // load old playlist
    else{
        _radio_active = _backup_playlist.radio_mode;
        _backup_playlist.is_valid = true;
        psl_createPlaylist(_backup_playlist.v_md);

    }
}


void Playlist::send_cur_playing_signal(int i){
    _is_playing = true;

    if(!Helper::checkTrack(_v_meta_data[i])) return;

    emit sig_selected_file_changed(i);
    emit sig_selected_file_changed_md(_v_meta_data[i]);
    _cur_play_idx = i;
}


void Playlist::load_old_playlist(){

    if(_v_meta_data.size() > 0) return;

    bool loadPlaylist = _settings->getLoadPlaylist();
    bool loadLastTrack = _settings->getLoadLastTrack();
    LastTrack* last_track = _settings->getLastTrack();
    bool load_last_position = _settings->getRememberTime();
    bool start_immediatly = _settings->getStartPlaying();

    //qDebug() << "Last track has been " << last_track->filepath;

    if( !loadPlaylist ) return;

    QStringList saved_playlist = _settings->getPlaylist();

    if(saved_playlist.size() == 0) return;

    // the path of the last played track
        QString last_track_path = last_track->filepath;
        QDir d2(last_track_path);
        last_track_path = d2.absolutePath();

    int last_track_idx = -1;

    // run over all tracks
    for(int i=0; i<saved_playlist.size(); i++){

        // convert item into MetaData
        QString item = saved_playlist[i];
        if(item.size() == 0) continue;

        // maybe we can get a track id
        bool ok;
        int track_id = item.toInt(&ok);

        // maybe it's an filepath
            QString path_in_list = item;
            QDir d(path_in_list);
            path_in_list = d.absolutePath();


        MetaData track;
        CDatabaseConnector* db = CDatabaseConnector::getInstance();

        // we have a track id
        if(track_id >= 0 && ok){
            track = db->getTrackById(track_id);

            // this track id cannot be found in db
            if(track.id < 0){
                if(!ID3::getMetaDataOfFile(track)) continue;
                track.is_extern = true;
            }

            else
                track.is_extern = false;

            if(track_id == last_track->id)
                last_track_idx = i;
        }

        // we have an filepath
        else{
            if(!QFile::exists(path_in_list)) continue;

            // maybe it's in the library neverthe less
            track = db->getTrackByPath(path_in_list);
            // we expected that.. try to get metadata
            if(track.id < 0){
                if(!ID3::getMetaDataOfFile(track)) continue;
            }

            track.is_extern = true;

            if(!path_in_list.compare(last_track_path, Qt::CaseInsensitive)){
                last_track_idx = i;
            }
        }

        _v_meta_data.push_back(track);
    }

    if(_v_meta_data.size() == 0) return;

    _cur_play_idx = 0;

    if(loadLastTrack && last_track_idx >= 0){
        _cur_play_idx = last_track_idx;
    }

    emit sig_playlist_created(_v_meta_data, _cur_play_idx, _radio_active);

    emit sig_selected_file_changed(_cur_play_idx);

    _is_playing = start_immediatly;

    qDebug() << "Load last position? " << load_last_position;

    qDebug() << "Start immediatly? " << start_immediatly;

    if(load_last_position){
        qDebug() << "Position: " << last_track->pos_sec;
            emit sig_selected_file_changed_md(_v_meta_data[_cur_play_idx], last_track->pos_sec, start_immediatly);
    }

    else {
        emit sig_selected_file_changed_md(_v_meta_data[_cur_play_idx], 0, start_immediatly);
    }
}



// save the playlist, for possibly reloading it on next startup
void Playlist::psl_save_playlist_to_storage(){

    if(_radio_active) return;

    QStringList playlist_lst;
    foreach(MetaData md, _v_meta_data){
        if(md.radio_mode != RadioOff) continue;

        if(md.id >= 0) playlist_lst << QString::number(md.id);
        else
            playlist_lst << md.filepath;
	}

    _settings->setPlaylist(playlist_lst);
}

// GUI -->
void Playlist::psl_save_playlist(const QString& filename, const MetaDataList& v_md, bool relative){

   PlaylistParser::save_playlist(filename, v_md, relative);
}



void Playlist::save_stream_playlist(){

    if(_v_stream_playlist.size() == 0) {

        return;
    }

    QString title = "Radio_" + QDate::currentDate().toString("yyyy-mm-dd");
    title += QTime::currentTime().toString("hh.mm");
    QString dir = _settings->getStreamRipperPath() + QDir::separator();

    psl_save_playlist(dir + title + ".m3u", _v_stream_playlist, false);
}


void Playlist::psl_next_track(){

	/*** LASTFM ***/
    if(_radio_active == RadioLFM){

    	MetaDataList v_md;

    	if(_v_meta_data.size() == 0){
    		_cur_play_idx = -1;
            _is_playing = false;
            emit sig_no_track_to_play();
            emit sig_need_more_radio();
            return;
        }

        int track_num = _cur_play_idx + 1;

        // track too high
        if(track_num >= (int) _v_meta_data.size()){
            _cur_play_idx = -1;
            _is_playing = false;
            emit sig_no_track_to_play();
            emit sig_need_more_radio();
            return;
        }

        // last track
        else if(track_num == (int) _v_meta_data.size()-1) {
            emit sig_need_more_radio();
        }

        // copy tracks to tmp vector

        for(int i=0; i< (int) _v_meta_data.size(); i++){

        	if(i <= track_num)
        		v_md.push_back(_v_meta_data[i]);
        }


        emit sig_playlist_created(v_md, track_num, _radio_active);
        send_cur_playing_signal(track_num);

        return;
    }


    /*** NOT LASTFM ***/
    int track_num = -1;
    if(_v_meta_data.size() == 0){
        _cur_play_idx = -1;
        _is_playing = false;
		emit sig_no_track_to_play();
		return;
	}


	// shuffle mode
    if(_playlist_mode.shuffle){
		track_num = rand() % _v_meta_data.size();
		if(track_num == _cur_play_idx){
			track_num = (_cur_play_idx + 1) % _v_meta_data.size();
		}
	}

	// standard next track, also valid for stream stations
	else {

		// last track
		if(_cur_play_idx >= (int) _v_meta_data.size() -1){
			track_num = -1;

            if(_playlist_mode.repAll)
				track_num = 0;

            else{
                _is_playing = false;
                emit sig_no_track_to_play();
                return;
            }
		}

        else
            track_num = _cur_play_idx + 1;
	}


	// valid next track
	if(track_num >= 0){
        _v_meta_data.setCurPlayTrack(track_num);
		MetaData md = _v_meta_data[track_num];
		md.radio_mode = _radio_active;

		// maybe track is deleted here
		if( checkTrack(md) ){
            send_cur_playing_signal(track_num);
		}

		else{
            remove_row(track_num);
			psl_next_track();
		}
	}    
}


void Playlist::psl_gapless_track(){
    if((uint)(_cur_play_idx + 1) >= _v_meta_data.size() || _cur_play_idx < 0) {

        return;
    }

    //_cur_play_idx++;
    MetaData md = _v_meta_data[_cur_play_idx + 1];
    emit sig_gapless_track(md);
}




// --> custom playlists
void Playlist::psl_prepare_playlist_for_save(int id){
	emit sig_playlist_prepared(id, _v_meta_data);
}

void Playlist::psl_prepare_playlist_for_save(QString name){
	emit sig_playlist_prepared(name, _v_meta_data);
}

void Playlist::psl_prepare_playlist_for_save_file(QString name, bool relative){
    psl_save_playlist(name, _v_meta_data, relative);
}



void Playlist::psl_edit_id3_request(){
	emit sig_data_for_id3_change(_v_meta_data);
}

void Playlist::psl_track_time_changed(MetaData& md){

    if(md.id >= 0) _db->updateTrack(md);

    for(uint i=0; i<_v_meta_data.size(); i++){
        MetaData md_old = _v_meta_data[i];
        QString old_path = QDir(md_old.filepath).absolutePath();
        QString new_path = QDir(md.filepath).absolutePath();

        int cmp = old_path.compare(new_path, Qt::CaseInsensitive);
        if(cmp == 0){

            _v_meta_data[i] = md;

            break;
        }
    }

    if(_cur_play_idx >= 0 && _cur_play_idx <= (int) _v_meta_data.size());
        emit sig_cur_played_info_changed(_v_meta_data[_cur_play_idx]);

    emit sig_playlist_created(_v_meta_data, _cur_play_idx, _radio_active);
}

void Playlist::psl_id3_tags_changed(MetaDataList& new_meta_data){

    for(uint i=0; i<_v_meta_data.size(); i++){

        MetaData md_old = _v_meta_data[i];

        foreach(MetaData md_new, new_meta_data){
        	QString old_path = QDir(md_old.filepath).absolutePath();
        	QString new_path = QDir(md_new.filepath).absolutePath();
            int cmp = old_path.compare(new_path, Qt::CaseInsensitive);
            if(cmp == 0){

                _v_meta_data[i] = md_new;
            }
        }
    }

    if(_cur_play_idx >= 0 && _cur_play_idx <= (int) _v_meta_data.size());
        emit sig_cur_played_info_changed(_v_meta_data[_cur_play_idx]);

    emit sig_playlist_created(_v_meta_data, _cur_play_idx, _radio_active);
}


void Playlist::psl_similar_artists_available(const QList<int>& artists){

	// the response came too late, we already switched to radio
    if(_radio_active != RadioOff ){
		return;
	}

	if(artists.size() == 0) return;

	QList<int> artists_copy = Helper::randomize_list(artists);

	srand ( time(NULL) );


	int cur_artist_idx = 0;
	bool is_track_already_in = false;
	MetaData md;


	do {
		int artist_id = artists_copy.at(cur_artist_idx);
		MetaDataList vec_tracks;
		_db->getAllTracksByArtist(artist_id, vec_tracks);

		// give each artist several trys
		int max_rounds = vec_tracks.size();
		for(int rounds=0; rounds < max_rounds; rounds++){
			is_track_already_in = false;
			int rnd_track = (rand() % vec_tracks.size());
			md = vec_tracks.at(rnd_track);


			// search playlist
			for(uint i=0; i<_v_meta_data.size(); i++){
				if(_v_meta_data[i].id == md.id){
					is_track_already_in = true;
					break;
				}
			}

			if(!is_track_already_in) break;
		}

		cur_artist_idx++;
	} while(is_track_already_in && cur_artist_idx < artists_copy.size());

    if(md.id < 0) return;

	if(!is_track_already_in)
        _v_meta_data.push_back(md);

    psl_save_playlist_to_storage();

    emit sig_playlist_created(_v_meta_data, _cur_play_idx, _radio_active);

}


void Playlist::psl_import_new_tracks_to_library(bool copy){

    MetaDataList v_md_extern;
    foreach(MetaData md, _v_meta_data){
        if(md.is_extern){
            v_md_extern.push_back(md);
        }
    }

    if(v_md_extern.size() == 0) return;
    emit sig_import_files(v_md_extern);

}

void Playlist::psl_import_result(bool success){

    if(success && _radio_active == RadioOff){

        for(uint i=0; i<_v_meta_data.size(); i++){
            _v_meta_data[i].is_extern = false;
        }

        sig_playlist_created(_v_meta_data, _cur_play_idx, _radio_active);
    }
}

void  Playlist::psl_lfm_radio_init(bool success){

    if(!success) return;

    psl_clear_playlist();
    _radio_active = RadioLFM;
    _cur_play_idx = -1;
}

void Playlist::psl_new_lfm_playlist_available(const MetaDataList& playlist){

    if(playlist.size() == 0){
        psl_clear_playlist();
        return;
    }


    MetaDataList v_md = _v_meta_data;

    int i=0;
    foreach(MetaData md, playlist){

        if(md.radio_mode != RadioLFM) continue;

        md.is_disabled = true;
        _v_meta_data.push_back(md);
        if(i==0) v_md.push_back(md);
        i++;
    }

    if(_v_meta_data.size() == 0 || v_md.size() == 0) return;

    if(_cur_play_idx == -1) {


        emit sig_new_stream_session();

        emit sig_playlist_created(v_md, 0, RadioLFM);
        send_cur_playing_signal(0);
    }

    else{
        emit sig_playlist_created(v_md, _cur_play_idx, RadioLFM);
    }

    _radio_active = RadioLFM;
}


void Playlist::psl_play_stream(const QString& url, const QString& name){

    emit sig_new_stream_session();
    psl_clear_playlist();
    MetaDataList v_md;

    // playlist radio
    qDebug() << "is playlist file? " << url << ": " << Helper::is_playlistfile(url);
    if(Helper::is_playlistfile(url)){

        if(PlaylistParser::parse_playlist(url, v_md) > 0){

            foreach(MetaData md, v_md){

                if(name.size() > 0) md.title = name;
                else md.title = "Radio Station";

                if(md.artist.size() == 0)
                    md.artist = url;

                if(md.album.size() == 0)
                    md.album = md.title;

                md.radio_mode = RadioStation;
                _v_meta_data.push_back(md);
			}
		}
	}

	// real stream
	else{

		MetaData md;

		if(name.size() > 0) md.title = name;
		else md.title = "Radio Station";

        md.artist = url;
        md.album = md.title;
		md.filepath = url;
        md.radio_mode = RadioStation;

        _v_meta_data.push_back(md);
	}

    if(_v_meta_data.size() == 0) return;

    _radio_active = RadioStation;

    emit sig_playlist_created(_v_meta_data, 0, _radio_active);
    send_cur_playing_signal(0);

}


void  Playlist::psl_play_podcast(const QString& url, const QString& name){

    emit sig_new_stream_session();
    psl_clear_playlist();
    MetaDataList v_md;

    // playlist radio
    qDebug() << "is podcast file? ";
    QString content;
    if(Helper::is_podcastfile(url, &content)){
        qDebug() << "true";

        if(Podcast::parse_podcast_xml_file_content(content, v_md) > 0){

            foreach(MetaData md, v_md){

                md.radio_mode = RadioStation;
                if(md.title.size() == 0){
                    if(name.size() > 0)
                        md.title = name;
                    else md.title = "Podcast";
                }

                _v_meta_data.push_back(md);
            }
        }

        else {
            qDebug() << "could not extract metadata";

        }
    }
    else {
        qDebug() << url << " is no podcast file";
    }

    if(_v_meta_data.size() == 0) return;

    _radio_active = RadioStation;

    emit sig_playlist_created(_v_meta_data, 0, _radio_active);
    send_cur_playing_signal(0);
}

void Playlist::psl_valid_strrec_track(const MetaData& md){

	_v_stream_playlist.push_back(md);
}




bool  Playlist::checkTrack(const MetaData& md){
	return Helper::checkTrack(md);
}


void Playlist::psl_play_next_tracks(const MetaDataList& v_md){
    psl_insert_tracks(v_md, _cur_play_idx);
}

uint Playlist::get_num_tracks(){

	return _v_meta_data.size();
}
