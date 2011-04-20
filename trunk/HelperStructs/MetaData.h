/*
 * MetaData.h
 *
 *  Created on: Mar 10, 2011
 *      Author: luke
 */

#ifndef METADATA_H_
#define METADATA_H_

#include <QString>
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


    inline MetaData () {
            title = "";
            artist = "";
            album = "";
            rating = 0;
            length_ms = 0;
            year = 0;
            filepath = "";
        }

	void print(){

		cout << title.toStdString()
				<< " by " << artist.toStdString()
				<< " from " << album.toStdString()
                << " (" << length_ms << " m_sec) :: " << filepath.toStdString() << endl;
	};

};




#endif /* METADATA_H_ */
