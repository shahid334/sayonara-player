/* Album.h */

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



#ifndef _ALBUM_H_
#define _ALBUM_H_

#include "HelperStructs/LibraryItem.h"

#include <QStringList>
#include <QVariant>
#include <QMetaType>

class Album;

Q_DECLARE_METATYPE(Album)

class Album : public LibraryItem {

public:
    QString name;
	qint32	id;
    quint16 num_songs;
    quint32 length_sec;
    quint16	year;
    QStringList artists;
    QList<quint8> discnumbers;
    quint8 n_discs;
    quint8 rating;

    bool is_sampler;

    Album();
	Album(const Album& album);
    ~Album();

	static QVariant toVariant(const Album& album);
	static bool fromVariant(const QVariant& v, Album& album);
	void print() const;

};


class AlbumList : public QVector<Album> {

private:
	void push_back(const Album &t);



};

#endif


