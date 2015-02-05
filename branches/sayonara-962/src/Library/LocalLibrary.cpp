/* LocalLibrary.cpp */

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
 */


#include "HelperStructs/Helper.h"
#include "Library/LocalLibrary.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Filter.h"
#include "application.h"

#include <QProgressBar>
#include <QFileSystemWatcher>
#include <QListWidget>

LocalLibrary::LocalLibrary(QObject *parent) :
	AbstractLibrary(parent)
{
    _db = CDatabaseConnector::getInstance();

	_library_path = _settings->get(Set::Lib_Path);

    _reload_thread = new ReloadThread();

    _reload_progress = 0;

    connect(_reload_thread, SIGNAL(sig_reloading_library(QString)), this, SLOT(library_reloading_state_slot(QString)));
    connect(_reload_thread, SIGNAL(sig_new_block_saved()), this, SLOT(library_reloading_state_new_block()));
    connect(_reload_thread, SIGNAL(finished()), this, SLOT(reload_thread_finished()));


	REGISTER_LISTENER(Set::Lib_Path, _sl_libpath_changed);
}


void LocalLibrary::psl_reload_library(bool clear) {

	_library_path = _settings->get(Set::Lib_Path);

	if(_library_path.isEmpty()) {

		emit sig_no_library_path();
		return;
	}

	if(clear) {
		delete_all_tracks();
	}

	if(_reload_thread->isRunning()) {
		_reload_thread->terminate();
	}

	_reload_thread->set_lib_path(_library_path);
	_reload_thread->start();
}


// TODO:
void LocalLibrary::reload_thread_finished() {

	load();

	MetaDataList v_md_broken;
	ID3::checkForBrokenFiles(_vec_md, v_md_broken);

	emit sig_reload_library_finished();
}

void LocalLibrary::library_reloading_state_new_block() {

	_reload_thread->pause();

	_db->getAllAlbums(_vec_albums, _sortorder.so_albums);
	_db->getAllArtists(_vec_artists, _sortorder.so_artists);
	_db->getTracksFromDatabase(_vec_md, _sortorder.so_tracks);

	emit_stuff();

	_reload_thread->goon();
}


void LocalLibrary::library_reloading_state_slot(QString str) {

	emit sig_reloading_library(str);
}


void LocalLibrary::psl_disc_pressed(int disc) {

    if( _selected_albums.size() == 0 ||
		_selected_albums.size() > 1 )
	{
		return;
	}

	if(disc < 0){
		emit sig_all_tracks_loaded(_vec_md);
		return;
	}


    MetaDataList v_metadata;
    foreach(MetaData md, _vec_md) {
        if(md.discnumber != disc) continue;

        v_metadata.push_back(md);
    }

	emit sig_all_tracks_loaded(v_metadata);
}


void LocalLibrary::_sl_libpath_changed(){
	_library_path = _settings->get(Set::Lib_Path);
}


void LocalLibrary::get_all_artists(ArtistList& artists, LibSortOrder so){
	_db->getAllArtists(artists, so.so_artists);
}

void LocalLibrary::get_all_artists_by_searchstring(Filter filter, ArtistList& artists, LibSortOrder so){
	_db->getAllArtistsBySearchString(filter, artists, so.so_artists);
}


void LocalLibrary::get_all_albums(AlbumList& albums, LibSortOrder so){
	_db->getAllAlbums(albums, so.so_albums);
}

void LocalLibrary::get_all_albums_by_artist(QList<int> artist_ids, AlbumList& albums, Filter filter, LibSortOrder so){
	_db->getAllAlbumsByArtist(artist_ids, albums, filter, so.so_albums)	;
}

void LocalLibrary::get_all_albums_by_searchstring(Filter filter, AlbumList& albums, LibSortOrder so){
	_db->getAllAlbumsBySearchString(filter, albums, so.so_albums);
}

void LocalLibrary::get_all_tracks(MetaDataList& v_md, LibSortOrder so){
	_db->getTracksFromDatabase(v_md, so.so_tracks);
}

void LocalLibrary::get_all_tracks_by_artist(QList<int> artist_ids, MetaDataList& v_md, Filter filter, LibSortOrder so){
	_db->getAllTracksByArtist(artist_ids, v_md, filter, so.so_tracks);
}

void LocalLibrary::get_all_tracks_by_album(QList<int> album_ids, MetaDataList& v_md, Filter filter, LibSortOrder so){
	_db->getAllTracksByAlbum(album_ids, v_md, filter, so.so_tracks);
}

void LocalLibrary::get_all_tracks_by_searchstring(Filter filter, MetaDataList& v_md, LibSortOrder so){
	_db->getAllTracksBySearchString(filter, v_md, so.so_tracks);
}

void LocalLibrary::get_album_by_id(int album_id, Album& album){
	_db->getAlbumByID(album_id, album);
}

void LocalLibrary::get_artist_by_id(int artist_id, Artist& artist){
	_db->getArtistByID(artist_id, artist);
}


void LocalLibrary::update_track(const MetaData& md){
	_db->updateTrack(md);
}

void LocalLibrary::update_album(const Album& album){
	_db->updateAlbum(album);
}


void LocalLibrary::delete_tracks(const MetaDataList &v_md, TrackDeletionMode mode){

	_db->deleteTracks(v_md);

	AbstractLibrary::delete_tracks(v_md, mode);
}
