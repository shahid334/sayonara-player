/* Helper.h */

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
 * Helper.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#ifndef _HELPER_H
#define _HELPER_H

#include <HelperStructs/MetaData.h>

#include <string>
#include <sstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <QString>
#include <QList>
#include <ctime>

using namespace std;



namespace Helper{

    QString cvtMsecs2TitleLengthString(long int msec, bool colon=true, bool show_days=true);
	QString cvtQString2FirstUpper(QString str);
	QString calc_filesize_str(qint64 filesize);

	QString getIconPath();
	QString getSayonaraPath();
    QString getSharePath();

    QString get_artist_image_path(QString artist, QString extension="jpg");
    QString get_cover_path(QString artist, QString album, QString extension="jpg");
    QString calc_cover_token(QString artist, QString album);

	QString calc_google_image_search_adress(QString searchstring,  QString size, QString filetype);
	QString calc_google_artist_adress(QString artist);
	QString calc_google_album_adress(QString artist, QString album);

	QString calc_lfm_artist_adress(QString artist);
	QString calc_lfm_album_adress(QString artist, QString album);

	QStringList get_soundfile_extensions();
	QStringList get_playlistfile_extensions();
    QString calc_file_extension(QString filename);

	bool is_soundfile(QString filename);
	bool is_playlistfile(QString filename);

	template <typename T> QList<T> randomize_list(const QList<T>& list){
		srand ( time(NULL) );

		QList<T> list_copy = list;
		for(int i=0; i<list.size(); i++){

			list_copy.swap(i, rand() % list.size());
		}

		return list_copy;
	}



	/* Checks if track exists
	 * if track does not exist
	 * it is deleted from database
	 * For online tracks it always returns true
	 * ATTENTION:
	 * Wrap this method, because for sure library has to be notified
	 */
	bool checkTrack(const MetaData& md);


	bool read_file_into_str(QString filename, QString& content);
	bool read_http_into_str(QString url, QString& content);


	QString easy_tag_finder(QString tag, QString& xml_doc);
	QString calc_hash(QString data);
};


#endif
