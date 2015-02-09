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
#include <QVector>

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

typedef qint32 ArtistID;
typedef qint32 AlbumID;
typedef qint32 TrackID;

typedef int ArtistCacheIdx;
typedef int AlbumCacheIdx;
typedef int TrackCacheIdx;

class SoundcloudParser
{

private:

	QString				create_dl_get_artist(QString name);
	QString				create_dl_get_playlists(qint64 artist_id);
	QString				create_dl_get_tracks(qint64 artist_id);

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




	ArtistList						_artist_cache;
	AlbumList						_album_cache;
	MetaDataList					_track_cache;

	QMap<ArtistID, bool>			_artist_available;
	QMap<ArtistID, ArtistCacheIdx>	_artist_idx_mapping;
	QMap<QString, ArtistCacheIdx>	_artist_name_mapping;

	QMap<AlbumID, bool>				_album_available;
	QMap<AlbumID, AlbumCacheIdx>	_album_idx_mapping;

	QMap<TrackID, bool>				_track_available;
	QMap<TrackID, TrackCacheIdx>	_track_idx_mapping;

	QMap<ArtistID, QList<AlbumCacheIdx>>	_artist_album_mapping;
	QMap<ArtistID, QList<TrackCacheIdx>>	_artist_track_mapping;
	QMap<AlbumID,  QList<TrackCacheIdx>>	_album_track_mapping;

	bool	create_artist_cache(const QString& name);
	bool	create_artist_cache(ArtistID artist_id);



public:

	SoundcloudParser();


	virtual ~SoundcloudParser();

	ArtistList	search_artist(const QString& name);

	ArtistList get_all_artists();
	MetaDataList get_all_tracks();
	AlbumList get_all_albums();

	MetaDataList get_all_tracks_by_artist(const QString& artist);
	AlbumList get_all_albums_by_artist(const QString& artist);

	MetaDataList get_all_tracks_by_artist(ArtistID artist_id);
	AlbumList get_all_albums_by_artist(ArtistID artist_id);

	MetaDataList get_all_tracks_by_album(AlbumID album_id);
};

#endif // SHOUTCASTHELPER_H
