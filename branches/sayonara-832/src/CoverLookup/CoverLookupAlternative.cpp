/* CoverLookupAlternative.cpp */

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



#include "CoverLookup/CoverLookupAlternative.h"

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, int n_covers) :
    CoverLookupInterface(parent)
{
    _n_covers = n_covers;
    _cl = NULL;
}

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const QString& album_name, const QString& artist_name, int n_covers) :
    CoverLookupAlternative(parent, n_covers)
{
    _album_name = album_name;
    _artist_name = artist_name;
    _search_type = ST_Standard;
}

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const QString& album_name, const QStringList& artists_name, int n_covers) :
    CoverLookupAlternative(parent, n_covers)
{
    _artists_name = artists_name;
    _search_type = ST_Sampler;
}

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const Album& album, int n_covers) :
    CoverLookupAlternative(parent, n_covers)
{
    _album = album;
    _search_type = ST_ByAlbum;
}

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, int album_id, int n_covers) :
    CoverLookupAlternative(parent, n_covers)
{
    _album_id = album_id;
    _search_type = ST_ByID;
}


CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const QString& artist_name, int n_covers)  :
    CoverLookupAlternative(parent, n_covers)
{
    _artist_name = artist_name;
    _search_type = ST_ByArtistName;
}


CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const Artist& artist, int n_covers) :
    CoverLookupAlternative(parent, n_covers)
{
    _artist = artist;
    _search_type = ST_ByArtist;
}


CoverLookupAlternative::~CoverLookupAlternative() {

    if(!_cl) return;

    _cl->stop();
    qDebug() << "Deleted cover alternative";
}



void CoverLookupAlternative::stop() {

    if(!_cl) return;

    _cl->stop();
}


void CoverLookupAlternative::start() {

    _run = true;
    _cl = new CoverLookup(this, _n_covers);
    connect(_cl, SIGNAL(sig_cover_found(QString)), this, SLOT(cover_found(QString)));
    connect(_cl, SIGNAL(sig_finished(bool)), this, SLOT(finished(bool)));


    switch(_search_type) {

        case ST_Standard:
            _cl->fetch_album_cover_standard(_artist_name, _album_name);
            break;

        case ST_ByID:
            _cl->fetch_album_cover_by_id(_album_id);
            break;

        case ST_Sampler:
            _cl->fetch_album_cover_sampler(_artists_name, _album_name);
            break;

        case ST_ByAlbum:
            _cl->fetch_album_cover(_album);
            break;

        case ST_ByArtistName:
            _cl->fetch_artist_cover_standard(_artist_name);

            break;

        case ST_ByArtist:
            _cl->fetch_artist_cover(_artist);
            break;

        default:
            break;
    }
}


void CoverLookupAlternative::cover_found(QString cover_path) {
    emit sig_cover_found(cover_path);
}

void CoverLookupAlternative::finished(bool success) {

    emit sig_finished(success);

    if(!_cl) return;

    delete _cl; _cl = 0;
}


