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

#include <unistd.h>


CoverLookupInterface::CoverLookupInterface(QObject* parent):
	QObject(parent){

}


CoverLookup::CoverLookup(QObject* parent, int n_covers) :
	CoverLookupInterface(parent),
    _n_covers(n_covers),
	_cft(NULL)
{
    _db = CDatabaseConnector::getInstance();
}

CoverLookup::~CoverLookup() {

    if(_cft){
        _cft->stop();
    }

	CoverLookup* cl = this;
	CoverLookup** pcl = &cl;
	*pcl = 0;
}


void CoverLookup::start_new_thread(const CoverLocation& cl ){

	if(_cft){
		_cft->stop();
		delete _cft;
		_cft = 0;
	}



	_cft = new CoverFetchThread(this, cl, _n_covers);

	connect(_cft, SIGNAL(sig_finished(bool)), this, SLOT(finished(bool)));
    connect(_cft, SIGNAL(sig_cover_found(QString)), this, SLOT(cover_found(QString)));

    _cft->start();
}

void CoverLookup::stop(){

    if(!_cft) return;

    _cft->stop();
}

bool CoverLookup::fetch_cover(const CoverLocation& cl){

	if( QFile::exists(cl.cover_path) && _n_covers == 1 ) {
		emit sig_cover_found(cl.cover_path);
		return true;
	}

	start_new_thread( cl );

	return true;
}


bool CoverLookup::fetch_album_cover_standard(const QString& artist_name, const QString& album_name){

	CoverLocation cl = CoverLocation::get_cover_location(album_name, artist_name);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_album_cover_sampler(const QStringList& artists, const QString& album_name){

	CoverLocation cl = CoverLocation::get_cover_location(album_name, artists);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_album_cover_by_id(const int album_id){

	CoverLocation cl = CoverLocation::get_cover_location(album_id);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_album_cover(const Album& album){

	CoverLocation cl = CoverLocation::get_cover_location(album);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_artist_cover_standard(const QString& artist){

	CoverLocation cl = CoverLocation::get_cover_location(artist);
	return fetch_cover(cl);
}



bool CoverLookup::fetch_artist_cover(const Artist& artist){

    return fetch_artist_cover_standard(artist.name);
}


bool CoverLookup::fetch_cover_by_searchstring(const QString& searchstring, const QString& target_name){

	CoverLocation cl;
	cl.cover_path = target_name;
	cl.google_url = Helper::calc_google_image_search_adress(searchstring);

	start_new_thread( cl );
	return true;
}


void CoverLookup::finished(bool success){

	if(_cft){
		delete _cft; _cft = 0;
	}

    emit sig_finished(success);
}

void CoverLookup::cover_found(QString file_path){

    emit sig_cover_found(file_path);
}

void CoverLookup::emit_standard_cover(){
	QString sayonara_logo = Helper::getIconPath() + "logo.png";
	emit sig_cover_found(sayonara_logo);
}



CoverLookupAll::CoverLookupAll(QObject* parent, const AlbumList& album_list) :
	CoverLookupInterface(parent),
    _album_list(album_list),
    _run(true)
{
    _cl = new CoverLookup(this);
    connect(_cl, SIGNAL(sig_cover_found(QString)), this, SLOT(cover_found(QString)));
	connect(_cl, SIGNAL(sig_finished(bool)), this, SLOT(finished(bool)));
}


CoverLookupAll::~CoverLookupAll(){
	if(!_cl) return;

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

	// Google and other webservices block, if looking too fast
    usleep(1000000);

    Album album = _album_list.back();
    _cl->fetch_album_cover(album);
}

void CoverLookupAll::finished(bool success){
	emit sig_finished(success);
}


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


CoverLookupAlternative::~CoverLookupAlternative(){

	if(!_cl) return;

	_cl->stop();
}



void CoverLookupAlternative::stop(){

	if(!_cl) return;

	_cl->stop();
}


void CoverLookupAlternative::start(){

    _run = true;
	_cl = new CoverLookup(this, _n_covers);
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
			_cl->fetch_artist_cover_standard(_artist_name);

            break;

        case ST_ByArtist:
            _cl->fetch_artist_cover(_artist);
            break;

        default:
            break;
    }
}


void CoverLookupAlternative::cover_found(QString cover_path){
	emit sig_cover_found(cover_path);
}

void CoverLookupAlternative::finished(bool success){

	emit sig_finished(success);

	if(!_cl) return;

	delete _cl; _cl = 0;
}





