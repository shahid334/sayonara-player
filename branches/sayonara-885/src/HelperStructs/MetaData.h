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

#include "HelperStructs/globals.h"
#include "HelperStructs/LibraryItem.h"
#include "HelperStructs/Artist.h"
#include "HelperStructs/Album.h"

#include <QString>
#include <QStringList>
#include <QPair>
#include <QVariant>
#include <QMetaType>

#include <vector>

using namespace std;

enum RadioMode {

	RadioModeOff = 0,
	RadioModeStation = 1,
};

class MetaData;
class LastTrack;

Q_DECLARE_METATYPE(MetaData)
Q_DECLARE_METATYPE(LastTrack)

class MetaData : public LibraryItem {

public:
	qint32 id;
	qint32 album_id;
	qint32 artist_id;
    QString title;
    QString artist;
    QString album;
    QStringList genres;
    quint8 rating;
    quint64 length_ms;
    quint16 year;
    QString filepath;
    quint16 track_num;
    quint32 bitrate;
    quint64 filesize;
    QString comment;
    quint8 discnumber;
    quint8 n_discs;


    bool is_extern;
    RadioMode radio_mode;

    bool pl_selected;
    bool pl_playing;
    bool pl_dragged;

    bool is_disabled;

	MetaData ();
    MetaData (const MetaData& );
	virtual ~MetaData();

    bool operator==(const MetaData& md) const;
    bool is_equal(const MetaData& md) const;
    
	void print() const;

    static QVariant toVariant(const MetaData& md);
    static bool fromVariant(const QVariant& v, MetaData& md);
};



class MetaDataList : public QVector<MetaData> {

    private:
        int _cur_played_track;

	public:

		MetaDataList();
		MetaDataList(int n_elems);

	    virtual ~MetaDataList();
        void setCurPlayTrack(int idx);

        virtual bool contains(const MetaData& md) const;

        int findTrack(int id) const;
        int findTrack(const QString&) const;

        QStringList toStringList() const;
};

struct CustomPlaylist{

    QString name;
    qint32 id;
    MetaDataList tracks;
    quint32 length;
    quint32 num_tracks;
    bool is_valid;

	CustomPlaylist() {
        is_valid = false;
        id = -1;
        name = "";
        length = 0;
        num_tracks = 0;
    }
};

class LastTrack : public MetaData {

public:

    quint32 pos_sec;

    LastTrack();
    LastTrack(const MetaData& md);
    LastTrack(const LastTrack& lt);
    virtual ~LastTrack();

    static QVariant toVariant(const LastTrack& md);
    static bool fromVariant(const QVariant& v, LastTrack& md);
};

#endif /* METADATA_H_ */
