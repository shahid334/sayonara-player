/*
 * MetaData.h
 *
 *  Created on: Mar 10, 2011
 *      Author: luke
 */

#ifndef METADATA_H_
#define METADATA_H_

#include <QString>
#include <QStringList>
#include <iostream>

using namespace std;

struct MetaData{

	QString title;
	QString artist;
	QString album;
	qint32 rating;
	qint64 length_ms;
	qint32 year;
	QString filepath;
	qint32 track_num;


    inline MetaData () {
            title = "";
            artist = "";
            album = "";
            rating = 0;
            length_ms = 0;
            year = 0;
            filepath = "";
            track_num = 0;
        }

	void print(){

		cout << title.toStdString()
				<< " by " << artist.toStdString()
				<< " from " << album.toStdString()
                << " (" << length_ms << " m_sec) :: " << filepath.toStdString() << endl;
	}

	QStringList toStringList(){

		QStringList list;
		list.push_back(title);
		list.push_back(artist);
		list.push_back(album);
		list.push_back(QString::number(rating));
		list.push_back(QString::number(length_ms));
		list.push_back(QString::number(year));
		list.push_back(filepath);
		list.push_back(QString::number(track_num));
		return list;
	}

	void fromStringList(QStringList list){

		if(list.size() < 8) return;

		title = list.at(0);
		artist = list.at(1);
		album = list.at(2);
		rating = list.at(3).toInt();
		length_ms = list.at(4).toULongLong();
		year = list.at(5).toInt();
		filepath = list.at(6);
		track_num = list.at(7).toInt();


	}



};

struct Artist{
	QString name;
	qint32	id;
	qint32	num_songs;
	qint32  num_albums;

};


struct Album{
	QString name;
	qint32	id;
	qint32 	num_songs;
	qint32 	length_sec;
	qint32	year;
	QStringList artists;
};


#endif /* METADATA_H_ */
