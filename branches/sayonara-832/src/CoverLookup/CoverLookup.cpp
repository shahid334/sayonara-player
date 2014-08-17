/* CoverLookup.cpp */

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
 * CoverLookup.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#include "CoverLookup/CoverLookup.h"
#include "CoverLookup/CoverFetchThread.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"


#include <QFile>
#include <QDebug>
#include <QImage>

using namespace std;


CoverLookup::CoverLookup(QObject* parent, int n_covers) :
    QObject(parent),
    _n_covers(n_covers),
    _thread(0)
{
    _db = CDatabaseConnector::getInstance();
}

CoverLookup::~CoverLookup() {

    if(_cft){
        _cft->stop();
    }
}


void CoverLookup::start_new_thread(const QString& url, const QString& target_name ){

    _cft = new CoverFetchThread(this, url, target_name, _n_covers);

    connect(_cft, SIGNAL(sig_finished(bool)), this, SLOT(thread_finished(bool)));
    connect(_cft, SIGNAL(sig_cover_found(QString)), this, SLOT(cover_found(QString)));

    _cft->start();
}

void CoverLookup::stop(){

    if(!_cft) return;

    _cft->stop();

}


bool CoverLookup::fetch_album_cover_standard(const QString& artist, const QString& album_name){

    QString url;
    QString target_name;

    target_name = Helper::get_cover_path(artist, tmp_album.name);

    if( QFile::exists(target_name) ) {
        emit sig_cover_found(target_name);
    }

    url = Helper::calc_google_album_adress(artist, album_name);

    start_new_thread(url, target_name);

    return true;
}


bool CoverLookup::fetch_album_cover_sampler(const QStringList& artists, const QString& album_name){

    QString major_artist;

    major_artist = Helper::get_major_artist(artists);
    return fetch_single_album_cover(major_artist, album_name);
}


bool CoverLookup::fetch_album_cover_by_id(const int album_id){

    if(album_id < 0) return false;

    Album album;

    bool success = _db->getAlbumByID(album_id, album);
    if(!success) return false;

    return fetch_album_cover(album);
}


bool CoverLookup::fetch_album_cover(const Album& album){

    int n_artists;

    n_artists = album.artists.size();

    if( n_artists > 0 ){
        return fetch_sampler_cover_album(album.artists, album.name);
    }

    else if( n_artists == 1 ){
        return fetch_single_album_cover(album.artists, album.name);
    }

    else {
        return fetch_single_album_cover("", album.name);
    }
}


bool CoverLookup::fetch_artist_cover_standard(const QString& artist){

    QString target_file = Helper::get_artist_image_path(artist);
    QString url = Helper::calc_google_artist_adress(artist);

    if( QFile::exists(target_file) ){
        emit sig_cover_found(target_file);
    }

    else{
        start_new_thread(url, target_file);
    }

    return true;
}


bool CoverLookup::fetch_artist_cover_by_id(const int artist_id){

    if(artist_id < 0) return false;

    Artist artist;
    bool success = _db->getArtistByID(artist_id, artist);

    if(!success) return false;

    return fetch_artist_cover_standard(artist.name);
}


bool CoverLookup::fetch_artist_cover(const Artist& artist){

    return fetch_artist_cover_standard(artist.name);
}


bool CoverLookup::fetch_cover_by_searchstring(const QString& searchstring, const QString& target_name){
    QString url;
    url = Helper::calc_google_image_search_adress(searchstring);
    start_new_thread(url, target_name);

}


void CoverLookup::thread_finished(bool success){

    emit sig_finished(success);
}

void CoverLookup::cover_found(QString file_path){

    emit sig_cover_found(file_path);
}


static QString CoverLookup::get_target_path(QString& album_name, const QString& artist_name){

}

static QString CoverLookup::get_target_path(QString& album_name, const QStrinLiss& artists_name){

}

static QString CoverLookup::get_target_path(int album_id){

}

static QString CoverLookup::get_target_path(Album album){

}

static QString CoverLookup::get_target_path(const Artist& artist){

}

static QString CoverLookup::get_target_path(const QString& artist){}




CoverLookupAll::CoverLookupAll(QObject* parent, const AlbumList& album_list) :
    QObject(parent),
    _album_list(album_list),
    _run(true)
{
    _cl = new CoverLookup(this);
    connect(_cl, SIGNAL(sig_cover_found(QString)), this, SLOT(cover_found(QString)));
    connect(_cl, SIGNAL(sig_finished(bool)), this, SLOT(finished(bool));
}


CoverLookupAll::~CoverLookupAll(){
    _cl->stop();
}


void CoverLookupAll::start(){

    Album album = _album_list.back();
    _cl->fetch_album_cover(album);
}


void CoverLookupAll::stop(){
    _run = false;
    _cl->stop();
}


void CoverLookupAll::cover_found(QString file_path){

    _album_list.pop_back();
    emit sig_cover_found(file_path);

    if(!_run) return;

    usleep(1000000);
    Album album = _album_list.back();
    _cl->fetch_album_cover(album);
}

void CoverLookupAll::finished(bool b){
    Q_UNUSED(b);
    emit sig_finished();
}




CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const QString& album_name, const QString& artist_name, int n_covers) :
    QObject(parent),
    _album_name(album_name),
    _artist_name(artist_name),
    _n_covers(n_covers),
    _search_type(ST_Standard)

{ }

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const QString& album_name, const QStringLists& artists_name, int n_covers) :
    QObject(parent),
    _artists_name(artists_name),
    _n_covers(n_covers),
    _search_type(ST_Sampler)
{ }

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const Album& album, int n_covers) :
    QObject(parent),
    _album(album),
    _n_covers(n_covers),
    _search_type(ST_ByAlbum)
{ }

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, int album_id, int n_covers) :
    QObject(parent),
    _album_id(album_id),
    _n_covers(n_covers),
    _search_type(ST_ByID)
{ }

CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const QString& artist_name, int n_covers)  :
    QObject(parent),
    _artist_name(artist_name),
    _n_covers(n_covers),
    _search_type(ST_ByArtistName)
{ }


CoverLookupAlternative::CoverLookupAlternative(QObject* parent, const Artist& artist, int n_covers) :
    QObject(parent),
    _artist(artist),
    _n_covers(n_covers),
    _search_type(ST_ByArtist){

}


CoverLookupAlternative::~CoverLookupAlternative(){

    if(_cl) _cl->stop();
}



void CoverLookupAlternative::stop(){
    if(_cl) _cl->stop();
}


void CoverLookupAlternative::start(){

    _run = true;
    _cl = new CoverLookup(this, n_covers);
    connect(_cl, SIGNAL(sig_cover_found(QString)), this, SLOT(cover_found(QString)));
    connect(_cl, SIGNAL(sig_finished(bool)), this, SLOT(finished(bool)));


    switch(_search_type){

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
            _cl->fetch_artist_cover_standard(_artists_name);
            break;

        case ST_ByArtist:
            _cl->fetch_artist_cover(_artist);
            break;

        default:
            break;
    }
}


void CoverLookupAlternative::cover_found(QString cover_path){
    emit sig_new_cover(cover_path);
}

void CoverLookupAlternative::finished(bool b){
    Q_UNUSED(b);
    emit sig_finished();
}




