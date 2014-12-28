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

#include "PlaylistHandler.h"
#include "StdPlaylist.h"
#include "StreamPlaylist.h"



PlaylistHandler::PlaylistHandler(QObject * parent) :
	QObject (parent),
	SayonaraClass()
{


	_db = CDatabaseConnector::getInstance();
    _last_pos = 0;
    _playlist = 0;
    _state = PlaylistStop;

    new_playlist(PlaylistTypeStd);

	REGISTER_LISTENER(Set::PL_Mode, psl_playlist_mode_changed);
}


PlaylistHandler::~PlaylistHandler() {
	if(_playlist)
		delete _playlist;
}

void PlaylistHandler::playlist_changed(const MetaDataList& v_md, int cur_idx) {

	_settings->set(Set::PL_Playlist, _playlist->toStringList());

	emit sig_playlist_created(v_md, cur_idx, _playlist->get_type());
}

void PlaylistHandler::track_changed(const MetaData& md, int cur_idx) {

    if(!md.is_disabled){
		_settings->set(Set::PL_LastTrack, cur_idx);
        emit sig_selected_file_changed(cur_idx);
        emit sig_selected_file_changed_md(md, _last_pos, (_state == PlaylistPlay && cur_idx >= 0));
        _last_pos = 0;
    }
}

void PlaylistHandler::no_track_to_play() {
    _state = PlaylistStop;
    emit sig_no_track_to_play();
}


PlaylistType PlaylistHandler::determine_playlist_type(const MetaDataList& v_md) {

    if(v_md.size() == 0) return PlaylistTypeStd;
   const MetaData& md = v_md[0];

    switch(md.radio_mode) {

        case RadioModeStation:
            return PlaylistTypeStream;

		case RadioModeOff:
        default:
            return PlaylistTypeStd;
    }

    return PlaylistTypeStd;
}


bool PlaylistHandler::new_playlist(PlaylistType type) {

    if(_playlist) {
        if(_playlist->get_type() == type) return false;

        delete _playlist;
    }

    switch(type) {
        case PlaylistTypeStd:
            _playlist = (Playlist*) new StdPlaylist(0);
            break;
        case PlaylistTypeStream:
            _playlist = (Playlist*) new StreamPlaylist(0);
            break;

        default:
            _playlist = (Playlist*) new StdPlaylist(0);
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
void PlaylistHandler::psl_createPlaylist(const MetaDataList& v_md, bool start_playing) {

    PlaylistType type = determine_playlist_type(v_md);

    bool new_created = new_playlist(type);

    _playlist->create_playlist(v_md, start_playing && new_created);

    if(start_playing && !_playlist->is_empty()) {

        // is stopped now
        if(_state == PlaylistStop){
            psl_play();
        }
    }

    else{
        psl_stop();
    }
}


// create a new playlist, where only filepaths are given
// Load Folder, Load File...
void PlaylistHandler::psl_createPlaylist(const QStringList& pathlist, bool start_playing) {

    if(start_playing) {
        _state = PlaylistPlay;
    }

    else if(_state != PlaylistPause) {
        _state = PlaylistStop;
    }

    bool new_created = new_playlist(PlaylistTypeStd);
    _playlist->create_playlist(pathlist, start_playing && new_created);
}


// create playlist from saved custom playlist
void PlaylistHandler::psl_createPlaylist(const CustomPlaylist& pl, bool start_playing) {

    if(start_playing){
        _state = PlaylistPlay;
    }

    else if(_state != PlaylistPause){
        _state = PlaylistStop;
    }

    psl_createPlaylist(pl.tracks, start_playing);
}


// GUI -->
void PlaylistHandler::psl_save_playlist(QString filename, bool relative) {

   _playlist->save_to_m3u_file(filename, relative);
}

// --> custom playlists
void PlaylistHandler::psl_prepare_playlist_for_save(int id) {

    MetaDataList v_md = _playlist->get_playlist();
    if(v_md.size() > 0){
        emit sig_playlist_prepared(id, v_md);
    }
}

void PlaylistHandler::psl_prepare_playlist_for_save(QString name) {

    MetaDataList v_md = _playlist->get_playlist();
    if(v_md.size() > 0){
        emit sig_playlist_prepared(name, v_md);
    }
}

void PlaylistHandler::psl_next() {
    _playlist->next();
}


void PlaylistHandler::psl_dur_changed(const MetaData& md) {

	QList<int>idx;

    if(md.id >= 0) {
        _db->updateTrack(md);

		idx = _playlist->find_tracks(md.id);
    }

    else {
		idx = _playlist->find_tracks(md.filepath);

    }

	foreach(int i, idx){
		_playlist->replace_track(i, md);
    }
}


void PlaylistHandler::psl_id3_tags_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new) {
	_playlist->metadata_changed(v_md_old, v_md_new);
}

//
void PlaylistHandler::psl_similar_artists_available(const QList<int>& artists) {

	if(artists.size() == 0) return;

	PlaylistType type = _playlist->get_type();
	PlaylistMode plm = _settings->get(Set::PL_Mode);

	if( (type != PlaylistTypeStd) || !plm.dynamic ) return;


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
		for(int rounds=0; rounds < max_rounds; rounds++) {

			int rnd_track = (rand() % vec_tracks.size());
            md = vec_tracks[rnd_track];

            // search playlist
			if( !_playlist->find_tracks(md.id).isEmpty() ) {
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
void PlaylistHandler::psl_clear_playlist() {
    _playlist->clear();
}


// play a track
void PlaylistHandler::psl_play() {
    if(_state == PlaylistStop) {
        _state = PlaylistPlay;
        _playlist->play();
    }

    if(_state == PlaylistPause) {
        _state = PlaylistPlay;
        emit sig_goon_playing();
    }

}


void PlaylistHandler::psl_pause() {
    if(_state == PlaylistPlay || _state == PlaylistStop) {
        _state = PlaylistPause;
        _playlist->pause();
    }
}


void PlaylistHandler::psl_stop() {
    _state = PlaylistStop;
    _playlist->stop();
}


void PlaylistHandler::psl_forward() {

    _playlist->fwd();
}


void PlaylistHandler::psl_backward() {

    _playlist->bwd();
}


void PlaylistHandler::psl_change_track(int idx, qint32 pos, bool start_playing) {

    _last_pos = pos;

    if(start_playing){
        _state = PlaylistPlay;
    }

    else{
        _state = PlaylistPause;
    }

    _playlist->change_track(idx);
}

void PlaylistHandler::psl_selection_changed(const QList<int> & lst) {

    _playlist->selection_changed(lst);

	emit sig_selection_changed( _playlist->get_selected_tracks() );
}


void PlaylistHandler::psl_insert_tracks(const MetaDataList& v_md, int row) {

    bool empty = _playlist->is_empty();

    if(_playlist->get_type() != PlaylistTypeStd){
        this->new_playlist(PlaylistTypeStd);
    }

    _playlist->insert_tracks(v_md, row);

    if(empty && _state == PlaylistStop && !_playlist->is_empty()){
        psl_play();
    }
}

void PlaylistHandler::psl_play_next(const MetaDataList & v_md) {

    if(_playlist->get_type() != PlaylistTypeStd){
        this->new_playlist(PlaylistTypeStd);
    }

    _playlist->insert_tracks(v_md, _playlist->get_cur_track() + 1);
}

void PlaylistHandler::psl_append_tracks(const MetaDataList& v_md) {

    bool empty = _playlist->is_empty();

    _playlist->append_tracks(v_md);

    if(empty && _state == PlaylistStop && !_playlist->is_empty()) {
         psl_play();
    }
}

void PlaylistHandler::psl_remove_rows(const QList<int> & rows, bool select_next_row) {

    _playlist->delete_tracks(rows);
}

void PlaylistHandler::psl_move_rows(const QList<int>& rows, int tgt) {

    _playlist->move_tracks(rows, tgt);
}


void PlaylistHandler::psl_playlist_mode_changed() {
	PlaylistMode plm = _settings->get(Set::PL_Mode);
	_playlist->set_playlist_mode(plm);
}


void PlaylistHandler::psl_audioconvert_on(){

	psl_stop();

	PlaylistMode pl_mode = _playlist->playlist_mode_backup();

	pl_mode.repAll = false;
	pl_mode.shuffle = false;
	pl_mode.dynamic = false;
	pl_mode.gapless = false;

	_playlist->set_playlist_mode(pl_mode);

	_settings->set(Set::PL_Mode, pl_mode);
}

void PlaylistHandler::psl_audioconvert_off(){

	psl_stop();

	PlaylistMode pl_mode = _playlist->playlist_mode_restore();

	_settings->set(Set::PL_Mode, pl_mode);
}
