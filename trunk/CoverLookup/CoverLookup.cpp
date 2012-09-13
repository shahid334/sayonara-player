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
#include "DatabaseAccess/CDatabaseConnector.h"

#include <string>
#include <iostream>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QImage>

#include <curl/curl.h>

using namespace std;

CoverLookup::CoverLookup(QString caller_class) {

	_caller_class = caller_class;
	_thread = new CoverFetchThread();
	_research_done = false;
	connect(_thread, SIGNAL(finished()), this, SLOT(thread_finished()));
}

CoverLookup::~CoverLookup() {
}


void CoverLookup::thread_finished() {

	vector<QImage> images;
	vector<QPixmap> pixmaps;

	_thread->get_images(images);

	// convert images to pixmaps
	for (uint i = 0; i < images.size(); i++) {
		pixmaps.push_back(QPixmap::fromImage(images[i]));
	}

	if (pixmaps.size() >= 1) {

		switch(_thread->get_cover_fetch_mode()){
			case COV_FETCH_MODE_ALBUM_STR:
			case COV_FETCH_MODE_ALL_ALBUMS:
			case COV_FETCH_MODE_SINGLE_ALBUM:
				emit sig_cover_found(_caller_class, Helper::get_cover_path(_metadata.artist, _metadata.album));
				break;

			case COV_FETCH_MODE_ARTIST_STR:
				emit sig_cover_found(_caller_class, Helper::get_artist_image_path(_metadata.artist));
				break;
			case COV_FETCH_MODE_SEARCHSTRING:
				emit sig_multi_covers_found(_caller_class);
				break;
			default: break;
		}
		_research_done = false;
	}

	else if (pixmaps.size() == 0) {
		emit sig_cover_found(_caller_class, "");

		// if not already tried google and not searching an artist string
		if( !_research_done &&
			_thread->get_cover_fetch_mode() != COV_FETCH_MODE_ARTIST_STR &&
			_thread->get_cover_fetch_mode() != COV_FETCH_MODE_SEARCHSTRING){
			qDebug() << "No cover found, research";
			research_cover(_metadata);
		}

		else{
			_research_done = false;
		}

		return;
	}
}

void CoverLookup::terminate_thread() {


	vector<QPixmap> pixmaps;
	vector<QImage> images;

	if (_thread->isRunning()) {
		_thread->terminate();
	}

	_thread->get_images(images);
	for (uint i = 0; i < images.size(); i++) {
		pixmaps.push_back(QPixmap::fromImage(images[i]));
	}

	if (pixmaps.size() >= 1) {
		emit sig_cover_found(_caller_class, Helper::get_cover_path(_metadata.artist, _metadata.album));
	}

	else if (pixmaps.size() == 0)
		emit sig_cover_found(_caller_class, "");
}



// search one cover
void CoverLookup::search_cover(const MetaData& md) {

	_metadata = md;

	QString cover_path = Helper::get_cover_path(_metadata.artist, _metadata.album);

	if (QFile::exists(cover_path) && cover_path != "") {
		emit sig_cover_found(_caller_class, Helper::get_cover_path(_metadata.artist, _metadata.album));
		return;
	}

	if (_thread->isRunning())
		return;

	vector<Album> albums;
	Album album;

	if (_metadata.album_id != -1){
		CDatabaseConnector* db = CDatabaseConnector::getInstance();
		album = db->getAlbumByID(_metadata.album_id);
	}

	else{
		album.name = _metadata.album;
	}


	album.artists.clear();
	album.artists.push_back(_metadata.artist);

	albums.push_back(album);
	search_covers(albums);
}

// search more covers
void CoverLookup::search_covers(const vector<Album> & vec_albums) {

	if (_thread->isRunning())
		_thread->terminate();
	_thread->setup_fetch_album_covers(vec_albums, COV_SRC_LFM);
	_thread->start();
}

// search covers for all albums
void CoverLookup::search_all_covers() {

	vector<Album> albums;
	CDatabaseConnector::getInstance()->getAllAlbums(albums);
	search_covers(albums);
}


// search cover for artist
void CoverLookup::search_artist_image(const QString& artist){

	if (_thread->isRunning()) return;

	_metadata = MetaData();
	_metadata.artist = artist;


	_thread->setup_fetch_artist_image(artist, COV_SRC_LFM);
	_thread->start();
}


// research on google
void CoverLookup::research_cover(const MetaData& md) {

	_research_done = true;
	if (_thread->isRunning()) return;

	_metadata = md;

	Album album = _get_album_from_metadata(md);
	album.artists.clear();
	album.artists.push_back(md.artist);
	_thread->setup_fetch_single_album(album, COV_SRC_GOOGLE);
	_thread->start();
}


void  CoverLookup::search_images_by_searchstring(QString searchstring, int n_images){

	if(_thread->isRunning()) return;

	_thread->setup_fetch_by_searchstring(searchstring, 10);
	_thread-> start();

}




bool CoverLookup::get_found_cover(int idx, QPixmap& p) {
	if (idx < 0 || idx >= ((int) _alternative_covers.size()))
		return false;

	p = _alternative_covers[idx];
	return true;
}



Album CoverLookup::_get_album_from_metadata(const MetaData& md) {

	Album album;

	if (md.album_id != -1) {
		CDatabaseConnector* db = CDatabaseConnector::getInstance();
		vector<Album> albums;
		album = db->getAlbumByID(md.album_id);
		return album;
	}

	else {

		album.artists.push_back(md.artist);
		album.name = md.album;

		return album;
	}
}
