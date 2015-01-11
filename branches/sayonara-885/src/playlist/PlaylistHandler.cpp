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

	apply_state(PlaylistStop);

	_cur_playlist = new_playlist(PlaylistTypeStd, 0);
	_cur_playlist_idx = 0;
	_active_playlist_idx = 0;
	_active_playlist = _cur_playlist;
	_playlists.append(_cur_playlist);

	_start_play = _settings->get(Set::PL_StartPlaying);

	REGISTER_LISTENER(Set::PL_Mode, psl_playlist_mode_changed);
}


PlaylistHandler::~PlaylistHandler() {

}

void PlaylistHandler::apply_state(PlaylistState state){

	_state = state;

	for(Playlist* pl : _playlists){
		pl->set_playlist_state(state);
	}
}



void PlaylistHandler::playlist_changed(const Playlist* pl, int playlist_idx) {

	_settings->set(Set::PL_Playlist, pl->toStringList());

	emit sig_playlist_created( pl->get_playlist(), pl->get_cur_track(), pl->get_type(), pl->get_idx() );
}


void PlaylistHandler::track_changed(const MetaData& md, int cur_track_idx, int playlist_idx) {

	if(!md.is_disabled){

		_settings->set(Set::PL_LastTrack, cur_track_idx);

		emit sig_cur_track_idx_changed(cur_track_idx);
		emit sig_cur_track_changed(md, _start_play);

		if(!_start_play){
			_start_play = true;
		}
	}
}

void PlaylistHandler::no_track_to_play(int playlist_idx) {

	apply_state(PlaylistStop);

	emit sig_no_track_to_play();
}




void PlaylistHandler::load_old_playlist(){

	if(!_settings->get(Set::PL_Load)) return;

	_playlist_loader = new PlaylistLoader(this);

	connect(_playlist_loader, SIGNAL(sig_create_playlist(const MetaDataList&)),
			this, SLOT(create_playlist(const MetaDataList&)));

	connect(_playlist_loader, SIGNAL(sig_change_track(int)),
			this, SLOT(psl_change_track(int)));


	if(_settings->get(Set::PL_LoadLastTrack) && !_settings->get(Set::PL_StartPlaying)){
		apply_state(PlaylistPause);
	}

	_playlist_loader->load_old_playlist();
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


Playlist* PlaylistHandler::new_playlist(PlaylistType type, int idx) {

	Playlist* pl;

    switch(type) {

        case PlaylistTypeStd:
			pl = (Playlist*) new StdPlaylist(idx, 0);
            break;

        case PlaylistTypeStream:
			pl = (Playlist*) new StreamPlaylist(idx, 0);
            break;

        default:
			pl = (Playlist*) new StdPlaylist(idx, 0);
            break;
	}

	if(!pl) return NULL;

	connect(pl,  SIGNAL(sig_playlist_changed(const Playlist*, int)),
			this, SLOT(playlist_changed(const Playlist*, int)));
	connect(pl, SIGNAL(sig_track_changed(const MetaData&, int, int)),
			this, SLOT(track_changed(const MetaData&, int, int)));
	connect(pl, SIGNAL(sig_stopped(int)), this, SLOT(no_track_to_play(int)));

	return pl;
}

// create a playlist, where metadata is already available
void PlaylistHandler::create_playlist(const MetaDataList& v_md) {

	qDebug() << "Create Playlist...";

    PlaylistType type = determine_playlist_type(v_md);
	PlaylistType old_type;

	if(_cur_playlist){

		old_type = _cur_playlist->get_type();

		if(old_type != type){

			int idx = _cur_playlist->get_idx();
			delete _cur_playlist;

			_cur_playlist = new_playlist(type, idx);

		}

		_cur_playlist->create_playlist(v_md);
	}

	else{

		Playlist* pl = new_playlist(type, _playlists.size());
		_cur_playlist = pl;
		_playlists << pl;
		_cur_playlist_idx = 0;
		apply_state(_state);
	}

	_active_playlist = _cur_playlist;
	_playlists[_cur_playlist_idx] = _cur_playlist;
}


// create a new playlist, where only filepaths are given
// Load Folder, Load File...
void PlaylistHandler::create_playlist(const QStringList& pathlist) {

	/// TODO
	///

	_cur_playlist->create_playlist(pathlist);
}


// create playlist from saved custom playlist
void PlaylistHandler::create_playlist(const CustomPlaylist& pl) {

	create_playlist(pl.tracks);
}


// GUI -->
void PlaylistHandler::psl_save_playlist(QString filename, bool relative) {

   _cur_playlist->save_to_m3u_file(filename, relative);
}

// --> custom playlists
void PlaylistHandler::psl_prepare_playlist_for_save(int id) {

	MetaDataList v_md = _cur_playlist->get_playlist();
    if(v_md.size() > 0){
        emit sig_playlist_prepared(id, v_md);
    }
}

void PlaylistHandler::psl_prepare_playlist_for_save(QString name) {

	MetaDataList v_md = _cur_playlist->get_playlist();
    if(v_md.size() > 0){
        emit sig_playlist_prepared(name, v_md);
    }
}

void PlaylistHandler::psl_next() {
	_active_playlist->next();
}


void PlaylistHandler::psl_md_changed(const MetaData& md) {

	foreach(Playlist* pl, _playlists){
		QList<int> idx;
		idx << pl->find_tracks(md.filepath);
		foreach(int i, idx){
			pl->replace_track(i, md);
		}
	}
}


void PlaylistHandler::psl_id3_tags_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new) {
	foreach(Playlist* pl, _playlists){
		pl->metadata_changed(v_md_old, v_md_new);
	}
}

//
void PlaylistHandler::psl_similar_artists_available(const QList<int>& artists) {

	if(artists.size() == 0) return;

	PlaylistType type = _active_playlist->get_type();
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
			if( !_cur_playlist->find_tracks(md.id).isEmpty() ) {
                is_track_already_in = true;
            }

            else break;
		}

		cur_artist_idx++;

	} while(is_track_already_in && cur_artist_idx < artists_copy.size());


    if(md.id < 0) return;

	if(!is_track_already_in){
		_active_playlist->append_track(md);
	}
}

// GUI -->
void PlaylistHandler::psl_clear_playlist() {
	_cur_playlist->clear();
}


// play a track
void PlaylistHandler::psl_play() {

	PlaylistState state = _active_playlist->get_playlist_state();
	if(state == PlaylistPause){
		emit sig_goon_playing();
	}

	_active_playlist->play();
	apply_state(PlaylistPlay);
}


void PlaylistHandler::psl_pause() {

	apply_state(PlaylistPause);

	_active_playlist->pause();
}


void PlaylistHandler::psl_stop() {

	apply_state(PlaylistStop);
	_active_playlist->stop();

	_settings->set(Set::PL_LastTrack, -1);
}


void PlaylistHandler::psl_forward() {

	_active_playlist->fwd();
}


void PlaylistHandler::psl_backward() {

	_active_playlist->bwd();
}


void PlaylistHandler::psl_change_track(int idx) {

	_active_playlist = _cur_playlist;
	_active_playlist->change_track(idx);
}

void PlaylistHandler::psl_selection_changed(const QList<int> & lst) {

	_cur_playlist->selection_changed(lst);

	emit sig_selection_changed( _cur_playlist->get_selected_tracks() );
}


void PlaylistHandler::psl_insert_tracks(const MetaDataList& v_md, int row) {

	if(_cur_playlist->get_type() != PlaylistTypeStd){

		Playlist* pl = new_playlist(PlaylistTypeStd, _cur_playlist_idx);

		_playlists[_cur_playlist_idx] = pl;

		if(_active_playlist_idx == _cur_playlist_idx){
			_active_playlist = pl;
		}

		_cur_playlist = pl;
    }

	_cur_playlist->insert_tracks(v_md, row);
	if(_cur_playlist->is_empty()) return;
}

void PlaylistHandler::psl_play_next(const MetaDataList & v_md) {

	if(_cur_playlist->get_type() != PlaylistTypeStd){
		this->new_playlist(PlaylistTypeStd, _cur_playlist_idx);
    }

	_cur_playlist->insert_tracks(v_md, _cur_playlist->get_cur_track() + 1);
}

void PlaylistHandler::psl_append_tracks(const MetaDataList& v_md) {

	_cur_playlist->append_tracks(v_md);
}

void PlaylistHandler::psl_remove_rows(const QList<int> & rows) {

	_cur_playlist->delete_tracks(rows);
}

void PlaylistHandler::psl_move_rows(const QList<int>& rows, int tgt) {

	_cur_playlist->move_tracks(rows, tgt);
}


void PlaylistHandler::psl_playlist_mode_changed() {

	PlaylistMode plm = _settings->get(Set::PL_Mode);
	_cur_playlist->set_playlist_mode(plm);
}


void PlaylistHandler::psl_audioconvert_on(){

	psl_stop();

	PlaylistMode pl_mode = _cur_playlist->playlist_mode_backup();

	pl_mode.repAll = false;
	pl_mode.shuffle = false;
	pl_mode.dynamic = false;
	pl_mode.gapless = false;

	_cur_playlist->set_playlist_mode(pl_mode);

	_settings->set(Set::PL_Mode, pl_mode);
}

void PlaylistHandler::psl_audioconvert_off(){

	psl_stop();

	PlaylistMode pl_mode = _cur_playlist->playlist_mode_restore();

	_settings->set(Set::PL_Mode, pl_mode);
}


int PlaylistHandler::add_new_playlist(QString name){

	int idx = _playlists.size();
	Playlist* pl = new_playlist(PlaylistTypeStd, _playlists.size());
	_playlists.append(pl);

	emit sig_new_playlist_added(idx, name);

	return idx;
}

bool PlaylistHandler::change_playlist_index(int idx){

	if(idx < 0 || idx >= _playlists.size()) return false;

	_cur_playlist = _playlists[idx];
	_cur_playlist_idx = idx;

	int cur_track = _cur_playlist->get_cur_track();
	PlaylistType cur_type = _cur_playlist->get_type();

	emit sig_playlist_created(_cur_playlist->get_playlist(), cur_track, cur_type, _cur_playlist_idx);
	emit sig_playlist_mode_changed(_cur_playlist->get_playlist_mode());
	emit sig_playlist_index_changed(idx);

	return true;
}

void PlaylistHandler::close_playlist(int idx){

	delete _playlists[idx];
	_playlists.removeAt(idx);

	foreach(Playlist* pl, _playlists){
		if(pl->get_idx() >= idx){
			pl->set_idx(pl->get_idx() - 1);
		}
	}

	emit sig_playlist_closed(idx);
}
