/* CoverLookupAlternative.h */

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



#ifndef COVERLOOKUPALTERNATIVE_H
#define COVERLOOKUPALTERNATIVE_H

#include "CoverLookup/CoverLookup.h"

class CoverLookupAlternative : public CoverLookupInterface
{

    Q_OBJECT

private:

    CoverLookupAlternative(QObject* parent, int n_covers);

    CoverLookup*        _cl;
    QString             _album_name;
    QString             _artist_name;
    QStringList         _artists_name;
    int                 _album_id;
    Album               _album;
    Artist              _artist;

    int					_n_covers;
    bool				_run;


    enum SearchType {

        ST_Standard=0,
        ST_Sampler=1,
        ST_ByID=2,
        ST_ByAlbum=3,
        ST_ByArtistName = 4,
        ST_ByArtist = 5
    } _search_type;


public:

    CoverLookupAlternative(QObject* parent, const QString& album_name, const QString& artist_name, int n_covers);
    CoverLookupAlternative(QObject* parent, const QString& album_name, const QStringList& artists_name, int n_covers);
    CoverLookupAlternative(QObject* parent, const Album& album, int n_covers);
    CoverLookupAlternative(QObject* parent, int album_id, int n_covers);
    CoverLookupAlternative(QObject* parent, const QString& artist, int n_covers);
    CoverLookupAlternative(QObject* parent, const Artist& artist, int n_covers);

    ~CoverLookupAlternative();

    virtual void stop();
    void start();


private slots:
	void cover_found(const CoverLocation& );
    void finished(bool);
};


#endif // COVERLOOKUPALTERNATIVE_H
