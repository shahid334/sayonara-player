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
#include "library/ReloadThread.h"
#include "library/CLibraryBase.h"
#include "GUI/library/GUIImportFolder.h"
#include "HelperStructs/id3.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"

#include <QDebug>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QFileSystemWatcher>
#include <QInputDialog>
#include <QListWidget>


CLibraryBase::CLibraryBase(QObject *parent) :
    QObject(parent)
{
	m_library_path = CSettingsStorage::getInstance()->getLibraryPath();
	m_thread = new ReloadThread();
	m_watcher = new QFileSystemWatcher();
	m_watcher->addPath(m_library_path);
	m_import_dialog = 0;

	_db = CDatabaseConnector::getInstance();

	_track_sortorder = TrackArtistAsc;
	_album_sortorder = AlbumNameAsc;
	_artist_sortorder = ArtistNameAsc;

	_filter.by_searchstring = BY_FULLTEXT;
	_filter.filtertext = "";
	_reload_progress = 0;

	connect(m_thread, SIGNAL(finished()), this, SLOT(reload_thread_finished()));
	connect(m_watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(file_system_changed(const QString&)));
	connect(m_thread, SIGNAL(reloading_library(QString)), this, SLOT(library_reloading_state_slot(QString)));
}


void CLibraryBase::baseDirSelected (const QString & baseDir) {

    QStringList fileList;
    int num_files = 0;

    m_reader.getFilesInsiderDirRecursive(QDir(baseDir),fileList, num_files);

    emit sig_playlist_created(fileList);

}

void CLibraryBase::importDirectory(QString directory){

	m_library_path = CSettingsStorage::getInstance()->getLibraryPath();
	m_src_dir = directory;

	QDir lib_dir(m_library_path);
	QDir src_dir(directory);

	QDir tmp_src_dir = src_dir;
	tmp_src_dir.cdUp();

	QString rel_src_path = tmp_src_dir.relativeFilePath(directory) + QDir::separator();
	QStringList content = lib_dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

	content.push_front("");

	if(m_import_dialog){
		disconnect(m_import_dialog, SIGNAL(accepted(const QString&, bool)), this, SLOT(importDirectoryAccepted(const QString&, bool)));
		delete m_import_dialog;
		m_import_dialog = 0;
	}

	m_import_dialog = new GUI_ImportFolder(NULL, content, true);
	connect(m_import_dialog, SIGNAL(accepted(const QString&, bool)), this, SLOT(importDirectoryAccepted(const QString&, bool)));
	m_import_dialog->show();
}

void CLibraryBase::importFiles(const MetaDataList& v_md){

	MetaDataList v_md_new = v_md;
	bool success = CDatabaseConnector::getInstance()->storeMetadata(v_md_new);
	emit sig_import_result(success);
}




void CLibraryBase::importDirectoryAccepted(const QString& chosen_item, bool copy){

	QDir lib_dir(m_library_path);
	QDir src_dir(m_src_dir);
	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	if(!copy){
		CDirectoryReader reader;
		QStringList files;
		int n_files;
		reader.getFilesInsiderDirRecursive(src_dir, files, n_files);
		MetaDataList v_md;
		foreach(QString filename, files){
			MetaData md;
			md = _db->getTrackByPath(md.filepath);

			if(md.id < 0){
                if(!ID3::getMetaDataOfFile(md)) continue;
			}

			v_md.push_back(md);
		}

		bool success = db->storeMetadata(v_md);

		emit sig_import_result(success);
		emit sig_reload_library_finished();

		return;
	}

	QDir tmp_src_dir = src_dir;
	tmp_src_dir.cdUp();

	QString rel_src_path = tmp_src_dir.relativeFilePath(m_src_dir) + QDir::separator();

	QString target_path = m_library_path +
				QDir::separator() +
				chosen_item +
				QDir::separator() +
				rel_src_path.replace(" ", "_");

		QStringList files2copy;
		files2copy.push_back(src_dir.absolutePath());
		int num_files = 1;

		for(int i=0; i<num_files; i++){

			// fetch all entries of a file, maybe it's a directory
			QDir sub_dir(files2copy[i]);
			QStringList sub_files = sub_dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst);

			// it is a directory
			if(sub_files.size() != 0){

				// create this directory
				QDir tmp_src_dir(src_dir);
				QDir new_sub_dir( target_path + tmp_src_dir.relativeFilePath(files2copy[i]) );
				sub_dir.mkpath(new_sub_dir.path());

				// remove this "file" from list
				files2copy.removeAt(i);
				num_files--;


				// insert all files/directories from subdir
				// into array
				for(int j=0; j<sub_files.size(); j++){
					files2copy.insert(i+j, sub_dir.path() + QDir::separator() + sub_files[j] );
				}

				num_files += sub_files.size();
				i--;
			}
		}


		// copy & save to database
		MetaDataList v_metadata;

		bool success = false;
		for(int i=0; i<files2copy.size(); i++){

			// target path + relative src path
			QDir tmp_src_dir(src_dir);
			QString new_filename = target_path + tmp_src_dir.relativeFilePath(files2copy[i]);

			QFile f(files2copy[i]);
			if( f.copy(new_filename) ){
				success = true;
				int percent = (i * 10000) / (100 * files2copy.size());
				if( i== files2copy.size() -1) {
					m_import_dialog->close();
				}
				else m_import_dialog->progress_changed(percent);
				if(Helper::is_soundfile(new_filename)){
					MetaData md;
                    md.filepath = new_filename;
                    if( ID3::getMetaDataOfFile( md))
						v_metadata.push_back( md );
				}
			}
		}

		success &= db->storeMetadata(v_metadata);
		emit sig_import_result(success);

}


void CLibraryBase::reloadLibrary(){

	m_library_path = CSettingsStorage::getInstance()->getLibraryPath();

	if(m_library_path.length() == 0) {
		QMessageBox msgBox;
		msgBox.setText("Please select your library first");
		msgBox.exec();

		QString dir = QFileDialog::getExistingDirectory(0, tr("Open Directory"),	getenv("$HOME"), QFileDialog::ShowDirsOnly);

		if(dir.length() < 3) {
			QMessageBox msgBox;
			msgBox.setText("I said: \"Please select your library first\". Bye bye!");
			msgBox.exec();
			return;
		}

		else {
			m_library_path = dir;
			CSettingsStorage::getInstance()->setLibraryPath(dir);
		}
	}


	if(m_thread->isRunning()){
		m_thread->terminate();
	}

	m_thread->set_lib_path(m_library_path);
	m_thread->start();


}

// TODO:

void CLibraryBase::reload_thread_finished(){

	_db->getAllAlbums(_vec_albums);
	_db->getAllArtists(_vec_artists);
    _db->getTracksFromDatabase(_vec_md);


	emit sig_reload_library_finished();
}

void CLibraryBase::library_reloading_state_slot(QString str){

	emit sig_reloading_library(str);

}



void CLibraryBase::insertMetaDataIntoDB(MetaDataList& v_md) {

    _db->storeMetadata(v_md);

    MetaDataList data;
    _db->getTracksFromDatabase(data);
    emit sig_metadata_loaded(data);
}



void CLibraryBase::loadDataFromDb () {



	_filter.cleared = true;
	_filter.filtertext = "";

	_db->getAllArtists(_vec_artists);
	_db->getAllAlbums(_vec_albums);
	_db->getTracksFromDatabase(_vec_md);

	emit_stuff();
}


void CLibraryBase::emit_stuff(){
	emit sig_all_albums_loaded(_vec_albums);
	emit sig_all_artists_loaded(_vec_artists);
	emit sig_metadata_loaded(_vec_md);
}



void CLibraryBase::psl_sortorder_changed(ArtistSort artist_so, AlbumSort album_so, TrackSort track_so){

    // artist sort order has changed
	if(artist_so != _artist_sortorder){

		_artist_sortorder = artist_so;
		_vec_artists.clear();

        ArtistList tmp_artists;
		if(!_filter.cleared){
            _db->getAllArtistsBySearchString(_filter, tmp_artists, _artist_sortorder);
		}

		else{
            _db->getAllArtists(tmp_artists, _artist_sortorder);
		}

        foreach(Artist artist, tmp_artists){
            if(_selected_artists.contains(artist.id))
                artist.is_lib_selected = true;

            _vec_artists.push_back(artist);
        }

		emit sig_all_artists_loaded(_vec_artists);
	}


    // album sort order has changed
    else if(album_so != _album_sortorder){

		_album_sortorder = album_so;
		_vec_albums.clear();

        vector<Album> tmp_albums;
		// selected artists and maybe filter
		if (_selected_artists.size() > 0){
            _db->getAllAlbumsByArtist(_selected_artists, tmp_albums, _filter, _album_sortorder);
        }

		// only filter
		else if( !_filter.cleared ){
            _db->getAllAlbumsBySearchString(_filter, tmp_albums, _album_sortorder);
		}

		// all albums
		else{
            _db->getAllAlbums(tmp_albums, _album_sortorder);
		}

        foreach(Album album, tmp_albums){
            if(_selected_albums.contains(album.id))
                album.is_lib_selected = true;

            _vec_albums.push_back(album);
        }

		emit sig_all_albums_loaded(_vec_albums);
	}


    // track sort order has changed
    else if(track_so != _track_sortorder){

		_track_sortorder = track_so;
		_vec_md.clear();

        MetaDataList v_md_tmp;
		if(_selected_albums.size() > 0){
            _db->getAllTracksByAlbum(_selected_albums, v_md_tmp, _filter, _track_sortorder);
		}
		else if(_selected_artists.size() > 0){
            _db->getAllTracksByArtist(_selected_artists, v_md_tmp, _filter, _track_sortorder);
		}

		else if(!_filter.cleared){
            _db->getAllTracksBySearchString(_filter, v_md_tmp, _track_sortorder);
		}

		else {
            _db->getTracksFromDatabase(v_md_tmp, _track_sortorder);
		}


        foreach(MetaData md, v_md_tmp){
            if(_selected_tracks.contains(md.id)){
                md.is_lib_selected = true;
            }
            _vec_md.push_back(md);
        }

		emit sig_metadata_loaded(_vec_md);
	}
}


void CLibraryBase::psl_filter_changed(const Filter& filter){

    if(     _filter.cleared &&
            filter.cleared &&
            filter.filtertext.size() < 5 &&
            (_selected_artists.size() == 0) &&
            (_selected_albums.size() == 0)) return;

    _filter = filter;

	_vec_albums.clear();
	_vec_artists.clear();
	_vec_md.clear();

    _selected_albums.clear();
    _selected_artists.clear();

	if(_filter.cleared){
		_db->getAllArtists(_vec_artists, _artist_sortorder);
		_db->getAllAlbums(_vec_albums, _album_sortorder);
		_db->getTracksFromDatabase(_vec_md, _track_sortorder);
    }

	else {
		_db->getAllArtistsBySearchString(_filter, _vec_artists, _artist_sortorder);
		_db->getAllAlbumsBySearchString(_filter, _vec_albums, _album_sortorder);
		_db->getAllTracksBySearchString(_filter, _vec_md, _track_sortorder);
	}

	emit_stuff();
}


void CLibraryBase::psl_selected_artists_changed(const QList<int>& idx_list){

	_vec_albums.clear();
    _vec_md.clear();

	_selected_artists.clear();
	_selected_albums.clear();
    _selected_tracks.clear();

    foreach(int idx, idx_list){
		Artist artist = _vec_artists[idx];
		_selected_artists << artist.id;
	}

    if(_selected_artists.size() > 0){
        _db->getAllTracksByArtist(_selected_artists, _vec_md, _filter, _track_sortorder);
        _db->getAllAlbumsByArtist(_selected_artists, _vec_albums, _filter, _album_sortorder);
    }

    else if(!_filter.cleared){
        _db->getAllTracksBySearchString(_filter, _vec_md, _track_sortorder);
        _db->getAllAlbumsBySearchString(_filter, _vec_albums, _album_sortorder);
    }
    else{
        _db->getTracksFromDatabase(_vec_md, _track_sortorder);
        _db->getAllAlbums(_vec_albums, _album_sortorder);
    }

	emit sig_all_albums_loaded(_vec_albums);
	emit sig_metadata_loaded(_vec_md);
}



void CLibraryBase::psl_selected_albums_changed(const QList<int>& idx_list){

	_vec_md.clear();
	_selected_albums.clear();
    _selected_tracks.clear();

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

	emit sig_metadata_loaded(_vec_md);
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

void CLibraryBase::psl_change_id3_tags(const QList<int>& lst){

	// album, artist
	if(lst.size() == 0 && _vec_md.size() > 0)
		emit sig_change_id3_tags(_vec_md);

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

void CLibraryBase::file_system_changed(const QString& path){
	Q_UNUSED(path);

	emit sig_should_reload_library();
}

void CLibraryBase::psl_prepare_artist_for_playlist(){
	emit sig_tracks_for_playlist_available(_vec_md);
}


void CLibraryBase::psl_prepare_album_for_playlist(){
	emit sig_tracks_for_playlist_available(_vec_md);
}


void CLibraryBase::psl_prepare_track_for_playlist(int idx){
	MetaDataList v_md;
	v_md.push_back(_vec_md[idx]);
	emit sig_tracks_for_playlist_available(v_md);
}



void CLibraryBase::delete_tracks(MetaDataList& vec_md, int answer){

	QStringList file_list;
	QString file_entry = "files";
	int n_files = vec_md.size();
	int n_fails = 0;

	foreach(MetaData md, vec_md){
		file_list.push_back(md.filepath);
	}

	_db->deleteTracks(vec_md);
	vec_md.clear();

	if(answer == 1){
		file_entry = "entries";
		foreach(QString filename, file_list){
			QFile file(filename);
			if( !file.remove() )
				n_fails ++;
		}
	}

	QString answer_str;

	if(n_fails == 0){
		answer_str = "All " + file_entry + " could be removed";
	}

	else {
		answer_str = QString::number(n_fails) + " of " + QString::number(n_files) + " " + file_entry + " could not be removed";
	}

	emit sig_delete_answer(answer_str);
}


void CLibraryBase::psl_delete_tracks(int answer){
	delete_tracks(_vec_md, answer);
}


void CLibraryBase::psl_delete_certain_tracks(const QList<int>& lst, int answer){

	MetaDataList vec_md;
	foreach(int idx, lst){
		vec_md.push_back(_vec_md[idx]);
	}

	delete_tracks(vec_md, answer);
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

