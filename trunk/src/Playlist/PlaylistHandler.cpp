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

#include "Playlist/PlaylistHandler.h"
#include "Playlist/StdPlaylist.h"
#include "Playlist/StreamPlaylist.h"


#include "HelperStructs/DirectoryReader/DirectoryReader.h"
#include "HelperStructs/Parser/PlaylistParser.h"
#include "TagEdit/MetaDataChangeNotifier.h"

#include <random>


PlaylistHandler::PlaylistHandler(QObject * parent) :
	QObject (parent),
	SayonaraClass()
{

	_db = CDatabaseConnector::getInstance();
	_play_manager = PlayManager::getInstance();
	_playlist_db_connector = PlaylistDBConnector::getInstance();

	_dummy_playlist = new_playlist(PlaylistTypeStd, -1);

	_cur_playlist_idx = -1;
	_active_playlist_idx = -1;

	connect(_play_manager,	SIGNAL(sig_playstate_changed(PlayManager::PlayState)),
			this,			SLOT(playstate_changed(PlayManager::PlayState)));

	connect(_play_manager, SIGNAL(sig_next()), this, SLOT(next()));
	connect(_play_manager, SIGNAL(sig_previous()), this, SLOT(previous()));


	MetaDataChangeNotifier* md_change_notifier = MetaDataChangeNotifier::getInstance();
	connect(md_change_notifier, SIGNAL(sig_metadata_changed(const MetaDataList&, const MetaDataList&)),
			this,				SLOT(md_changed(const MetaDataList&,const MetaDataList&)));


	REGISTER_LISTENER(Set::PL_Mode, playlist_mode_changed);
}

PlaylistHandler::~PlaylistHandler() {

	delete _dummy_playlist;
	for(int i=0; i<_playlists.size(); i++){
		delete _playlists[i];
	}
}

void PlaylistHandler::emit_playlist_created(Playlist* pl){

	if(!pl){
		pl = get_current();
	}

	if(pl->is_temporary()){

		if(pl->get_db_id() >= 0){
			pl->save();
		}

		else{
			pl->insert_temporary_into_db();
		}
	}

	emit sig_playlist_created( pl );
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

	int playlist_id = pl->get_db_id();
	if(playlist_id == -1){
		pl->insert_temporary_into_db();
		playlist_id = pl->get_db_id();
	}

	_settings->set(Set::PL_LastPlaylist, playlist_id);
	emit sig_cur_track_idx_changed( cur_track_idx,
									playlist_idx );

	_play_manager->change_track(md);
	_play_manager->change_track_idx(cur_track_idx);
}


int PlaylistHandler::load_old_playlists(){

	if(!_settings->get(Set::PL_Load)) {
		return 0;
	}

	PlaylistLoader* _playlist_loader = new PlaylistLoader(this);

	int n_playlists = _playlist_loader->load_old_playlists();

	if(n_playlists == 0){	
		create_empty_playlist(request_new_playlist_name(), true);
		_play_manager->stop();
		return 0;
	}

	if(!_settings->get(Set::PL_StartPlaying)){
		_play_manager->pause();
	}

	delete _playlist_loader;
	_playlist_loader = 0;



	return _playlists.size();
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

		default: // will never be executed
			pl = (Playlist*) new StdPlaylist(playlist_idx, name);
            break;
	}

	return pl;
}

// create a playlist, where metadata is already available
int PlaylistHandler::create_playlist(const MetaDataList& v_md, QString name, bool temporary, PlaylistType type) {

	int idx = add_new_playlist(name, temporary, type);
	change_playlist_index(idx);

	get_current()->create_playlist(v_md);
	get_current()->set_temporary( get_current()->is_temporary() && temporary ) ;

	emit_playlist_created();
	return _cur_playlist_idx;
}


// create a new playlist, where only filepaths are given
// Load Folder, Load File...
int PlaylistHandler::create_playlist(const QStringList& pathlist, QString name, bool temporary, PlaylistType type) {

	DirectoryReader reader;
	MetaDataList v_md = reader.get_md_from_filelist(pathlist);
	return create_playlist(v_md, name, temporary, type);
}


int PlaylistHandler::create_playlist(const QString& dir, QString name, bool temporary, PlaylistType type) {

	QStringList lst;
	lst << dir;
	return create_playlist(lst, name, temporary, type);
}

int PlaylistHandler::create_empty_playlist(const QString name, bool temporary){

	MetaDataList v_md;
	return create_playlist(v_md, name, temporary);
}


int PlaylistHandler::create_playlist(const CustomPlaylist& cpl){
	Playlist* pl = 0;

	int idx = create_playlist(cpl.tracks, cpl.name, cpl.is_temporary);

	pl = _playlists[idx];
	pl->set_db_id(cpl.id);

	return idx;
}


void PlaylistHandler::save_playlist(QString filename, bool relative) {

	PlaylistParser::save_playlist(filename, get_current()->get_playlist(), relative);
}


PlaylistDBInterface::SaveAsAnswer PlaylistHandler::save_cur_playlist(){
	return save_playlist(_cur_playlist_idx);
}

PlaylistDBInterface::SaveAsAnswer PlaylistHandler::save_playlist(int idx){

	PlaylistDBInterface::SaveAsAnswer ret = PlaylistDBInterface::SaveAs_Error;

	if(idx < 0 || idx >= _playlists.size()) {
		return ret;
	}

	ret = _playlists[idx]->save();

	emit sig_playlists_changed();

	return ret;
}


PlaylistDBInterface::SaveAsAnswer PlaylistHandler::save_cur_playlist_as(const QString& name, bool force_override){
	return save_playlist_as(_cur_playlist_idx, name, force_override);
}

PlaylistDBInterface::SaveAsAnswer PlaylistHandler::save_playlist_as(int idx, const QString& name, bool force_override){

	PlaylistDBInterface::SaveAsAnswer ret = PlaylistDBInterface::SaveAs_Error;

	if(idx < 0 || idx >= _playlists.size()) {
		return ret;
	}

	int new_idx;
	Playlist* new_pl;
	Playlist* pl = _playlists[idx];

	ret = pl->save_as(name, force_override);

	if(ret != PlaylistDBInterface::SaveAs_Success){
		return ret;
	}

	pl->set_temporary(false);

	new_idx = create_playlist(pl->get_playlist(), name, false);
	new_pl = _playlists[new_idx];
	new_pl->change_track(pl->get_cur_track_idx());

	bool update_cur = (_cur_playlist_idx == idx);
	bool update_active = (_active_playlist_idx == idx);

	close_playlist(idx);

	if(update_cur){
		_cur_playlist_idx = new_pl->get_idx();
	}

	if(update_active){

		_active_playlist_idx = new_pl->get_idx();
		int db_id = get_active()->get_db_id();
		int track_idx = get_active()->get_cur_track_idx();
		_settings->set(Set::PL_LastPlaylist, db_id);
		_settings->set(Set::PL_LastTrack, track_idx);
	}

	emit sig_playlists_changed();

	return PlaylistDBInterface::SaveAs_Success;
}

void PlaylistHandler::delete_playlist(int idx){

	bool success =  _playlists[idx]->remove_from_db();

	if(success){
		emit sig_playlists_changed();
	}
}


void PlaylistHandler::md_changed(const MetaData& md) {

	for( Playlist* pl : _playlists ){
		QList<int> idx;
		idx << pl->find_tracks(md.filepath());

		for(int i : idx){
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

	std::mt19937 rnd_generator;

	MetaData md;
	QList<int> artists_copy = Helper::randomize_list(artists);
	int cur_artist_idx = 0;
    bool is_track_already_in = false;

	do {

		int artist_id = artists_copy.at(cur_artist_idx);
		MetaDataList vec_tracks;
		_db->getAllTracksByArtist(artist_id, vec_tracks);
		std::uniform_int_distribution<int> distr(0, vec_tracks.size() - 1);


		// give each artist several trys
		int max_rounds = vec_tracks.size();
		for(int rounds=0; rounds < max_rounds; rounds++) {

			int rnd_track = distr(rnd_generator);
            md = vec_tracks[rnd_track];

            // search playlist
			if( ! get_active()->find_tracks(md.id).isEmpty() ) {
                is_track_already_in = true;
            }

            else break;
		}

		cur_artist_idx++;

	} while(is_track_already_in && cur_artist_idx < artists_copy.size());


	if(md.id < 0) {
		return;
	}

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


void PlaylistHandler::set_active_idx(int idx){
	_active_playlist_idx = idx;
	_settings->set(Set::PL_LastPlaylist, get_active()->get_db_id());
}


void PlaylistHandler::change_track(int idx, int playlist_idx) {

	if(playlist_idx != _active_playlist_idx && playlist_idx >= 0){
		get_active()->stop();
		set_active_idx(playlist_idx);
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


QString PlaylistHandler::request_new_playlist_name(){

	return PlaylistDBInterface::request_new_db_name();

}


int PlaylistHandler::add_new_playlist(const QString& name, bool temporary, PlaylistType type){

	int idx = exists(name);
	Playlist* pl = NULL;

	if(idx >= 0) {
		return idx;
	}

	idx = _playlists.size();

	pl = new_playlist(type, _playlists.size(), name);
	pl->set_temporary(temporary);

	_playlists.append(pl);

	emit sig_new_playlist_added(idx, name);

	return idx;
}


bool PlaylistHandler::change_playlist_index(int idx){

	if(idx < 0 || idx >= _playlists.size()) return _cur_playlist_idx;

	_cur_playlist_idx = idx;

	if(_play_manager->get_play_state() == PlayManager::PlayState_Stopped){
		set_active_idx(idx);
	}

	emit_playlist_created();

	emit sig_playlist_mode_changed( get_current()->get_playlist_mode());
	emit sig_playlist_index_changed(idx, get_current()->is_temporary());

	return _cur_playlist_idx;
}

void PlaylistHandler::close_playlist(int idx){

	if(_playlists[idx]->is_temporary()){
		QString name = _playlists[idx]->get_name();
		_playlist_db_connector->delete_playlist( name );
	}

	delete _playlists[idx];

	_playlists.removeAt(idx);

	if(_cur_playlist_idx == idx){
		_cur_playlist_idx = 0;
	}

	else if(_cur_playlist_idx > idx){
		_cur_playlist_idx --;
	}

	if(_active_playlist_idx == idx){
		set_active_idx(0);
	}

	else if(_active_playlist_idx > idx){
		_active_playlist_idx --;
	}

	foreach(Playlist* pl, _playlists){
		if(pl->get_idx() >= idx){
			pl->set_idx(pl->get_idx() - 1);
		}
	}

	_settings->set(Set::PL_LastPlaylist, get_active()->get_db_id());

	emit_playlist_created();

	emit sig_playlist_closed(idx);
}


Playlist* PlaylistHandler::get_active(){
	if(_active_playlist_idx < 0 || _active_playlist_idx >= _playlists.size()) {
		return _dummy_playlist;
	}

	return _playlists[_active_playlist_idx]	;
}

Playlist* PlaylistHandler::get_current(){
	if(_cur_playlist_idx < 0 || _cur_playlist_idx >= _playlists.size()) {
		return _dummy_playlist;
	}

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

const MetaDataList& PlaylistHandler::get_tracks(PlaylistIndex which){

	if(which == PlaylistIndex_Current){
		return get_current()->get_playlist();
	}

	else{
		return get_active()->get_playlist();
	}
}
