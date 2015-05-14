/* MetaDataInfo.h */

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



#ifndef METADATAINFO_H
#define METADATAINFO_H

#include "HelperStructs/Helper.h"
#include "HelperStructs/SayonaraClass.h"
#include "CoverLookup/CoverLookup.h"
#include <QObject>
#include <QMap>

enum InfoStrings {
	Info_nTracks=0,		// set by MetaDataInfo
	Info_nAlbums,		// set by ArtistInfo, AlbumInfo
	Info_nArtists,		// set by ArtistInfo, AlbumInfo
	Info_Filesize,		// set by MetaDataInfo
	Info_PlayingTime,	// set by MetaDataInfo
	Info_Year,			// set by MetaDataInfo
	Info_Sampler,		// set by AlbumInfo
	Info_Bitrate,		// set by MetaDataInfo
	Info_Genre			// set by MetaDataInfo
};


class MetaDataInfo : public QObject, protected SayonaraClass
{
Q_OBJECT

	private:
		void set_cover_location(const MetaDataList& lst);
		void set_subheader(quint16 tracknum);
		void set_header(const MetaDataList& lst);

		QString get_info_string(InfoStrings idx);

	protected:

		QString _header;
		QString _subheader;
		QMap<InfoStrings, QString> _info;
		QStringList _paths;
		CoverLocation		_cover_location;

		QStringList _albums;
		QStringList _artists;


		QString calc_tracknum_str( quint16 tracknum );
		QString calc_artist_str();
		QString calc_album_str();

		virtual void set_cover_location();
		virtual void set_subheader();
		virtual void set_header();

		void insert_playing_time(quint64 len);
		void insert_genre(const QStringList& lst);
		void insert_filesize(quint64 filesize);

		template<typename T>
		void insert_interval(InfoStrings key, T min, T max){
			QString str;

			if(min == max){
				str = QString::number(min);
			}

			else {
				str = QString::number(min) + " - " + QString::number(max);
			}

			if(key == Info_Bitrate){
				str += " kBit/s";
			}

			_info.insert(key, str);
		}

		template<typename T>
		void insert_number(InfoStrings key, T number){
			QString str = QString::number(number);
			_info.insert(key, str);
		}

	public:

		MetaDataInfo(QObject* parent, const MetaDataList&);
		virtual ~MetaDataInfo();

		QString get_header();
		QString get_subheader();
		QMap<InfoStrings, QString> get_info();
		QString get_info_as_string();
		QStringList get_paths();
		QString get_paths_as_string();
		CoverLocation get_cover_location();

		virtual QString get_cover_artist();
		virtual QString get_cover_album();


};

class AlbumInfo : public MetaDataInfo {
Q_OBJECT
	private:
		void set_cover_location();
		void set_subheader();
		void set_header();

	public:
		AlbumInfo(QObject* parent, const MetaDataList&);
		virtual ~AlbumInfo();
};


class ArtistInfo : public MetaDataInfo {
Q_OBJECT

	private:
		void set_cover_location();
		void set_subheader();
		void set_header();

	public:
		ArtistInfo(QObject* parent, const MetaDataList&);
		virtual ~ArtistInfo();

		virtual QString get_cover_album();
};

#endif // METADATAINFO_H
