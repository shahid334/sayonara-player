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

#include "HelperStructs/MetaData.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/id3.h"
#include "HelperStructs/PlaylistParser.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStringList>


CDirectoryReader::CDirectoryReader () {

	this->m_filters = Helper::get_soundfile_extensions();

}
CDirectoryReader::~CDirectoryReader () {

}


void CDirectoryReader::setFilter (const QStringList & filter) {

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

    //qDebug() << "Get files inside " << baseDir.path();
    baseDir.setFilter(QDir::Files);
    baseDir.setNameFilters(this -> m_filters);
    QStringList tmp;

    tmp = baseDir.entryList();
    foreach (QString f, tmp) {
       // qDebug() << "found file in " << baseDir.path();
        files.push_back(baseDir.absoluteFilePath(f));
    }
}


void CDirectoryReader::getMetadataFromFileList(QStringList lst, MetaDataList& v_md){


    CDatabaseConnector* db = CDatabaseConnector::getInstance();

    QStringList files;


    // fetch sound and playlist files
    QStringList filter = Helper::get_soundfile_extensions();
    filter.append(Helper::get_playlistfile_extensions());
    setFilter(filter);

    foreach(QString str, lst){
        if(!QFile::exists(str)) continue;

        QFileInfo fileinfo(str);
        if(fileinfo.isDir()){

            int n_files;

            QDir dir(str);
            dir.cd(str);

            getFilesInsiderDirRecursive(dir, files, n_files);
        }

        else if(fileinfo.isFile()){
            files.push_back(str);
        }
    }


    // this has to be so strange,
    MetaDataList v_possible_md;
    QStringList playlist_paths;

    db->getMultipleTracksByPath(files, v_possible_md);

    foreach(MetaData md, v_possible_md){

        if(Helper::is_playlistfile(md.filepath)){
            playlist_paths.push_back(md.filepath);
            continue;
        }

        if(Helper::is_soundfile(md.filepath)){

            //qDebug() << md.filepath << " is soundfile " << md.id;
            if(md.id < 0){
                if(!ID3::getMetaDataOfFile(md)) continue;
            }
            //qDebug() << "push back " << md.filepath;
            v_md.push_back(md);
        }
    }


    // TODO: look for playlists if paths could be read from database
	//extract media files out of playlist files
    foreach(QString path, playlist_paths){

        qDebug() << "parse playlist file " << path;

        MetaDataList v_md_pl;
        PlaylistParser::parse_playlist(path, v_md_pl);

        // check, that metadata is not already available
        foreach(MetaData md_pl, v_md_pl){

            if(!v_md.contains(md_pl)){
                //qDebug() << md_pl.filepath << " not in vector";
                v_md.push_back(md_pl);
            }
        }
	}





}
