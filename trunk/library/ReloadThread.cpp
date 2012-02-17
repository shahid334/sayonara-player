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

#include "library/ReloadThread.h"
#include "HelperStructs/CDirectoryReader.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/id3.h"
#include "HelperStructs/Helper.h"

#include <QStringList>
#include <QDebug>
#include <vector>
#include <omp.h>

using namespace std;

ReloadThread::ReloadThread() {
	_state = -1;
}

ReloadThread::~ReloadThread() {

}

int ReloadThread::getState(){
	return _state;
}


void ReloadThread::run(){

	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	vector<MetaData> v_metadata;
	vector<MetaData> v_to_delete;

	emit reloading_library( -2 );
	db->getTracksFromDatabase(v_metadata);

	for(uint i=0; i<v_metadata.size(); i++){
		MetaData md = v_metadata.at(i);
		if(!Helper::checkTrack((const MetaData&) md)){
			v_to_delete.push_back(md);
		}
	}

	db->deleteTracks(v_to_delete);
	v_to_delete.clear();

	QStringList fileList;

	CDirectoryReader reader;
	int num_files = 0;
	emit reloading_library( -1 );
	reader.getFilesInsiderDirRecursive(QDir(_library_path), fileList, num_files);

	vector<MetaData> v_md;
	_state = -1;

#ifdef OMP_H
	qDebug() << "OpenMP active";
#else
	qDebug() << "OpenMP inactive";
#endif

	/// TODO: commit status every 20-30 tracks
	// to give user a response what happens
	// we're in a thread, baby! You can play the
	// sound nevertheless... Fuck I'm drunk :(
	for(int i=0; i<fileList.size(); i++){

		qDebug() << fileList.at(i);

		MetaData md = ID3::getMetaDataOfFile(fileList.at(i));

		v_md.push_back(md);
		emit reloading_library( (i * 100) / fileList.size() );
	}

	_v_metadata = v_md;
}

void ReloadThread::set_lib_path(QString library_path){
	_library_path = library_path;
}

void ReloadThread::get_metadata(vector<MetaData>& md){
	md = _v_metadata;
}
