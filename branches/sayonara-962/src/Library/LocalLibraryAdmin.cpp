/* CLibraryAdmin.cpp */

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


#include "HelperStructs/Helper.h"
#include "Library/LocalLibrary.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Filter.h"
#include "application.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QFileSystemWatcher>
#include <QListWidget>

/// TODO: FORBID RELOAD LIBRARY <=> IMPORT at same time


void LocalLibrary::empty_library() {
    MetaDataList lst;
    _db->getTracksFromDatabase(lst, _sortorder.so_tracks);
    _db->deleteTracks(lst);
    refresh();
}


void LocalLibrary::psl_reload_library(bool clear) {

	_library_path = _settings->get(Set::Lib_Path);

	if(_library_path.isEmpty()) {

		emit sig_no_library_path();
		return;
	}

    if(clear) {
		empty_library();
    }

    if(_reload_thread->isRunning()) {
        _reload_thread->terminate();
    }

	_reload_thread->set_lib_path(_library_path);
    _reload_thread->start();
}


// TODO:
void LocalLibrary::reload_thread_finished() {

    _db->getAllAlbums(_vec_albums);
    _db->getAllArtists(_vec_artists);
    _db->getTracksFromDatabase(_vec_md, _sortorder.so_tracks);

    emit_stuff();

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



void LocalLibrary::loadDataFromDb () {

    _filter.cleared = true;
    _filter.filtertext = "";

    _db->getAllArtists(_vec_artists, _sortorder.so_artists);
    _db->getAllAlbums(_vec_albums, _sortorder.so_albums);
    _db->getTracksFromDatabase(_vec_md, _sortorder.so_tracks);

    emit_stuff();
}



void LocalLibrary::psl_delete_tracks(TrackDeletionMode answer) {
    delete_tracks(_vec_md, answer);
}


void LocalLibrary::psl_delete_certain_tracks(const QList<int>& idx_lst, TrackDeletionMode answer) {

    MetaDataList vec_md;
	foreach(int idx, idx_lst) {
		vec_md.push_back( _vec_md[idx] );
    }

    delete_tracks(vec_md, answer);
}



void LocalLibrary::delete_tracks(const MetaDataList& vec_md, TrackDeletionMode answer) {

    QStringList file_list;
    QString file_entry = tr("files");
    int n_files = vec_md.size();
    int n_fails = 0;

	for(const MetaData& md : vec_md) {
		file_list << md.filepath();
    }

    _db->deleteTracks(vec_md);

	if(answer == TrackDeletionModeAlsoFiles) {
        file_entry = tr("entries");
        foreach(QString filename, file_list) {
            QFile file(filename);
            if( !file.remove() )
                n_fails ++;
        }
    }

    QString answer_str;

    if(n_fails == 0) {
        answer_str = tr("All %1 could be removed").arg(file_entry);
    }

    else {
        answer_str = tr("%1 of %2 %3 could not be removed").arg(n_fails).arg(n_files).arg(file_entry);
    }

    emit sig_delete_answer(answer_str);
    refresh();
}
