/* Artist.h */

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



#ifndef _ARTIST_H_
#define _ARTIST_H_

#include "HelperStructs/LibraryItem.h"

#include <QString>
#include <QVariant>
#include <QMetaType>


class Artist : public LibraryItem {

public:

    qint32 id;                                                                                                                                       
    QString name;                                                                                                                                                      
    quint16 num_songs;                                                                                                           
	quint16 num_albums;


	Artist();
	virtual	~Artist();

	static bool fromVariant(const QVariant& v, Artist& a);
	static QVariant toVariant(const Artist& a);
	void print() const ;
};


Q_DECLARE_METATYPE(Artist)

typedef struct QVector<Artist> ArtistList;

#endif
