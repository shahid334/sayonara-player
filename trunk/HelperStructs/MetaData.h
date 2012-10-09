/* MetaData.h */

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
 * MetaData.h
 *
 *  Created on: Mar 10, 2011
 *      Author: luke
 */

#ifndef METADATA_H_
#define METADATA_H_

#define DROP_TYPE_ARTISTS 0
#define DROP_TYPE_ALBUMS 1
#define DROP_TYPE_TRACKS 2

#include <QString>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QVariant>
#include <vector>

#include "HelperStructs/globals.h"

using namespace std;

struct MetaData {



public:
	qint32 id;
	qint32 album_id;
	qint32 artist_id;
	QString title;
	QString artist;
	QString album;
	//QString genre;
	qint32 rating;
	qint64 length_ms;
	qint32 year;
	QString filepath;
	qint32 track_num;
	qint32 bitrate;
	bool is_extern;
	int radio_mode;

	bool pl_selected;
	bool pl_playing;
	bool pl_dragged;


    inline MetaData () {
            id = -1;
            artist_id = -1;
            album_id = -1;
			title = "";
            artist = "";
            album = "";
            //genre = "";
            rating = 0;
            length_ms = 0;
            year = 0;
            filepath = "";
            track_num = 0;
            bitrate = 0;
            is_extern = false;
            radio_mode = RADIO_OFF;

            pl_selected = false;
            pl_playing = false;
            pl_dragged = false;
        }

	void print(){

		qDebug() << title
				<< " by " << artist
				<< " from " << album
                << " (" << length_ms << " m_sec) :: " << filepath;
	}

	QVariant toVariant() const{

		QStringList list;

		QString tmpTitle = title;
		QString tmpArtist = artist;
		QString tmpAlbum = album;

		if(title.trimmed().size() == 0) tmpTitle = QString("(Unknown title)");
		if(artist.trimmed().size() == 0) tmpArtist = QString("");
		if(album.trimmed().size() == 0) tmpAlbum = QString("");

		list.push_back(tmpTitle);
		list.push_back(tmpArtist);
		list.push_back(tmpAlbum);
		list.push_back(QString::number(rating));
		list.push_back(QString::number(length_ms));
		list.push_back(QString::number(year));
		list.push_back(filepath);
		list.push_back(QString::number(track_num));
		list.push_back(QString::number(bitrate));
		list.push_back(QString::number(id));
		list.push_back(QString::number(album_id));
		list.push_back(QString::number(artist_id));
		list.push_back(QString::number(   (is_extern) ? 1 : 0  ));
		list.push_back( QString::number(radio_mode) );
		list.push_back( (pl_playing) ? "1" : "0" );
		list.push_back( (pl_selected) ? "1" : "0" );
		list.push_back( (pl_dragged) ? "1" : "0" );

		return list;
	}


	static bool fromVariant(QVariant v, MetaData& md){

		QStringList list = v.toStringList();

		if(list.size() < 17) return false;

		md.title = list[0];
		md.artist = list[1];
		md.album = list[2];
		md.rating = list[3].toInt();
		md.length_ms = list[4].toULongLong();
		md.year = list[5].toInt();
		md.filepath = list[6];
		md.track_num = list[7].toInt();
		md.bitrate = list[8].toInt();
		md.id = list[9].toInt();
		md.album_id = list[10].toInt();
		md.artist_id = list[11].toInt();
		md.is_extern = ( list[12] == "1" );
		md.radio_mode = list[13].toInt();

		md.pl_playing = (list[14] == "1");
		md.pl_selected = (list[15] == "1");
		md.pl_dragged = (list[16] == "1");

		return true;
	}


};

class MetaDataList : public vector<MetaData>{

public:

	bool contains(const MetaData& md, bool cs=false){
		QString filepath;

		if(cs){
			filepath = md.filepath.trimmed();

			for(uint i=0; i<size(); i++){
				MetaData md =  vector<MetaData>::at(i);
				QString filepath2 = md.filepath.trimmed();
				if(!filepath.compare(filepath2)) return true;

			}
		}

		else{

			filepath = md.filepath.toLower().trimmed();

			for(uint i=0; i<size(); i++){
				MetaData md =  vector<MetaData>::at(i);
				QString filepath2 = md.filepath.toLower().trimmed();
				if(!filepath.compare(filepath2)) return true;

			}
		}

		return false;
	}

	void insert_mid(const MetaData& md, int pos){
		if(pos >= (int) size()){
			push_back(md);
			return;
		}

		if(pos < 0) pos = 0;

		MetaData md_cp = md;
		MetaDataList::iterator it = begin();
		insert(it + pos, md_cp);
	}
};

struct Artist{
	QString name;
	qint32	id;
	qint32	num_songs;
	qint32  num_albums;


	Artist(){
		id = -1;
		name = "";
		num_songs = 0;
		num_albums = 0;
	}

	QStringList toStringList(){
		QStringList list;
		QString tmpName = name;
		if(name.trimmed().size() == 0) tmpName = QString("(Unknown artist)");
		list.push_back(tmpName);
		list.push_back(QString::number(id));
		list.push_back(QString::number(num_songs));
		list.push_back(QString::number(num_albums));
		return list;
	}

	void fromStringList(QStringList& list){
		if(list.size() < 4) return;
		name = list.at(0);
		id = list.at(1).toInt();
		num_songs = list.at(2).toInt();
		num_albums = list.at(3).toInt();
	}

};


struct Album{
	QString name;
	qint32	id;
	qint32 	num_songs;
	qint32 	length_sec;
	qint32	year;
	QStringList artists;
	bool is_sampler;

	Album(){
		name = "";
		id = -1;
		num_songs = 0;
		length_sec = 0;
		year = 0;
		is_sampler = false;
	}

	QStringList toStringList(){
		QStringList list;
		QString tmpName = "(Unknown album)";
		if(name.trimmed().size() > 0) tmpName = name;

		list.push_back(tmpName);
		if(artists.size() > 0){
			QString tmp_artists = artists.at(0);
			for(int i=1; i<artists.size(); i++){
				tmp_artists += "," + artists[i];
			}
			list.push_back(tmp_artists);
		}
		else list.push_back("");

		list.push_back(QString::number(id));
		list.push_back(QString::number(num_songs));
		list.push_back(QString::number(length_sec));
		list.push_back(QString::number(year));
		if(is_sampler){
			list.push_back("sampler");
		}
		else{
			list.push_back("no_sampler");
		}
		return list;


	}

	void fromStringList(QStringList& list){

		if(list.size() < 7) return;
		name = list.at(0);
		QString tmp_artists = list.at(1);
		QStringList tmp_list = tmp_artists.split(',');
		artists = tmp_list;
		id = list.at(2).toInt();
		num_songs = list.at(3).toInt();
		length_sec = list.at(4).toLong();
		year = list.at(5).toInt();
		is_sampler = (list.at(6) == "sampler");

	}
};


struct CustomPlaylist{
	QString name;
	qint32 id;
	MetaDataList tracks;
	qint32 length;
	qint32 num_tracks;
};


#endif /* METADATA_H_ */
