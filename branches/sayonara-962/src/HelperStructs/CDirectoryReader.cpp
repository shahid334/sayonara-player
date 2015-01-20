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
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/PlaylistParser.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QDir>
#include <QFileInfo>



CDirectoryReader::CDirectoryReader () {

	m_filters = Helper::get_soundfile_extensions();

}

CDirectoryReader::~CDirectoryReader () {

}

void CDirectoryReader::set_filter (const QStringList & filter) {

	m_filters = filter;
}

void CDirectoryReader::set_filter(const QString& filter){
	m_filters.clear();
	m_filters << filter;
}

void CDirectoryReader::get_files_in_dir_rec( QDir base_dir, QStringList& files){

    QStringList dirs;
	base_dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

	dirs = base_dir.entryList();

	for(const QString& dir : dirs) {

		base_dir.cd(dir);
		get_files_in_dir_rec(base_dir, files);
		base_dir.cd("..");
    }

	files << get_files_in_dir(base_dir);
}

QStringList CDirectoryReader::get_files_in_dir (QDir base_dir) {

	QStringList entries;
	QStringList files;

	base_dir.setFilter(QDir::Files);
	base_dir.setNameFilters( m_filters );

	entries = base_dir.entryList();

	for(const QString& file : entries) {
		files << base_dir.absoluteFilePath(file);
    }

	return files;
}

MetaDataList CDirectoryReader::get_md_from_filelist(const QStringList& lst) {

	MetaDataList v_md;
	QStringList files;
    CDatabaseConnector* db = CDatabaseConnector::getInstance();

	// fetch sound and playlist files
    QStringList filter = Helper::get_soundfile_extensions();
    filter.append(Helper::get_playlistfile_extensions());

	set_filter(filter);

	for( const QString& str : lst) {

        if(!QFile::exists(str)) continue;

        if(Helper::is_dir(str)) {

            QDir dir(str);
            dir.cd(str);

			get_files_in_dir_rec(dir, files);
        }

        else if(Helper::is_file(str)) {
            files.push_back(str);
        }
    }


    QStringList playlist_paths;

	db->getMultipleTracksByPath(files, v_md);

	for(MetaDataList::iterator it=v_md.begin(); it != v_md.end(); it++) {

		QString filepath = QDir(it->filepath).absolutePath();

        if(Helper::is_playlistfile(filepath)) {
            playlist_paths.push_back(filepath);
			it = v_md.erase(it);
            continue;
        }

        if(Helper::is_soundfile(filepath)) {

			qDebug() << it->filepath << " is soundfile " << it->filepath;
			if( it->id < 0 ) {

				if(!ID3::getMetaDataOfFile(*it)) {
					it = v_md.erase(it);
                    continue;
                }

				it->is_extern = true;
            }
        }
    }


    // TODO: look for playlists if paths could be read from database
	//extract media files out of playlist files
	for(const QString& path : playlist_paths) {

        qDebug() << "parse playlist file " << path;

        MetaDataList v_md_pl;
        PlaylistParser::parse_playlist(path, v_md_pl);

        // check, that metadata is not already available
		for(const MetaData& md_pl : v_md_pl) {

            if(!v_md.contains(md_pl)) {
                //qDebug() << md_pl.filepath << " not in vector";
                v_md.push_back(md_pl);
            }
        }
	}

	return v_md;
}
