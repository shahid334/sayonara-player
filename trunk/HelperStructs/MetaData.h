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
#include <iostream>

using namespace std;



struct MetaData{

	qint32 id;
	qint32 album_id;
	qint32 artist_id;
	QString title;
	QString artist;
	QString album;
	qint32 rating;
	qint64 length_ms;
	qint32 year;
	QString filepath;
	qint32 track_num;
	qint32 bitrate;
	bool is_extern;



    inline MetaData () {
            id = 0;
            artist_id = 0;
            album_id = 0;
			title = "";
            artist = "";
            album = "";
            rating = 0;
            length_ms = 0;
            year = 0;
            filepath = "";
            track_num = 0;
            bitrate = 0;
            is_extern = false;
        }

	void print(){

		cout << title.toStdString()
				<< " by " << artist.toStdString()
				<< " from " << album.toStdString()
                << " (" << length_ms << " m_sec) :: " << filepath.toStdString() << endl;
	}

	QStringList toStringList(){

		QStringList list;
		QString tmpTitle = title;
		QString tmpArtist = artist;
		QString tmpAlbum = album;
		if(title.trimmed().size() == 0) tmpTitle = QString("(Unknown title)");
		if(artist.trimmed().size() == 0) tmpArtist = QString("(Unknown artist)");
		if(album.trimmed().size() == 0) tmpAlbum = QString("(Unknown album)");

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
		return list;
	}

	void fromStringList(QStringList& list){

		if(list.size() < 8) return;

		title = list.at(0);
		artist = list.at(1);
		album = list.at(2);
		rating = list.at(3).toInt();
		length_ms = list.at(4).toULongLong();
		year = list.at(5).toInt();
		filepath = list.at(6);
		track_num = list.at(7).toInt();
		bitrate = list.at(8).toInt();
		id = list.at(9).toInt();
		album_id = list.at(10).toInt();
		artist_id = list.at(11).toInt();
		is_extern = ( list.at(12) == "1" );


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


#endif /* METADATA_H_ */
