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

#include "PlaylistNew.h"
#include "LFMPlaylist.h"
#include "StdPlaylist.h"
#include "StreamPlaylist.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"

#include <QList>
#include <QDebug>


PlaylistNew::PlaylistNew(QObject * parent) : QObject (parent){

    _settings = CSettingsStorage::getInstance();
	_db = CDatabaseConnector::getInstance();
    _last_pos = 0;
    _playlist = 0;

    new_playlist(PlaylistTypeStd);
}

PlaylistNew::~PlaylistNew() {

}

void PlaylistNew::playlist_changed(const MetaDataList& v_md, int cur_idx){
    emit sig_playlist_created(v_md, cur_idx, _playlist->get_type());
    _settings->setPlaylist(_playlist->toStringList());
}

void PlaylistNew::track_changed(const MetaData& md, int cur_idx){

    emit sig_selected_file_changed(cur_idx);
    emit sig_selected_file_changed_md(md, _last_pos, (_state == PlaylistPlay && cur_idx >= 0));
    _last_pos = 0;

}

void PlaylistNew::no_track_to_play(){
    _state = PlaylistStop;
    emit sig_no_track_to_play();
}


PlaylistType PlaylistNew::determine_playlist_type(const MetaDataList& v_md){

    if(v_md.size() == 0) return PlaylistTypeStd;
    MetaData md = v_md[0];

    switch(md.radio_mode){
        case RADIO_LFM:
            return PlaylistTypeLFM;

        case RADIO_STATION:
            return PlaylistTypeStream;

        case RADIO_OFF:
        default:
            return PlaylistTypeStd;
    }

    return PlaylistTypeStd;
}


bool PlaylistNew::new_playlist(PlaylistType type){

    if(_playlist){
        if(_playlist->get_type() == type) return false;

        delete _playlist;
    }

    switch(type){

        case PlaylistTypeLFM:
            _playlist = (PlaylistHandler*) new LFMPlaylist(0);
            break;
        case PlaylistTypeStd:
            _playlist = (PlaylistHandler*) new StdPlaylist(0);
            break;
        case PlaylistTypeStream:
            _playlist = (PlaylistHandler*) new StreamPlaylist(0);
            break;

        default:
            _playlist = (PlaylistHandler*) new StdPlaylist(0);
            break;
    }

    connect(_playlist,  SIGNAL(sig_playlist_changed(const MetaDataList&,int)),
            this, SLOT(playlist_changed(const MetaDataList&,int)));
    connect(_playlist, SIGNAL(sig_track_changed(const MetaData&,int)),
            this, SLOT(track_changed(const MetaData&,int)));
    connect(_playlist, SIGNAL(sig_stopped()), this, SLOT(no_track_to_play()));

    return true;
}

// create a playlist, where metadata is already available
void PlaylistNew::psl_createPlaylist(MetaDataList& v_md, bool start_playing){

    if(start_playing) _state = PlaylistPlay;
    else if(_state != PlaylistPause) _state = PlaylistStop;

    PlaylistType type = determine_playlist_type(v_md);
    bool new_created = new_playlist(type);

    _playlist->create_playlist(v_md, start_playing && new_created);
}


// create a new playlist, where only filepaths are given
// Load Folder, Load File...
void PlaylistNew::psl_createPlaylist(QStringList& pathlist, bool start_playing){

    if(start_playing) _state = PlaylistPlay;
    else if(_state != PlaylistPause) _state = PlaylistStop;

    bool new_created = new_playlist(PlaylistTypeStd);
    _playlist->create_playlist(pathlist, start_playing && new_created);
}


// create playlist from saved custom playlist
void PlaylistNew::psl_createPlaylist(CustomPlaylist& pl, bool start_playing){
    if(start_playing) _state = PlaylistPlay;
    else if(_state != PlaylistPause) _state = PlaylistStop;

    // TDOD: Save old playlist
}


void PlaylistNew::psl_save_playlist_to_storage(){
   _playlist->save_for_reload();
}

// GUI -->
void PlaylistNew::psl_save_playlist(const QString& filename, bool relative){

   _playlist->save_to_m3u_file(filename, relative);
}


// --> custom playlists
void PlaylistNew::psl_prepare_playlist_for_save(int id){

    MetaDataList v_md;
    bool may_save = _playlist->request_playlist_for_collection(v_md);
    if(may_save)
        emit sig_playlist_prepared(id, v_md);
}

void PlaylistNew::psl_prepare_playlist_for_save(QString name){
    MetaDataList v_md;
    bool may_save = _playlist->request_playlist_for_collection(v_md);
    if(may_save)
        emit sig_playlist_prepared(name, v_md);
}


void PlaylistNew::psl_next(){
    _playlist->next();
}



void PlaylistNew::psl_track_time_changed(MetaData& md){

    if(md.id >= 0) {
        _db->updateTrack(md);
        int idx = _playlist->find_track_by_id(md.id);
        if(idx >= 0) _playlist->replace_track(idx, md);
    }

    else {
        int idx = _playlist->find_track_by_path(md.filepath);
        if(idx >= 0) _playlist->replace_track(idx, md);
    }
}


void PlaylistNew::psl_id3_tags_changed(MetaDataList& new_meta_data){
    _playlist->metadata_changed(new_meta_data);
}

//
void PlaylistNew::psl_similar_artists_available(const QList<int>& artists){

	if(artists.size() == 0) return;
    if(! (_playlist->get_type() == PlaylistTypeStd && _settings->getPlaylistMode().dynamic)) return;


	QList<int> artists_copy = Helper::randomize_list(artists);

	srand ( time(NULL) );

	int cur_artist_idx = 0;

	MetaData md;

    bool is_track_already_in = false;
	do {
		int artist_id = artists_copy.at(cur_artist_idx);
		MetaDataList vec_tracks;
		_db->getAllTracksByArtist(artist_id, vec_tracks);

		// give each artist several trys
		int max_rounds = vec_tracks.size();
		for(int rounds=0; rounds < max_rounds; rounds++){

			int rnd_track = (rand() % vec_tracks.size());
			md = vec_tracks.at(rnd_track);

            // search playlist
            if(_playlist->find_track_by_id(md.id) != -1){
                is_track_already_in = true;
            }

            else break;
		}

		cur_artist_idx++;
	} while(is_track_already_in && cur_artist_idx < artists_copy.size());


    if(md.id < 0) return;

	if(!is_track_already_in)
        _playlist->append_track(md);
}

// GUI -->
void PlaylistNew::psl_clear_playlist(){
    _playlist->clear();
}



// play a track
void PlaylistNew::psl_play(){
    if(_state == PlaylistStop){
        _state = PlaylistPlay;
        _playlist->play();
    }

    if(_state == PlaylistPause){
        emit sig_goon_playing();
    }

}


void PlaylistNew::psl_pause(){
    if(_state == PlaylistPlay || _state == PlaylistStop){
        _state = PlaylistPause;
        _playlist->pause();
    }
}

void PlaylistNew::psl_stop(){
    _state = PlaylistStop;
    _playlist->stop();
}

void PlaylistNew::psl_forward(){

    _playlist->fwd();
}

void PlaylistNew::psl_backward(){

    _playlist->bwd();
}

void PlaylistNew::psl_change_track(int idx, qint32 pos, bool start_playing){
    _last_pos = pos;
    if(start_playing)
        _state = PlaylistPlay;
    else
        _state = PlaylistPause;
    _playlist->change_track(idx);
}

void PlaylistNew::psl_selection_changed(const QList<int> & lst){
    _playlist->selection_changed(lst);
}


void PlaylistNew::psl_insert_tracks(const MetaDataList& v_md, int row){

    if(_playlist->get_type() != PlaylistTypeStd)
        this->new_playlist(PlaylistTypeStd);

    _playlist->insert_tracks(v_md, row);

}

void PlaylistNew::psl_play_next(const MetaDataList & v_md){

    if(_playlist->get_type() != PlaylistTypeStd)
        this->new_playlist(PlaylistTypeStd);

    _playlist->insert_tracks(v_md, _playlist->get_cur_track() + 1);
}



void PlaylistNew::psl_append_tracks(const MetaDataList& v_md){

    _playlist->append_tracks(v_md);
}


void PlaylistNew::psl_remove_rows(const QList<int> & rows, bool select_next_row){

    _playlist->delete_tracks(rows);
}

void PlaylistNew::psl_move_rows(const QList<int>& rows, int tgt){

    _playlist->move_tracks(rows, tgt);
}


void PlaylistNew::psl_playlist_mode_changed(const Playlist_Mode& playlist_mode){
    _playlist->set_playlist_mode(playlist_mode);
}


