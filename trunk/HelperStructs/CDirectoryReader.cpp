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
#include <vector>



using namespace std;


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

bool vec_md_contains(MetaDataList& v_md, MetaData& md){

	if(v_md.size() == 0) return false;

	QString filepath1 = md.filepath.toLower().trimmed();

	for(uint j=0; j<v_md.size(); j++){
		QString filepath2 = v_md[j].filepath.toLower().trimmed();

		// equal
		if(!filepath1.compare(filepath2)){
			return true;
		}
	}

	return false;
}

void CDirectoryReader::getMetadataFromFileList(QStringList lst, MetaDataList& v_md){

	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	// read pure sound files files
	setFilter(Helper::get_soundfile_extensions());
	foreach(QString str, lst){

		if(!QFile::exists(str)) continue;
		QFileInfo fileinfo(str);

		if(fileinfo.isDir()){

			QStringList files;
			int n_files;
			getFilesInsiderDirRecursive(fileinfo.absoluteDir(), files, n_files);

			foreach(QString filename, files){
				MetaData md = db->getTrackByPath(filename);

				if(md.id < 0){
					if(!ID3::getMetaDataOfFile(filename, md)) continue;
				}

				v_md.push_back(md);

			}
		}

		else if(fileinfo.isFile() && Helper::is_soundfile(str) ){
			MetaData md = db->getTrackByPath(str);

			if(md.id < 0){
				if(!ID3::getMetaDataOfFile(str, md)) continue;
			}

			v_md.push_back(md);
		}
	}


	// TODO: look for playlists if paths could be read from database
	//extract media files out of playlist files
	setFilter(Helper::get_soundfile_extensions());
	foreach(QString str, lst){

		if(!QFile::exists(str)) continue;

		QFileInfo fileinfo(str);

		if(fileinfo.isDir()){

			// get playlists out of dirs
			QStringList files;
			int n_files;
			getFilesInsiderDirRecursive(fileinfo.absoluteDir(), files, n_files);

			// parse all playlists
			foreach(QString file, files){
				MetaDataList v_md_pl;
				PlaylistParser::parse_playlist(file, v_md_pl);

				// check, that metadata is not already available
				for(uint i=0; i<v_md_pl.size(); i++){
					MetaData md_pl = v_md_pl[i];
					if(!vec_md_contains(v_md, md_pl)){
						v_md.push_back(md_pl);
					}
				}

			} // end for each playlist file
		} // if is dir


		else if(fileinfo.isFile() && Helper::is_playlistfile(str)){

			MetaDataList v_md_pl;
			PlaylistParser::parse_playlist(str, v_md_pl);

			// check, that metadata is not already available
			for(uint i=0; i<v_md_pl.size(); i++){
				MetaData md_pl = v_md_pl[i];
				if(!vec_md_contains(v_md, md_pl)){
					v_md.push_back(md_pl);
				}
			}
		}
	}

}
