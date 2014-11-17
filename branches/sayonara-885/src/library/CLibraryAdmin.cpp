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


#include "Settings/Settings.h"
#include "HelperStructs/Helper.h"
#include "library/CLibraryBase.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Filter.h"
#include "application.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QFileSystemWatcher>
#include <QListWidget>

/// TODO: FORBIN RELOAD LIBRARY <=> IMPORT at same time

void CLibraryBase::baseDirSelected (const QString & baseDir) {

    QStringList fileList;
    int num_files = 0;

    _reader.getFilesInsiderDirRecursive(QDir(baseDir),fileList, num_files);

    emit sig_playlist_created(fileList);

}



void CLibraryBase::clearLibrary() {
    MetaDataList lst;
	_db->getTracksFromDatabase(lst, _track_sortorder);
    _db->deleteTracks(lst);
    refresh();
}


void CLibraryBase::reloadLibrary(bool clear) {

	Settings* settings = Settings::getInstance();
	m_library_path = settings->get(Set::Lib_Path);

    if(m_library_path.length() == 0) {
        QMessageBox msgBox(_main_window);
        msgBox.setText(tr("Please select your library first"));
        msgBox.exec();

        QString dir = QFileDialog::getExistingDirectory(_main_window, tr("Open Directory"),	getenv("$HOME"), QFileDialog::ShowDirsOnly);


        if(dir.length() < 3) {
            QMessageBox msgBox(_main_window);
            msgBox.setText(tr("I said: \"Please select your library first\". Bye bye!"));
            msgBox.exec();
            return;
        }

        else {
            m_library_path = dir;
			settings->set(Set::Lib_Path, dir);
            emit sig_libpath_set(dir);
        }
    }


    if(clear) {
        clearLibrary();
    }

    if(_reload_thread->isRunning()) {
        _reload_thread->terminate();
    }

    _reload_thread->set_lib_path(m_library_path);
    _reload_thread->start();
}


// TODO:
void CLibraryBase::reload_thread_finished() {

    _db->getAllAlbums(_vec_albums);
    _db->getAllArtists(_vec_artists);
	_db->getTracksFromDatabase(_vec_md, _track_sortorder);

    emit_stuff();

	MetaDataList v_md_broken;
	ID3::checkForBrokenFiles(_vec_md, v_md_broken);

    emit sig_reload_library_finished();
}

void CLibraryBase::library_reloading_state_new_block() {

    _reload_thread->pause();

	_db->getAllAlbums(_vec_albums, _album_sortorder);
	_db->getAllArtists(_vec_artists, _artist_sortorder);
	_db->getTracksFromDatabase(_vec_md, _track_sortorder);

    emit_stuff();

    _reload_thread->goon();

}

void CLibraryBase::library_reloading_state_slot(QString str) {

    emit sig_reloading_library(str);
}


void CLibraryBase::insertMetaDataIntoDB(MetaDataList& v_md) {

    _db->storeMetadata(v_md);

    MetaDataList data;
	_db->getTracksFromDatabase(data, _track_sortorder);
    emit sig_all_tracks_loaded(data);
}



void CLibraryBase::loadDataFromDb () {

    _filter.cleared = true;
    _filter.filtertext = "";

	_db->getAllArtists(_vec_artists, _artist_sortorder);
	_db->getAllAlbums(_vec_albums, _album_sortorder);
	_db->getTracksFromDatabase(_vec_md, _track_sortorder);

    emit_stuff();
}



void CLibraryBase::psl_delete_tracks(int answer) {
    delete_tracks(_vec_md, answer);
}


void CLibraryBase::psl_delete_certain_tracks(const QList<int>& lst, int answer) {

    MetaDataList vec_md;
    foreach(int idx, lst) {
        vec_md.push_back(_vec_md[idx]);
    }

    delete_tracks(vec_md, answer);
}



void CLibraryBase::delete_tracks(MetaDataList& vec_md, int answer) {

    QStringList file_list;
    QString file_entry = tr("files");
    int n_files = vec_md.size();
    int n_fails = 0;

    foreach(MetaData md, vec_md) {
        file_list.push_back(md.filepath);
    }

    _db->deleteTracks(vec_md);
    vec_md.clear();

    if(answer == 1) {
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
