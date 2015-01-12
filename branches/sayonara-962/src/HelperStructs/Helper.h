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

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QIcon>
#include <QThread>

namespace Helper{

	QString cvt_ms_to_string(quint64 msec, bool empty_zero = false, bool colon=true, bool show_days=true);
	QString cvtQString2FirstUpper(const QString& str);
	QString calc_filesize_str(quint64 filesize);

    QIcon getIcon(const QString& icon_name);
    QPixmap getPixmap(const QString& icon_name, QSize sz=QSize(0, 0), bool keep_aspect=true);
	QString getErrorFile();
	QString getSayonaraPath();
    QString getSharePath();
    QString getLibPath();
	QString createLink(const QString& name,
					   const QString& target="",
					   bool underline=true);

	QString calc_cover_token(const QString& artist, const QString& album);

	QString calc_google_image_search_adress(const QString& searchstring);
	QString calc_google_artist_adress(const QString& artist);
	QString calc_google_album_adress(const QString& artist, const QString& album);

	QString calc_lfm_artist_adress(const QString& artist);
	QString calc_lfm_album_adress(const QString& artist, const QString& album);

	QStringList get_soundfile_extensions();
	QStringList get_playlistfile_extensions();
    QStringList get_podcast_extensions();

	QString calc_file_extension(const QString& filename);
	void remove_files_in_directory(const QString& dir_name, const QStringList& filters);
	QString get_parent_folder(const QString& path);
	QString get_filename_of_path(const QString& path);
	void split_filename(const QString& src, QString& path, QString& filename);
	QStringList extract_folders_of_files(const QStringList& list);


    // Everything clear
	bool is_url(const QString& str);
	bool is_www(const QString& str);
	bool is_file(const QString& filename);
	bool is_dir(const QString& filename);
	bool is_soundfile(const QString& filename);
	bool is_playlistfile(const QString& filename);
	bool is_podcastfile(const QString& filename, QString* content);


    // get album name with discnumber (calls database)
    QString get_album_w_disc(const MetaData& md);

	template <typename T> QList<T> randomize_list(const QList<T>& list) {
		srand ( time(NULL) );

		QList<T> list_copy = list;
		for(int i=0; i<list.size(); i++) {

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

	bool read_file_into_str(const QString& filename, QString* content);
	bool read_http_into_str(const QString& url, QString* content);
	bool read_http_into_img(const QString& url, QImage* image);
	bool read_file_into_byte_arr(const QString& filename, QByteArray& content);

	QString easy_tag_finder(const QString&  tag, const QString& xml_doc);
	QString calc_hash(const QString&  data);
	QString get_newest_version();
    QString get_album_major_artist(int album_id);
	QString get_major_artist(const QStringList& artists);
	QString get_major_artist(const ArtistList& artists);

	QString get_location_from_ip(const QString&);

	void set_bin_path(const QString& str);


	class _Sleeper : public QThread{

		public:
			static void sleep_ms(unsigned long ms){
				msleep(ms);
			}

	};

	void sleep_ms(unsigned long ms);

}


#endif
