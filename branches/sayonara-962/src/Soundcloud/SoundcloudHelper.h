/* SoundcloudHelper.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef SHOUTCASTHELPER_H
#define SHOUTCASTHELPER_H

#include "HelperStructs/Helper.h"

struct JsonItem {
	enum Type {
		TypeArray=0,
		TypeNumber,
		TypeString,
		TypeBlock
	} type;

	QList<JsonItem> values;
	QString key;
	QString pure_value;

	void print(const QString& parent_key="") const {


		QString my_key = parent_key + "." + key;
		int i=0;
		for(const JsonItem& item : values){
			item.print(my_key);
			i++;
		}

		if(values.size() == 0){
			qDebug() << my_key << " = " << pure_value;
		}
	}

};


namespace SoundcloudHelper
{

	ArtistList			search_artist(const QString& name);
	bool				get_all_playlists(qint32 artist_id, MetaDataList& v_md, AlbumList& albums);

	QString				create_dl_get_artist(QString name);
	QString				create_dl_get_playlists(qint64 artist_id);
	QString				create_dl_get_tracks(qint64 artist_id);

	namespace Parser {


		JsonItem			parse(QString key, const QString& content);

		JsonItem			parse_array(QString key, QString content);
		JsonItem			parse_block(QString key, QString content);
		JsonItem			parse_standard(QString key, QString content);

		int					find_block_end(const QString& content, int start_at=0);
		int					find_array_end(const QString& content, int start_at=0);
		int					find_value_end(const QString& content, int start_at=0);

		bool				extract_track(const JsonItem& item, MetaData& md);
		bool				extract_artist(const JsonItem& item, Artist& artist);
		bool				extract_playlist(const JsonItem& item, Album& album, MetaDataList& v_md);
	}

}

#endif // SHOUTCASTHELPER_H
