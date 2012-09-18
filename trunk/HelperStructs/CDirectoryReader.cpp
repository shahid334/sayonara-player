/* CDirectoryReader.cpp */

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

#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/id3.h"
#include <QDebug>
#include <QDir>


CDirectoryReader::CDirectoryReader () {

	this->m_filters = Helper::get_soundfile_extensions();

}
CDirectoryReader::~CDirectoryReader () {

}


void CDirectoryReader::setFilter (const QStringList & filter) {
    qDebug() <<  Q_FUNC_INFO << "Setting filter to: " << filter;
    this -> m_filters = filter;
}

void CDirectoryReader::getFilesInsiderDirRecursive (QDir baseDir, vector<MetaData>& v_md) {

    QStringList dirs;
	baseDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dirs = baseDir.entryList();

    foreach (QString dir, dirs) {

    	baseDir.cd(dir);
        this -> getFilesInsiderDirRecursive(baseDir, v_md);

        baseDir.cd("..");
    }

    baseDir.setFilter(QDir::Files);
    baseDir.setNameFilters(this -> m_filters);
    this -> getFilesInsideDirectory (baseDir, v_md);
    if(v_md.size() >= NUM_FILES_TO_SAVE){
	CDatabaseConnector::getInstance()->storeMetadata(v_md);
	v_md.clear();
    }
}



void CDirectoryReader::getFilesInsideDirectory (QDir baseDir, vector<MetaData>& v_md) {
    baseDir.setFilter(QDir::Files);
    baseDir.setNameFilters(this -> m_filters);
    QStringList tmp = baseDir.entryList();

    foreach (QString f, tmp) {
	MetaData md;
	QString filename = baseDir.absoluteFilePath(f);
	if( ID3::getMetaDataOfFile( filename, md )){
		v_md.push_back(md);
	}
    }
}
