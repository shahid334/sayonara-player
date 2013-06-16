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

#include "HelperStructs/MetaData.h"

#include <QString>
#include <QList>
#include <QWidget>

namespace Helper{

    QString cvtMsecs2TitleLengthString(long int msec, bool colon=true, bool show_days=true);
	QString cvtQString2FirstUpper(QString str);
	QString calc_filesize_str(qint64 filesize);

	QString getIconPath();
	QString getSayonaraPath();
    QString getSharePath();
    QString createLink(QString name, QString target="", bool underline=true);

    QString get_artist_image_path(QString artist, QString extension="jpg");
    QString get_cover_path(QString artist, QString album, QString extension="jpg");
    QString get_cover_path(int album_id);
    QString calc_cover_token(QString artist, QString album);

    QString calc_google_image_search_adress(QString searchstring);
	QString calc_google_artist_adress(QString artist);
	QString calc_google_album_adress(QString artist, QString album);

	QString calc_lfm_artist_adress(QString artist);
	QString calc_lfm_album_adress(QString artist, QString album);

	QStringList get_soundfile_extensions();
	QStringList get_playlistfile_extensions();
    QStringList get_podcast_extensions();

    QString calc_file_extension(QString filename);
    void remove_files_in_directory(QString dir_name, QStringList filters);
    QString get_parent_folder(QString path);
    QString get_filename_of_path(QString path);
    void split_filename(QString src, QString& path, QString& filename);
    QStringList extract_folders_of_files(QStringList list);

    QString split_string_to_widget(QString str, QWidget* w, QChar sep=' ');
    bool is_url(QString str);
    bool is_www(QString str);
    bool is_file(QString filename);
    bool is_dir(QString filename);
	bool is_soundfile(QString filename);
	bool is_playlistfile(QString filename);
    bool is_podcastfile(QString filename, QString* content);

    QString get_album_w_disc(const MetaData& md);

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


    bool read_file_into_str(QString filename, QString* content);
    bool read_http_into_str(QString url, QString* content);


	QString easy_tag_finder(QString tag, QString& xml_doc);
	QString calc_hash(QString data);
	QString get_newest_version();
    QString get_album_major_artist(int album_id);
    Album get_album_from_metadata(const MetaData& md);


    void set_deja_vu_font(QWidget* w);


};


#endif
