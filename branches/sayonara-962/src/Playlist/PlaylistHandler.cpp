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
#include "HelperStructs/CDirectoryReader.h"


PlaylistHandler::PlaylistHandler(QObject * parent) :
	QObject (parent),
	SayonaraClass()
{
	_db = CDatabaseConnector::getInstance();

	_state = PlaylistStop;

	Playlist* pl = new_playlist(PlaylistTypeStd, 0);
	_playlists.append(pl);

	_cur_playlist_idx = 0;
	_active_playlist_idx = 0;

	_start_play = _settings->get(Set::PL_StartPlaying);
	if(!_start_play){
		_state = PlaylistWaiting;
	}

	_last_track = _settings->get(Set::PL_LastTrack);

	REGISTER_LISTENER(Set::PL_Mode, psl_playlist_mode_changed);
}


PlaylistHandler::~PlaylistHandler() {

}

void PlaylistHandler::emit_playlist_created(){
	emit sig_playlist_created(
				get_current()->get_playlist(),
				get_current()->get_cur_track_idx(),
				get_current()->get_type(),
				get_current()->get_idx()
	);
}

void PlaylistHandler::emit_cur_track_changed(){

	bool success;
	MetaData md;

	success = get_active()->get_cur_track(md);

	if(!success){
		emit sig_no_track_to_play();
		return;
	}


	_settings->set(Set::PL_LastTrack, get_active()->get_cur_track_idx());

	_state = PlaylistPlay;
	emit sig_cur_track_idx_changed(get_active()->get_cur_track_idx(),
								   get_active()->get_idx());

	emit sig_cur_track_changed(md, true);
}


void PlaylistHandler::load_old_playlist(){

	if(!_settings->get(Set::PL_Load)) return;

	_playlist_loader = new PlaylistLoader(this);

	connect(_playlist_loader, SIGNAL(sig_create_playlist(const MetaDataList&)),
			this, SLOT(create_playlist(const MetaDataList&)));

	connect(_playlist_loader, SIGNAL(sig_change_track(int)),
			this, SLOT(psl_change_track(int)));

	_playlist_loader->load_old_playlist();
}


PlaylistType PlaylistHandler::determine_playlist_type(const MetaDataList& v_md) {

    if(v_md.size() == 0) return PlaylistTypeStd;
	const MetaData& md = v_md[0];

	switch( md.radio_mode() ) {

        case RadioModeStation:
		case RadioModeSoundcloud:
			return PlaylistTypeStream;

		case RadioModeOff:
        default:
            return PlaylistTypeStd;
    }

    return PlaylistTypeStd;
}


Playlist* PlaylistHandler::new_playlist(PlaylistType type, int playlist_idx) {

	Playlist* pl;

    switch(type) {

        case PlaylistTypeStd:
			pl = (Playlist*) new StdPlaylist(playlist_idx);
            break;

        case PlaylistTypeStream:
			pl = (Playlist*) new StreamPlaylist(playlist_idx);
            break;

        default:
			pl = (Playlist*) new StdPlaylist(playlist_idx);
            break;
	}

	return pl;
}

// create a playlist, where metadata is already available
void PlaylistHandler::create_playlist(const MetaDataList& v_md) {

	get_current()->create_playlist(v_md);

	emit_playlist_created();

	if(_state == PlaylistStop ){

		MetaData md;
		bool success = get_current()->get_cur_track(md);
		if(!success){
			get_current()->play();
		}

		emit_cur_track_changed();
	}

	else if(_state == PlaylistWaiting){

		if(_last_track >= v_md.size() || _last_track < 0) return;

		MetaData md;
		get_current()->change_track(_last_track);
		bool success = get_current()->get_cur_track(md);

		if(!success){
			get_current()->play();
		}

		emit_cur_track_changed();
	}
}


// create a new playlist, where only filepaths are given
// Load Folder, Load File...
void PlaylistHandler::create_playlist(const QStringList& pathlist) {

	CDirectoryReader reader;
	MetaDataList v_md = reader.get_md_from_filelist(pathlist);
	create_playlist(v_md);
}


// create playlist from saved custom playlist
void PlaylistHandler::create_playlist(const CustomPlaylist& pl) {

	create_playlist(pl.tracks);
}


void PlaylistHandler::create_playlist(const QString& dir) {

	QStringList lst;
	lst << dir;
	create_playlist(lst);
}


// GUI -->
void PlaylistHandler::psl_save_playlist(QString filename, bool relative) {

	get_current()->save_to_m3u_file(filename, relative);
}

// --> custom playlists
void PlaylistHandler::psl_prepare_playlist_for_save(int id) {

	MetaDataList v_md = get_current()->get_playlist();

    if(v_md.size() > 0){
        emit sig_playlist_prepared(id, v_md);
    }
}

void PlaylistHandler::psl_prepare_playlist_for_save(QString name) {

	MetaDataList v_md = get_current()->get_playlist();

    if(v_md.size() > 0){
        emit sig_playlist_prepared(name, v_md);
    }
}

void PlaylistHandler::psl_next() {

	get_active()->next();
	emit_cur_track_changed();
}


void PlaylistHandler::psl_md_changed(const MetaData& md) {

	foreach(Playlist* pl, _playlists){
		QList<int> idx;
		idx << pl->find_tracks(md.filepath());

		foreach(int i, idx){
			pl->replace_track(i, md);
		}
	}

	emit_playlist_created();
}


void PlaylistHandler::psl_md_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new) {
	for(Playlist* pl : _playlists){
		pl->metadata_changed(v_md_old, v_md_new);
	}

	emit_playlist_created();
}

//
void PlaylistHandler::psl_similar_artists_available(const QList<int>& artists) {

	if(artists.size() == 0) return;

	PlaylistType type = get_active()->get_type();
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
			if( ! get_active()->find_tracks(md.id).isEmpty() ) {
                is_track_already_in = true;
            }

            else break;
		}

		cur_artist_idx++;

	} while(is_track_already_in && cur_artist_idx < artists_copy.size());


	if(md.id < 0) return;

	if(!is_track_already_in){
		get_active()->append_track(md);
	}
}

// GUI -->
void PlaylistHandler::psl_clear_playlist() {

	get_current()->clear();
	emit_playlist_created();
}


// play a track
void PlaylistHandler::psl_play() {

	if(_state == PlaylistPause){
		emit sig_goon_playing();
	}

	else if(_state == PlaylistStop){
		get_active()->play();
		emit_cur_track_changed();
	}

	_state = PlaylistPlay;
}


void PlaylistHandler::psl_pause() {

	get_active()->pause();
	_state = PlaylistPause;
}


void PlaylistHandler::psl_stop() {

	get_active()->stop();

	_settings->set(Set::PL_LastTrack, -1);

	_state = PlaylistStop;

	emit_cur_track_changed();
}


void PlaylistHandler::psl_forward() {

	get_active()->fwd();
	emit_cur_track_changed();
}


void PlaylistHandler::psl_backward() {
	get_active()->bwd();
	emit_cur_track_changed();
}


void PlaylistHandler::psl_change_track(int idx, int playlist_idx) {

	if(playlist_idx != _active_playlist_idx){
		get_active()->stop();
		_active_playlist_idx = playlist_idx;
	}

	bool success = get_active()->change_track(idx);

	if(!success) return;

	emit_cur_track_changed();
}

void PlaylistHandler::psl_selection_changed(const QList<int> & lst) {

	get_current()->selection_changed(lst);

	emit sig_selection_changed( get_current()->get_selected_tracks() );
}


void PlaylistHandler::psl_insert_tracks(const MetaDataList& v_md, int row) {

	get_current()->insert_tracks(v_md, row);
	emit_playlist_created();

}

void PlaylistHandler::psl_play_next(const MetaDataList & v_md) {

	get_current()->insert_tracks(v_md, get_current()->get_cur_track_idx() + 1);
	emit_playlist_created();
}

void PlaylistHandler::psl_append_tracks(const MetaDataList& v_md) {

	get_current()->append_tracks(v_md);
	emit_playlist_created();
}

void PlaylistHandler::psl_remove_rows(const QList<int> & rows) {

	get_current()->delete_tracks(rows);
	emit_playlist_created();
}

void PlaylistHandler::psl_move_rows(const QList<int>& rows, int tgt) {

	get_current()->move_tracks(rows, tgt);
	emit_playlist_created();
}


void PlaylistHandler::psl_playlist_mode_changed() {

	PlaylistMode plm = _settings->get(Set::PL_Mode);
	get_current()->set_playlist_mode(plm);
}


void PlaylistHandler::psl_audioconvert_on(){

	psl_stop();

	PlaylistMode pl_mode = get_current()->playlist_mode_backup();

	pl_mode.repAll = false;
	pl_mode.shuffle = false;
	pl_mode.dynamic = false;
	pl_mode.gapless = false;

	get_current()->set_playlist_mode(pl_mode);

	_settings->set(Set::PL_Mode, pl_mode);
}

void PlaylistHandler::psl_audioconvert_off(){

	psl_stop();

	PlaylistMode pl_mode = get_current()->playlist_mode_restore();

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

	_cur_playlist_idx = idx;

	emit sig_playlist_mode_changed( get_current()->get_playlist_mode());
	emit sig_playlist_index_changed(idx);

	return true;
}

void PlaylistHandler::close_playlist(int idx){

	delete _playlists[idx];
	_playlists.removeAt(idx);

	if(_cur_playlist_idx == idx){
		_cur_playlist_idx = 0;
	}

	if(_active_playlist_idx == idx){
		_active_playlist_idx = 0;
	}

	foreach(Playlist* pl, _playlists){
		if(pl->get_idx() >= idx){
			pl->set_idx(pl->get_idx() - 1);
		}
	}

	emit sig_playlist_closed(idx);
}


Playlist* PlaylistHandler::get_active(){
	return _playlists[_active_playlist_idx]	;
}

Playlist* PlaylistHandler::get_current(){
	return _playlists[_cur_playlist_idx];
}
