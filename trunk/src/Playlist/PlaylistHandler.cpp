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
	_play_manager = PlayManager::getInstance();

	Playlist* pl = new_playlist(PlaylistTypeStd, 0);
	_playlists.append(pl);

	_cur_playlist_idx = 0;
	_active_playlist_idx = 0;
	_max_playlist_name = 2;

	connect(_play_manager,	SIGNAL(sig_playstate_changed(PlayManager::PlayState)),
			this,			SLOT(playstate_changed(PlayManager::PlayState)));

	connect(_play_manager, SIGNAL(sig_next()), this, SLOT(next()));
	connect(_play_manager, SIGNAL(sig_previous()), this, SLOT(previous()));


	REGISTER_LISTENER(Set::PL_Mode, playlist_mode_changed);
}


PlaylistHandler::~PlaylistHandler() {

}

void PlaylistHandler::emit_playlist_created(Playlist* pl){


	if(!pl){
		pl = get_current();
	}

	_settings->set(Set::PL_Playlist, pl->toStringList() );
	_play_manager->playlist_changed( pl->get_playlist() );

	emit sig_playlist_created(
				pl->get_playlist(),
				pl->get_cur_track_idx(),
				pl->get_type(),
				pl->get_idx()
	);
}


void PlaylistHandler::emit_cur_track_changed(Playlist* pl){

	MetaData md;
	bool success;
	int cur_track_idx;
	int playlist_idx;


	if(!pl){
		pl = get_active();
	}

	success = pl->get_cur_track(md);
	cur_track_idx = pl->get_cur_track_idx();
	playlist_idx = pl->get_idx();

	if(!success || cur_track_idx == -1){
		_play_manager->change_track_idx(-1);
		return;
	}



	emit sig_cur_track_idx_changed( cur_track_idx,
									playlist_idx );

	_play_manager->change_track(md);
	_play_manager->change_track_idx(cur_track_idx);

}


void PlaylistHandler::load_old_playlist(){

	if(!_settings->get(Set::PL_Load)) {
		return;
	}

	_playlist_loader = new PlaylistLoader(this);

	MetaDataList v_md = _playlist_loader->load_old_playlist();
	int last_track = v_md.getCurPlayTrack();

	create_playlist(v_md);

	if(last_track >= 0 && last_track < v_md.size()){
		change_track(last_track);
	}

	else{
		change_track(0);
	}


	if(!_settings->get(Set::PL_StartPlaying)){
		_play_manager->pause();
	}

	delete _playlist_loader;
	_playlist_loader = 0;
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


Playlist* PlaylistHandler::new_playlist(PlaylistType type, int playlist_idx, QString name) {

	Playlist* pl;

    switch(type) {

		case PlaylistTypeStd:
			pl = (Playlist*) new StdPlaylist(playlist_idx, name);
            break;

        case PlaylistTypeStream:
			pl = (Playlist*) new StreamPlaylist(playlist_idx, name);
            break;

        default:
			pl = (Playlist*) new StdPlaylist(playlist_idx, name);
            break;
	}

	return pl;
}

// create a playlist, where metadata is already available
void PlaylistHandler::create_playlist(const MetaDataList& v_md, QString name) {

	int idx = add_new_playlist(name);
	change_playlist_index(idx);

	get_current()->create_playlist(v_md);

	emit_playlist_created();
}


// create a new playlist, where only filepaths are given
// Load Folder, Load File...
void PlaylistHandler::create_playlist(const QStringList& pathlist, QString name) {

	CDirectoryReader reader;
	MetaDataList v_md = reader.get_md_from_filelist(pathlist);
	create_playlist(v_md, name);
}


// create playlist from saved custom playlist
void PlaylistHandler::create_playlist(const CustomPlaylist& pl, QString name) {

	create_playlist(pl.tracks, name);
}


void PlaylistHandler::create_playlist(const QString& dir, QString name) {

	QStringList lst;
	lst << dir;
	create_playlist(lst, name);
}


// GUI -->
void PlaylistHandler::save_playlist(QString filename, bool relative) {

	get_current()->save_to_m3u_file(filename, relative);
}

// --> custom playlists
void PlaylistHandler::prepare_playlist_for_save(int id) {

	MetaDataList v_md = get_current()->get_playlist();

    if(v_md.size() > 0){
        emit sig_playlist_prepared(id, v_md);
    }
}

void PlaylistHandler::prepare_playlist_for_save(QString name) {

	MetaDataList v_md = get_current()->get_playlist();

    if(v_md.size() > 0){
        emit sig_playlist_prepared(name, v_md);
    }
}



void PlaylistHandler::md_changed(const MetaData& md) {

	foreach(Playlist* pl, _playlists){
		QList<int> idx;
		idx << pl->find_tracks(md.filepath());

		foreach(int i, idx){
			pl->replace_track(i, md);
		}
	}

	emit_playlist_created();
}


void PlaylistHandler::md_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new) {
	for(Playlist* pl : _playlists){
		pl->metadata_changed(v_md_old, v_md_new);
	}

	emit_playlist_created();
}

//
void PlaylistHandler::similar_artists_available(const QList<int>& artists) {

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
		emit_playlist_created(get_active());
	}
}

// GUI -->
void PlaylistHandler::clear_playlist() {

	get_current()->clear();
	emit_playlist_created();
}

void PlaylistHandler::playstate_changed(PlayManager::PlayState state){

	switch(state){
		case PlayManager::PlayState_Playing:
			played();
			break;
		case PlayManager::PlayState_Paused:
			paused();
			break;
		case PlayManager::PlayState_Stopped:
			stopped();
			break;

		default:
			return;
	}

	return;
}

void PlaylistHandler::played(){

	get_active()->play();
}

void PlaylistHandler::paused(){

	get_active()->pause();
}

void PlaylistHandler::stopped(){

	get_active()->stop();
}


void PlaylistHandler::next() {
	get_active()->next();
	emit_cur_track_changed();
}



void PlaylistHandler::previous() {
	get_active()->bwd();
	emit_cur_track_changed();
}


void PlaylistHandler::change_track(int idx, int playlist_idx) {

	if(playlist_idx != _active_playlist_idx && playlist_idx >= 0){
		get_active()->stop();
		_active_playlist_idx = playlist_idx;
	}

	get_active()->change_track(idx);

	emit_cur_track_changed();
}



void PlaylistHandler::selection_changed(const QList<int> & lst) {

	get_current()->selection_changed(lst);

	emit sig_selection_changed( get_current()->get_selected_tracks() );

}


void PlaylistHandler::insert_tracks(const MetaDataList& v_md, int row) {

	get_current()->insert_tracks(v_md, row);
	emit_playlist_created();

}

void PlaylistHandler::play_next(const MetaDataList & v_md) {

	Playlist* pl = get_active();

	pl->insert_tracks(v_md, pl->get_cur_track_idx() + 1);

	emit_playlist_created(pl);
}

void PlaylistHandler::append_tracks(const MetaDataList& v_md) {

	get_current()->append_tracks(v_md);
	emit_playlist_created();
}

void PlaylistHandler::remove_rows(const QList<int> & rows) {

	get_current()->delete_tracks(rows);
	emit_playlist_created();
}

void PlaylistHandler::move_rows(const QList<int>& rows, int tgt) {

	get_current()->move_tracks(rows, tgt);
	emit_playlist_created();
}


void PlaylistHandler::playlist_mode_changed() {

	PlaylistMode plm = _settings->get(Set::PL_Mode);
	get_current()->set_playlist_mode(plm);
}


QString PlaylistHandler::request_first_playlist_name(){
	return tr("List %1").arg(1);
}

QString PlaylistHandler::request_new_playlist_name(){

	return tr("List %1").arg(_max_playlist_name);
}

int PlaylistHandler::add_new_playlist(QString name){

	int idx = exists(name);

	if(idx >= 0) return idx;

	idx = _playlists.size();

	Playlist* pl = new_playlist(PlaylistTypeStd, _playlists.size(), name);
	_playlists.append(pl);

	emit sig_new_playlist_added(idx, name);

	_max_playlist_name ++;
	return idx;
}

bool PlaylistHandler::change_playlist_index(int idx){

	if(idx < 0 || idx >= _playlists.size()) return false;

	if(_play_manager->get_play_state() == PlayManager::PlayState_Stopped){
		_active_playlist_idx = idx;
	}

	_cur_playlist_idx = idx;
	emit_playlist_created();


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

	else if(_cur_playlist_idx > idx){
		_cur_playlist_idx --;
	}

	if(_active_playlist_idx == idx){
		_active_playlist_idx = 0;
	}

	else if(_active_playlist_idx > idx){
		_active_playlist_idx --;
	}

	foreach(Playlist* pl, _playlists){
		if(pl->get_idx() >= idx){
			pl->set_idx(pl->get_idx() - 1);
		}
	}

	emit_playlist_created();

	emit sig_playlist_closed(idx);
}


Playlist* PlaylistHandler::get_active(){
	return _playlists[_active_playlist_idx]	;
}

Playlist* PlaylistHandler::get_current(){
	return _playlists[_cur_playlist_idx];
}

int PlaylistHandler::exists(QString name){

	if(name.isEmpty()) {
		return _cur_playlist_idx;
	}

	for(int i=0; i<_playlists.size(); i++){
		Playlist* pl = _playlists[i];
		if(pl->get_name().compare(name) == 0){
			return i;
		}
	}

	return -1;
}
