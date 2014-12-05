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
#include "Settings/Settings.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Helper.h"

#include <qmetatype.h>


int ReloadThread::get_and_save_all_files() {

	if(_library_path.size() == 0 || !QFile::exists(_library_path)) return 0;

	QDir dir(_library_path);
	MetaDataList v_md;
	int n_files = 0;

	get_files_recursive (dir, v_md, &n_files);

	n_files += v_md.size();

	if(v_md.size() >  0) {
		CDatabaseConnector::getInstance()->storeMetadata(v_md);
		v_md.clear();
	}

	return n_files;

}


void ReloadThread::get_files_recursive (QDir baseDir, MetaDataList& v_md, int* n_files) {
	QDir baseDirDirs(baseDir);
	QDir baseDirFiles(baseDir);
    QStringList dirs;

	baseDirDirs.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dirs = baseDirDirs.entryList();

    foreach (QString dir, dirs) {
    	baseDirDirs.cd(dir);
        this -> get_files_recursive(baseDirDirs, v_md, n_files);
        baseDirDirs.cd("..");
    }

    int num_files = *n_files;

    dirs.clear();
    baseDirFiles.setFilter(QDir::Files);
    baseDirFiles.setNameFilters( Helper::get_soundfile_extensions() );
    dirs = baseDirFiles.entryList();

    QString reload_status_str;
    foreach (QString f, dirs) {


    	MetaData md;
        md.filepath = baseDirFiles.absoluteFilePath(f);

        if(ID3::getMetaDataOfFile(md)) {
			v_md.push_back(md);
			num_files ++;


			if(num_files % 20 == 0) {

                reload_status_str = tr("Reloading %1 tracks").arg(num_files);


                emit sig_reloading_library( reload_status_str );

			}
		}

		if(v_md.size() >= N_FILES_TO_STORE ) {
			CDatabaseConnector::getInstance()->storeMetadata(v_md);
            while(_paused) {
				Helper::sleep_ms(10);
            }

            emit sig_new_block_saved();

			v_md.clear();
		}
	}

    *n_files = num_files;

}



ReloadThread::ReloadThread() {
	 qRegisterMetaType<QString*>("QString*");
	_state = -1;
}

ReloadThread::~ReloadThread() {

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
	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	MetaDataList v_metadata;
	MetaDataList v_to_delete;

    QString reload_status_str = tr("Delete orphaned tracks...");
    emit sig_reloading_library(reload_status_str);
	db->getTracksFromDatabase(v_metadata);

	// find orphaned tracks in library && delete them
    for(int i=0; i<v_metadata.size(); i++) {
        const MetaData& md = v_metadata[i];
		if(!Helper::checkTrack(md)) {
			v_to_delete.push_back(md);
		}
	}

	db->deleteTracks(v_to_delete);
	v_to_delete.clear();
	v_metadata.clear();

	int n_files = get_and_save_all_files();
	(void) n_files;

    _paused = false;
}

void ReloadThread::set_lib_path(QString library_path) {
	_library_path = library_path;
}

void ReloadThread::get_metadata(MetaDataList& md) {
	md = _v_metadata;
}


