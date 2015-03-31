/* AbstractLibrary.cpp */

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



#include "Library/AbstractLibrary.h"

AbstractLibrary::AbstractLibrary(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
	_playlist = PlaylistHandler::getInstance();
	_sortorder = _settings->get(Set::Lib_Sorting);

	_filter.by_searchstring = BY_FULLTEXT;
	_filter.filtertext = "";

	REGISTER_LISTENER(Set::Lib_Sorting, _sl_sortorder_changed);
}

AbstractLibrary::~AbstractLibrary(){

}


void AbstractLibrary::load () {

	_filter.cleared = true;
	_filter.filtertext = "";

	get_all_artists(_vec_artists, _sortorder);
	get_all_albums(_vec_albums, _sortorder);
	get_all_tracks(_vec_md, _sortorder);

	emit_stuff();
}



void AbstractLibrary::emit_stuff() {

	emit sig_all_albums_loaded(_vec_albums);
	emit sig_all_artists_loaded(_vec_artists);
	emit sig_all_tracks_loaded(_vec_md);
}



void AbstractLibrary::refetch(){

	_selected_albums.clear();
	_selected_artists.clear();
	_selected_tracks.clear();

	_vec_albums.clear();
	_vec_artists.clear();
	_vec_md.clear();

	_filter.cleared = true;
	_filter.filtertext = "";

	get_all_artists(_vec_artists, _sortorder);
	get_all_albums(_vec_albums, _sortorder);
	get_all_tracks(_vec_md, _sortorder);

	emit_stuff();
}



void AbstractLibrary::refresh() {

	QList<int> sel_artists, sel_albums, sel_tracks;
	QList<int> sel_artists_idx, sel_albums_idx, sel_tracks_idx;

	sel_artists = _selected_artists;
	sel_albums = _selected_albums;
	sel_tracks = _selected_tracks;

	fetch_by_filter(_filter, true);

	for(int i=0; i<_vec_artists.size(); i++){
		if(sel_artists.contains(_vec_artists[i].id)) {
			_vec_artists[i].is_lib_selected = true;
			sel_artists_idx << i;
		}
	}

	change_artist_selection(sel_artists_idx);

	for(int i=0; i<_vec_albums.size(); i++){
		if(sel_albums.contains(_vec_albums[i].id)) {
			_vec_albums[i].is_lib_selected = true;
			sel_albums_idx << i;
		}
	}

	change_album_selection(sel_albums_idx);

	for(int i=0; i<_vec_md.size(); i++){
		if(sel_tracks.contains(_vec_md[i].id)) {
			_vec_md[i].is_lib_selected = true;
			sel_tracks_idx << i;
		}
	}

	emit_stuff();

	if(sel_tracks_idx.size() > 0){
		MetaDataList v_md = change_track_selection(sel_tracks_idx);

		if(v_md.size() > 0){
			qDebug() << "1: mimedata";
			emit sig_track_mime_data_available(v_md);
		}
	}
}


void AbstractLibrary::psl_prepare_artist_for_playlist(int idx) {
	Q_UNUSED(idx);
	_playlist->create_playlist(_vec_md);
}


void AbstractLibrary::psl_prepare_album_for_playlist(int idx) {
	Q_UNUSED(idx);
	_playlist->create_playlist(_vec_md);
}


void AbstractLibrary::psl_prepare_track_for_playlist(int idx) {
	QList<int> lst;
	lst << idx;
	psl_prepare_tracks_for_playlist(lst);
}

void AbstractLibrary::psl_prepare_tracks_for_playlist(QList<int> idx_lst) {
	MetaDataList v_md;

	foreach(int idx, idx_lst) {
		v_md.push_back(_vec_md[idx]);
	}

	_playlist->create_playlist(v_md);
}


void AbstractLibrary::psl_play_next_all_tracks() {
	_playlist->play_next(_vec_md);

}

void AbstractLibrary::psl_play_next_tracks(const QList<int>& idx_lst) {
	MetaDataList v_md;
	foreach(int idx, idx_lst) {
		v_md.push_back(_vec_md[idx]);
	}

	_playlist->play_next(v_md);
}


void AbstractLibrary::psl_append_all_tracks() {
	_playlist->append_tracks(_vec_md);
}


void AbstractLibrary::psl_append_tracks(const QList<int>& lst) {
	MetaDataList v_md;
	foreach(int i, lst) {
		v_md.push_back(_vec_md[i]);
	}

	_playlist->append_tracks(_vec_md);
}


void AbstractLibrary::restore_artist_selection(const QList<int>& old_selected_idx){

	QList<int> new_selected_artists;
	for(int i=0; i<_vec_artists.size(); i++) {
		if(!_selected_artists.contains(_vec_artists[i].id)) continue;
		_vec_artists[i].is_lib_selected = true;
		new_selected_artists << _vec_artists[i].id;
	}

	_selected_artists = new_selected_artists;
}


void AbstractLibrary::restore_album_selection(const QList<int>& old_selected_idx){

	QList<int> new_selected_albums;
	for(int i=0; i<_vec_albums.size(); i++) {
		if(!_selected_albums.contains(_vec_albums[i].id)) continue;
		_vec_albums[i].is_lib_selected = true;
		new_selected_albums << _vec_albums[i].id;
	}

	_selected_albums = new_selected_albums;
}


void AbstractLibrary::restore_track_selection(const QList<int>& old_selected_idx){

	QList<int> new_selected_tracks;
	for(int i=0; i<_vec_md.size(); i++) {
		if(!old_selected_idx.contains(_vec_md[i].id)) continue;

		qDebug() << "Restore tracks: " << i;
		_vec_md[i].is_lib_selected = true;
		new_selected_tracks << _vec_md[i].id;
	}

	_selected_tracks = new_selected_tracks;
}




void AbstractLibrary::change_artist_selection(const QList<int>& idx_list){

	QList<int> selected_artists;

	foreach(int idx, idx_list) {
		Artist artist = _vec_artists[idx];
		selected_artists << artist.id;
	}

	if(selected_artists == _selected_artists && _selected_albums.size() == 0) {

		restore_album_selection(_selected_albums);
		restore_track_selection(_selected_tracks);

		emit sig_all_albums_loaded(_vec_albums);
		emit sig_all_tracks_loaded(_vec_md);
		return;
	}

	_vec_albums.clear();
	_vec_md.clear();

	_selected_artists = selected_artists;

	if(_selected_artists.size() > 0) {
		get_all_tracks_by_artist(_selected_artists, _vec_md, _filter, _sortorder);
		get_all_albums_by_artist(_selected_artists, _vec_albums, _filter, _sortorder);
	}

	else if(!_filter.cleared) {
		get_all_tracks_by_searchstring(_filter, _vec_md, _sortorder);
		get_all_albums_by_searchstring(_filter, _vec_albums, _sortorder);
		get_all_artists_by_searchstring(_filter, _vec_artists, _sortorder);
	}

	else{
		get_all_tracks(_vec_md, _sortorder);
		get_all_albums(_vec_albums, _sortorder);
	}

	restore_album_selection(_selected_albums);
	restore_track_selection(_selected_tracks);
}




void AbstractLibrary::psl_selected_artists_changed(const QList<int>& idx_list) {

	change_artist_selection(idx_list);

	emit sig_all_albums_loaded(_vec_albums);
	emit sig_all_tracks_loaded(_vec_md);
}


void AbstractLibrary::change_album_selection(const QList<int>& idx_list){

	QList<int> selected_albums;

	foreach(int idx, idx_list) {
		Album album = _vec_albums[idx];
		selected_albums << album.id;
	}

	if(selected_albums == _selected_albums) return;

	_vec_md.clear();
	_selected_albums = selected_albums;

	// only show tracks of selected album / artist
	if(_selected_artists.size() > 0) {
		MetaDataList v_md;
		if(_selected_albums.size() > 0) {

			get_all_tracks_by_album(_selected_albums, v_md, _filter, _sortorder);

			// filter by artist
			foreach(MetaData md, v_md) {
				if(_selected_artists.contains(md.artist_id))
					_vec_md.push_back(md);
			}
		}

		else{
			get_all_tracks_by_artist(_selected_artists, _vec_md, _filter, _sortorder);
		}
	}

	// only album is selected
	else if(_selected_albums.size() > 0) {
		get_all_tracks_by_album(_selected_albums, _vec_md, _filter, _sortorder);
	}

	// neither album nor artist, but searchstring
	else if(!_filter.cleared) {
		get_all_tracks_by_searchstring(_filter, _vec_md, _sortorder);
	}

	// no album, no artist, no searchstring
	else{
		get_all_tracks(_vec_md, _sortorder);
	}

	restore_track_selection(_selected_tracks);
}


void AbstractLibrary::psl_selected_albums_changed(const QList<int>& idx_list) {

	change_album_selection(idx_list);
	emit sig_all_tracks_loaded(_vec_md);
}


MetaDataList AbstractLibrary::change_track_selection(const QList<int>& idx_list){

	_selected_tracks.clear();

	MetaDataList v_md;

	foreach(int idx,idx_list) {
		const MetaData& md = _vec_md[idx];
		v_md.push_back(md);
		_selected_tracks << md.id;
	}

	return v_md;
}



void AbstractLibrary::psl_selected_tracks_changed(const QList<int>& idx_list) {

	MetaDataList v_md =	change_track_selection(idx_list);
	if(v_md.size() > 0){
		emit sig_track_mime_data_available(v_md);
	}
}


void AbstractLibrary::fetch_by_filter(const Filter& filter, bool force){

	if( _filter.cleared &&
		filter.cleared &&
		filter.filtertext.size() < 5 &&
		(_selected_artists.size() == 0) &&
		(_selected_albums.size() == 0) &&
		!force) {

		return;
	}

	_filter = filter;

	_vec_albums.clear();
	_vec_artists.clear();
	_vec_md.clear();

	_selected_albums.clear();
	_selected_artists.clear();


	if(_filter.cleared) {

		get_all_artists(_vec_artists, _sortorder);
		get_all_albums(_vec_albums, _sortorder);
		get_all_tracks(_vec_md, _sortorder);
	}

	else {
		get_all_artists_by_searchstring(_filter, _vec_artists, _sortorder);
		get_all_albums_by_searchstring(_filter, _vec_albums, _sortorder);
		get_all_tracks_by_searchstring(_filter, _vec_md, _sortorder);
	}
}


void AbstractLibrary::psl_filter_changed(const Filter& filter, bool force) {

	//qDebug() << "Library: Filter changed";
	fetch_by_filter(filter, force);
	emit_stuff();
}




void AbstractLibrary::_sl_sortorder_changed() {

	LibSortOrder so = _settings->get(Set::Lib_Sorting);

	// artist sort order has changed
	if(so.so_artists != _sortorder.so_artists) {

		_sortorder = so;
		_vec_artists.clear();

		if(!_filter.cleared) {
			get_all_artists_by_searchstring(_filter, _vec_artists, _sortorder);
		}

		else{
			get_all_artists(_vec_artists, _sortorder);
		}

		for(Artist& artist : _vec_artists) {
			artist.is_lib_selected = _selected_artists.contains(artist.id);
		}

		emit sig_all_artists_loaded(_vec_artists);
	}


	// album sort order has changed
	if(so.so_albums != _sortorder.so_albums) {

		_sortorder = so;
		_vec_albums.clear();

		// selected artists and maybe filter
		if (_selected_artists.size() > 0) {
			get_all_albums_by_artist(_selected_artists, _vec_albums, _filter, _sortorder);
		}

		// only filter
		else if( !_filter.cleared ) {
			get_all_albums_by_searchstring(_filter, _vec_albums, _sortorder);
		}

		// all albums
		else{
			get_all_albums(_vec_albums, _sortorder);
		}

		for(int i=0; i<_vec_albums.size(); i++) {
			_vec_albums[i].is_lib_selected = _selected_albums.contains(_vec_albums[i].id);
		}

		emit sig_all_albums_loaded(_vec_albums);
	}


	// track sort order has changed
	if(so.so_tracks != _sortorder.so_tracks) {

		_sortorder = so;
		_vec_md.clear();

		if(_selected_albums.size() > 0) {
			get_all_tracks_by_album(_selected_albums, _vec_md, _filter, _sortorder);
		}
		else if(_selected_artists.size() > 0) {
			get_all_tracks_by_artist(_selected_artists, _vec_md, _filter, _sortorder);
		}

		else if(!_filter.cleared) {
			get_all_tracks_by_searchstring(_filter, _vec_md, _sortorder);
		}

		else {
			get_all_tracks(_vec_md, _sortorder);
		}

		for(int i=0; i<_vec_md.size(); i++) {
			_vec_md[i].is_lib_selected = _selected_tracks.contains(_vec_md[i].id);
		}

		emit sig_all_tracks_loaded(_vec_md);
	}
}



void AbstractLibrary::psl_track_rating_changed(int idx, int rating) {

	_vec_md[idx].rating = rating;
	update_track(_vec_md[idx]);
}

void AbstractLibrary::psl_album_rating_changed(int idx, int rating) {

	_vec_albums[idx].rating = rating;
	update_album(_vec_albums[idx]);
}



void AbstractLibrary::psl_metadata_changed(const MetaData& md) {

	if( md.id < 0 ) return;

	if(_old_md.length_ms == md.length_ms) return;

	if(md.id >= 0){
		update_track(md);
		refresh();
	}

	emit_stuff();
}




void AbstractLibrary::psl_metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new){

	// check for new artists and albums
	for(int i=0; i<v_md_old.size(); i++){
		int new_artist_id = v_md_new[i].artist_id;
		int new_album_id = v_md_new[i].album_id;

		if( v_md_old[i].artist_id != new_artist_id ){
			if( !_selected_artists.contains(new_artist_id) ){
				_selected_artists << new_artist_id;
			}
		}

		if( v_md_old[i].album_id != new_album_id){
			if( !_selected_albums.contains(new_album_id) ){
				_selected_albums << new_album_id;
			}
		}
	}

	refresh();
}



void AbstractLibrary::delete_current_tracks(TrackDeletionMode mode){
	delete_tracks(_vec_md, mode);
}

void AbstractLibrary::delete_all_tracks(){

	MetaDataList v_md;
	get_all_tracks(v_md, _sortorder);
	delete_tracks(v_md, TrackDeletionModeOnlyLibrary);
}



void AbstractLibrary::delete_tracks(const MetaDataList &v_md, TrackDeletionMode mode){

	QString file_entry = tr("entries");
	QString answer_str;

	int n_fails = 0;
	if(mode == TrackDeletionModeAlsoFiles){
		file_entry = tr("files");
		for( const MetaData& md : v_md ){
			QFile f(md.filepath());
			if(!f.remove()){
				n_fails++;
			}
		}
	}

	if(n_fails == 0) {
		answer_str = tr("All %1 could be removed").arg(file_entry);
	}

	else {
		answer_str = tr("%1 of %2 %3 could not be removed").arg(n_fails).arg(v_md.size()).arg(file_entry);
	}

	emit sig_delete_answer(answer_str);

	refresh();
}


void AbstractLibrary::delete_tracks_by_idx(const QList<int>& idxs, TrackDeletionMode mode){

	MetaDataList v_md;
	for(int i : idxs){
		v_md.push_back(_vec_md[i]);
	}

	delete_tracks(v_md, mode);
}
