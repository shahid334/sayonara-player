/* ReloadThread.cpp */

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


/*
 * ReloadThread.cpp
 *
 *  Created on: Jun 19, 2011
 *      Author: luke
 */

#define N_FILES_TO_STORE 500

#include "library/threads/ReloadThread.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Helper.h"

#include <qmetatype.h>


ReloadThread::ReloadThread() {
	 qRegisterMetaType<QString*>("QString*");
	_state = -1;
	_db = CDatabaseConnector::getInstance();
}

ReloadThread::~ReloadThread() {

}


int ReloadThread::get_and_save_all_files() {

	if(_library_path.size() == 0 || !QFile::exists(_library_path)) return 0;

	QDir dir(_library_path);
	MetaDataList v_md;
	int n_files = 0;

	get_files_recursive (dir, v_md, &n_files);

	n_files += v_md.size();

	if(v_md.size() >  0) {
		_db->storeMetadata(v_md);
		v_md.clear();
	}

	return n_files;
}


void ReloadThread::get_files_recursive(QDir base_dir, MetaDataList& v_md, int* n_files) {

	QStringList soundfile_exts = Helper::get_soundfile_extensions();
	QStringList sub_dirs;
	QStringList sub_files;

	sub_dirs = base_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for(const QString& dir : sub_dirs) {

		base_dir.cd(dir);
			get_files_recursive(base_dir, v_md, n_files);
		base_dir.cdUp();
    }

    int num_files = *n_files;

	sub_files = base_dir.entryList(soundfile_exts, QDir::Files);

	for(const QString& f : sub_files) {

    	MetaData md;
		md.set_filepath( base_dir.absoluteFilePath(f) );

		if( ID3::getMetaDataOfFile(md) ) {
			v_md.push_back(md);
			num_files ++;

			if(num_files % 20 == 0) {
				QString reload_status_str = tr("Reloading %1 tracks").arg(num_files);
                emit sig_reloading_library( reload_status_str );
			}
		}

		if(v_md.size() >= N_FILES_TO_STORE ) {

			_db->storeMetadata(v_md);
            while(_paused) {

				Helper::sleep_ms(10);
            }

            emit sig_new_block_saved();

			v_md.clear();
		}
	}

    *n_files = num_files;
}




int ReloadThread::getState() {
	return _state;
}

void ReloadThread::pause() {
    _paused = true;
}

void ReloadThread::goon() {
    _paused = false;
}

void ReloadThread::run() {

    _paused = false;

	MetaDataList v_metadata, v_to_delete;

    QString reload_status_str = tr("Delete orphaned tracks...");
    emit sig_reloading_library(reload_status_str);

	_db->getTracksFromDatabase(v_metadata);
	qDebug() << "Had " << v_metadata.size() << " tracks";
	v_metadata.clear();

	_db->deleteInvalidTracks();

	_db->getTracksFromDatabase(v_metadata);
	qDebug() << "Have " << v_metadata.size() << " tracks";

	// find orphaned tracks in library && delete them
	for(const MetaData& md : v_metadata){

		if(!Helper::checkTrack(md)) {
			v_to_delete.push_back(md);
		}
	}

	_db->deleteTracks(v_to_delete);

	get_and_save_all_files();

    _paused = false;
}

void ReloadThread::set_lib_path(QString library_path) {
	_library_path = library_path;
}

MetaDataList ReloadThread::get_metadata() {
	return _v_metadata;
}


