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


	QStringList fileList;

	CDirectoryReader reader;
	int num_files = 0;
	reader.getFilesInsiderDirRecursive(QDir(_library_path), fileList, num_files);

	vector<MetaData> v_md;
	_state = -1;

#ifdef OMP_H
	qDebug() << "OpenMP active";
#else
	qDebug() << "OpenMP inactive";
#endif



	for(int i=0; i<fileList.size(); i++){

		qDebug() << fileList.at(i);

		MetaData md = ID3::getMetaDataOfFile(fileList.at(i));

		v_md.push_back(md);
		emit reloading_library( (i * 100) / fileList.size() );
	}

	CDatabaseConnector::getInstance()->deleteTracksAlbumsArtists();
	_v_metadata = v_md;
}

void ReloadThread::set_lib_path(QString library_path){
	_library_path = library_path;
}

void ReloadThread::get_metadata(vector<MetaData>& md){
	md = _v_metadata;
}
