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

#include <QFile>
#include <QImage>

#include <unistd.h>


CoverLookupInterface::CoverLookupInterface(QObject* parent):
	QObject(parent) {

}


CoverLookup::CoverLookup(QObject* parent, int n_covers) :
	CoverLookupInterface(parent),
    _n_covers(n_covers),
	_cft(NULL)
{
	_big = false;
    _db = CDatabaseConnector::getInstance();
}

CoverLookup::~CoverLookup() {

    if(_cft) {
        _cft->stop();
    }
}


void CoverLookup::set_big(bool big){
	_big = big;
}

void CoverLookup::start_new_thread(const CoverLocation& cl ) {

	if(_cft) {
		_cft->stop();
	}


	_cft = new CoverFetchThread(this, cl, _n_covers);
	_cfts << _cft;

	connect(_cft, SIGNAL(sig_finished(bool)), this, SLOT(finished(bool)));
	connect(_cft, SIGNAL(sig_cover_found(const CoverLocation&)),
			this, SLOT(cover_found(const CoverLocation&)));

    _cft->start();
}

void CoverLookup::stop() {

    if(!_cft) return;

    _cft->stop();
}

bool CoverLookup::fetch_cover(const CoverLocation& cl) {

	if( QFile::exists(cl.cover_path) && _n_covers == 1 ) {
		emit sig_cover_found(cl);
		return true;
	}

	start_new_thread( cl );

	return true;
}


bool CoverLookup::fetch_album_cover_standard(const QString& artist_name, const QString& album_name) {

	CoverLocation cl = CoverLocation::get_cover_location(album_name, artist_name, _big);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_album_cover_sampler(const QStringList& artists, const QString& album_name) {

	CoverLocation cl = CoverLocation::get_cover_location(album_name, artists, _big);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_album_cover_by_id(const int album_id) {

	CoverLocation cl = CoverLocation::get_cover_location(album_id, _big);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_album_cover(const Album& album) {

	CoverLocation cl = CoverLocation::get_cover_location(album, _big);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_artist_cover_standard(const QString& artist) {

	CoverLocation cl = CoverLocation::get_cover_location(artist, _big);
	return fetch_cover(cl);
}



bool CoverLookup::fetch_artist_cover(const Artist& artist) {

	return fetch_artist_cover_standard(artist.name);
}


bool CoverLookup::fetch_cover_by_searchstring(const QString& searchstring, const QString& target_name) {

	CoverLocation cl;
	cl.cover_path = target_name;
	cl.google_url = Helper::calc_google_image_search_adress(searchstring, _big);

	start_new_thread( cl );
	return true;
}


void CoverLookup::finished(bool success) {

	QObject* sender = QObject::sender();
	QObject** psender = &sender;

	foreach(CoverFetchThread* t, _cfts){
		if(t == (CoverFetchThread*) sender){
			_cfts.removeOne(t);
		}
	}

	delete sender;
	*psender = 0;

    emit sig_finished(success);
}

void CoverLookup::cover_found(const CoverLocation& file_path) {

    emit sig_cover_found(file_path);
}

void CoverLookup::emit_standard_cover() {

	CoverLocation cl = CoverLocation::getInvalidLocation();
	emit sig_cover_found(cl);
}


