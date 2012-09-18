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


#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/Helper.h"
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

void CDirectoryReader::getFilesInsiderDirRecursive (QDir baseDir, QStringList & files, int& num_files) {

    QStringList dirs;
	baseDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dirs = baseDir.entryList();

    foreach (QString dir, dirs) {

    	baseDir.cd(dir);
        this -> getFilesInsiderDirRecursive(baseDir, files, num_files);
        baseDir.cd("..");
    }
    QStringList tmp;
    baseDir.setFilter(QDir::Files);
    baseDir.setNameFilters(this -> m_filters);
    this -> getFilesInsideDirectory (baseDir, tmp);
    num_files += tmp.size();

    // absolute paths
    files += tmp;
}

void CDirectoryReader::getFilesInsideDirectory (QDir baseDir, QStringList & files) {
    baseDir.setFilter(QDir::Files);
    baseDir.setNameFilters(this -> m_filters);
    QStringList tmp;
    tmp = baseDir.entryList();
    foreach (QString f, tmp) {
        files.push_back(baseDir.absoluteFilePath(f));
    }
}
