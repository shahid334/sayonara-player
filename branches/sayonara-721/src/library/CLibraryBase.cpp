/* CLibraryBase.cpp */

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


#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "library/CLibraryBase.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include <QElapsedTimer>
#include "application.h"


#include <QDebug>
#include <QProgressBar>
#include <QFileSystemWatcher>
#include <QListWidget>


CLibraryBase::CLibraryBase(QWidget* main_window, QObject *parent) :
    QObject(parent)
{
    _main_window = main_window;

    CSettingsStorage* settings = CSettingsStorage::getInstance();
    _db = CDatabaseConnector::getInstance();

    m_library_path = settings->getLibraryPath();

    _reload_thread = new ReloadThread();

    QList<int> sortings = settings->getLibSorting();
    _artist_sortorder = (SortOrder) sortings[0];
    _album_sortorder = (SortOrder) sortings[1];
    _track_sortorder = (SortOrder) sortings[2];

    _filter.by_searchstring = BY_FULLTEXT;
    _filter.filtertext = "";
    _reload_progress = 0;

    connect(_reload_thread, SIGNAL(sig_reloading_library(QString)), this, SLOT(library_reloading_state_slot(QString)));
    connect(_reload_thread, SIGNAL(sig_new_block_saved()), this, SLOT(library_reloading_state_new_block()));
    connect(_reload_thread, SIGNAL(finished()), this, SLOT(reload_thread_finished()));
}



void CLibraryBase::emit_stuff(){
    emit sig_all_albums_loaded(_vec_albums);
    emit sig_all_artists_loaded(_vec_artists);
    emit sig_all_tracks_loaded(_vec_md);
}



void CLibraryBase::psl_sortorder_changed(SortOrder artist_so, SortOrder album_so, SortOrder track_so){

    QList<int> lst;
    lst << artist_so << album_so << track_so;
    CSettingsStorage::getInstance()->setLibSorting(lst);

    // artist sort order has changed
    if(artist_so != _artist_sortorder){

        _artist_sortorder = artist_so;
        _vec_artists.clear();

        if(!_filter.cleared){
            _db->getAllArtistsBySearchString(_filter, _vec_artists, _artist_sortorder);
        }

        else{
            _db->getAllArtists(_vec_artists, _artist_sortorder);
        }

        for(uint i=0; i<_vec_artists.size(); i++){
            _vec_artists[i].is_lib_selected = _selected_artists.contains(_vec_artists[i].id);
        }

        emit sig_all_artists_loaded(_vec_artists);
    }


    // album sort order has changed
    if(album_so != _album_sortorder){

        _album_sortorder = album_so;
        _vec_albums.clear();

        // selected artists and maybe filter
        if (_selected_artists.size() > 0){
            _db->getAllAlbumsByArtist(_selected_artists, _vec_albums, _filter, _album_sortorder);
        }

        // only filter
        else if( !_filter.cleared ){
            _db->getAllAlbumsBySearchString(_filter, _vec_albums, _album_sortorder);
        }

        // all albums
        else{
            _db->getAllAlbums(_vec_albums, _album_sortorder);
        }

        for(uint i=0; i<_vec_albums.size(); i++){
            _vec_albums[i].is_lib_selected = _selected_albums.contains(_vec_albums[i].id);
        }

        emit sig_all_albums_loaded(_vec_albums);
    }


    // track sort order has changed
    if(track_so != _track_sortorder){

        _track_sortorder = track_so;
        _vec_md.clear();

        if(_selected_albums.size() > 0){
            _db->getAllTracksByAlbum(_selected_albums, _vec_md, _filter, _track_sortorder);
        }
        else if(_selected_artists.size() > 0){
            _db->getAllTracksByArtist(_selected_artists, _vec_md, _filter, _track_sortorder);
        }

        else if(!_filter.cleared){
            _db->getAllTracksBySearchString(_filter, _vec_md, _track_sortorder);
        }

        else {
            _db->getTracksFromDatabase(_vec_md, _track_sortorder);
        }



        for(uint i=0; i<_vec_md.size(); i++){
            _vec_md[i].is_lib_selected = _selected_tracks.contains(_vec_md[i].id);
        }

        emit sig_all_tracks_loaded(_vec_md);

    }

}
void CLibraryBase::refresh(bool b){

    if(b)
        psl_filter_changed(_filter, true);
}

void CLibraryBase::psl_filter_changed(const Filter& filter, bool force){

    if(     _filter.cleared &&
            filter.cleared &&
            filter.filtertext.size() < 5 &&
            (_selected_artists.size() == 0) &&
            (_selected_albums.size() == 0) &&
            !force) return;

    _filter = filter;

    _vec_albums.clear();
    _vec_artists.clear();
    _vec_md.clear();

    _selected_albums.clear();
    _selected_artists.clear();


    if(_filter.cleared){

        QElapsedTimer timer;
        timer.start();

        _db->getAllArtists(_vec_artists, _artist_sortorder);
        _db->getAllAlbums(_vec_albums, _album_sortorder);
        _db->getTracksFromDatabase(_vec_md, _track_sortorder);

        qDebug() << "Fetched data in " << timer.elapsed();

    }

    else {
        _db->getAllArtistsBySearchString(_filter, _vec_artists, _artist_sortorder);
        _db->getAllAlbumsBySearchString(_filter, _vec_albums, _album_sortorder);
        _db->getAllTracksBySearchString(_filter, _vec_md, _track_sortorder);

    }

    emit_stuff();


}


void CLibraryBase::psl_selected_artists_changed(const QList<int>& idx_list){

    QList<int> selected_artists;

    foreach(int idx, idx_list){
        Artist artist = _vec_artists[idx];
        selected_artists << artist.id;
    }

    if(selected_artists == _selected_artists && _selected_albums.size() == 0) {
        emit sig_all_albums_loaded(_vec_albums);
        emit sig_all_tracks_loaded(_vec_md);
    }

    _vec_albums.clear();
    _vec_md.clear();

    _selected_artists = selected_artists;

    if(_selected_artists.size() > 0){
        _db->getAllTracksByArtist(_selected_artists, _vec_md, _filter, _track_sortorder);
        _db->getAllAlbumsByArtist(_selected_artists, _vec_albums, _filter, _album_sortorder);
    }

    else if(!_filter.cleared){
        _db->getAllTracksBySearchString(_filter, _vec_md, _track_sortorder);
        _db->getAllAlbumsBySearchString(_filter, _vec_albums, _album_sortorder);
        _db->getAllArtistsBySearchString(_filter, _vec_artists, _artist_sortorder);
    }

    else{
        _db->getTracksFromDatabase(_vec_md, _track_sortorder);
        _db->getAllAlbums(_vec_albums, _album_sortorder);
    }

    _selected_albums.clear();
    _selected_tracks.clear();


    emit sig_all_albums_loaded(_vec_albums);
    emit sig_all_tracks_loaded(_vec_md);
}

void CLibraryBase::psl_disc_pressed(int disc){

    if( _selected_albums.size() == 0 ||
            _selected_albums.size() > 1 ||
            disc < 0 ) return;

    MetaDataList v_metadata;
    foreach(MetaData md, _vec_md){
        if(md.discnumber != disc) continue;
        v_metadata.push_back(md);
    }

    _vec_md = v_metadata;

    emit sig_all_tracks_loaded(_vec_md);
}

void CLibraryBase::psl_selected_albums_changed(const QList<int>& idx_list){


    QList<int> selected_albums;

    foreach(int idx, idx_list){
        Album album = _vec_albums[idx];
        _selected_albums << album.id;
    }

    if(selected_albums == _selected_albums) return;

    _vec_md.clear();

    _selected_albums = selected_albums;

    foreach(int idx, idx_list){

        Album album = _vec_albums[idx];
        _selected_albums << album.id;
    }


    // only show tracks of selected album / artist
    if(_selected_artists.size() > 0){
        MetaDataList v_md;
        if(_selected_albums.size() > 0){
            _db->getAllTracksByAlbum(_selected_albums, v_md, _filter, _track_sortorder);

            // filter by artist
            foreach(MetaData md, v_md){
                if(_selected_artists.contains(md.artist_id))
                    _vec_md.push_back(md);
            }
        }

        else
            _db->getAllTracksByArtist(_selected_artists, _vec_md, _filter, _track_sortorder);
    }

    // only album is selected
    else if(_selected_albums.size() > 0){
        _db->getAllTracksByAlbum(_selected_albums, _vec_md, _filter, _track_sortorder);
    }

    // neither album nor artist, but searchstring
    else if(!_filter.cleared){
        _db->getAllTracksBySearchString(_filter, _vec_md, _track_sortorder);
    }

    // no album, no artist, no searchstring
    else{
        _db->getTracksFromDatabase(_vec_md, _track_sortorder);
    }


    QList<int> new_selected_tracks;
    for(uint i=0; i<_vec_md.size(); i++){
        if(!_selected_tracks.contains(_vec_md[i].id)) continue;
        _vec_md[i].is_lib_selected = true;
        new_selected_tracks << _vec_md[i].id;
    }

    _selected_tracks = new_selected_tracks;


    emit sig_all_tracks_loaded(_vec_md);
}


void CLibraryBase::psl_selected_tracks_changed(const QList<int>& idx_list){

    _selected_tracks.clear();

    MetaDataList v_md;

    foreach(int idx,idx_list){
        MetaData md = _vec_md[idx];
        v_md.push_back(md);
        _selected_tracks << md.id;
    }

    emit sig_track_mime_data_available(v_md);
}


void CLibraryBase::psl_track_time_changed(MetaData& md){

    if(md.id < 0) return;

    _db->updateTrack(md);
    emit_stuff();
}

void CLibraryBase::psl_change_id3_tags(const QList<int>& lst){

    // album, artist
    if(lst.size() == 0 && _vec_md.size() > 0){
        emit sig_change_id3_tags(_vec_md);
    }

    // set of tracks
    else if(lst.size()){
        MetaDataList v_md;
        foreach(int i, lst){
            v_md.push_back(_vec_md[i]);
        }

        emit sig_change_id3_tags(v_md);
    }
}



void CLibraryBase::setLibraryPath(QString path){

    m_library_path = path;
}



void CLibraryBase::psl_prepare_artist_for_playlist(int idx){
    emit sig_tracks_for_playlist_available(_vec_md);
}


void CLibraryBase::psl_prepare_album_for_playlist(int idx){
    emit sig_tracks_for_playlist_available(_vec_md);
}


void CLibraryBase::psl_prepare_track_for_playlist(int idx){
    MetaDataList v_md;
    v_md.push_back(_vec_md[idx]);
    emit sig_tracks_for_playlist_available(v_md);
}




void CLibraryBase::psl_play_next_all_tracks(){
    emit sig_play_next_tracks(_vec_md);
}

void CLibraryBase::psl_play_next_tracks(const QList<int>& lst){
    MetaDataList v_md;
    foreach(int i, lst){
        v_md.push_back(_vec_md[i]);
    }

    emit sig_play_next_tracks(v_md);
}


void CLibraryBase::psl_append_all_tracks(){
    emit sig_append_tracks_to_playlist(_vec_md);
}

void CLibraryBase::psl_append_tracks(const QList<int>& lst){
    MetaDataList v_md;
    foreach(int i, lst){
        v_md.push_back(_vec_md[i]);
    }
    emit sig_append_tracks_to_playlist(v_md);
}

