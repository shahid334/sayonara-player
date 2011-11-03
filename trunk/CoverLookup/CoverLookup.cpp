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
#include <QCryptographicHash>
#include <QImage>
#include <QProgressDialog>
#include <QMessageBox>

#include <curl/curl.h>

using namespace std;

CoverLookup::CoverLookup() {
	// TODO Auto-generated constructor stub
	_thread = new CoverFetchThread();

	connect(_thread, SIGNAL(finished()), this, SLOT(thread_finished()));
}

CoverLookup* CoverLookup::getInstance() {
	static CoverLookup instance;
	return &instance;
}

CoverLookup::~CoverLookup() {
	// TODO Auto-generated destructor stub
}

void CoverLookup::thread_finished() {

	vector<QImage> images;
	vector<QPixmap> pixmaps;

	_thread->get_images(images);

	for (uint i = 0; i < images.size(); i++) {
		pixmaps.push_back(QPixmap::fromImage(images[i]));
	}
	//emit covers_found(pixmaps);
	if (pixmaps.size() >= 1 && _emit_type == EMIT_ONE) {
		emit cover_found(true);
	}

	else if (pixmaps.size() == 0) {
		emit cover_found(false);
		research_cover(_metadata);
		return;

	}

	_emit_type = EMIT_NONE;
}

void CoverLookup::terminate_thread() {

	if (_thread->isRunning()) {
		_thread->terminate();
	}

	vector<QPixmap> pixmaps;
	vector<QImage> images;
	_thread->get_images(images);
	for (uint i = 0; i < images.size(); i++) {

		pixmaps.push_back(QPixmap::fromImage(images[i]));
	}

	//emit covers_found(pixmaps);
	if (pixmaps.size() >= 1 && _emit_type == EMIT_ONE) {
		emit cover_found(true);
	}

	else if (pixmaps.size() == 0)
		emit cover_found(false);

	_emit_type = EMIT_NONE;
}

void CoverLookup::search_cover(const MetaData& md) {

	_metadata = md;
	if(CDatabaseConnector::getInstance()->getAlbumByID(md.album_id).is_sampler){
		_metadata.artist = "";
		_metadata.artist_id = -1;
	}


	QString cover_path = Helper::get_cover_path(_metadata.artist, _metadata.album);

	if (QFile::exists(cover_path) && cover_path != "") {
		emit cover_found(true);
		return;
	}

	if (_thread->isRunning())
		return;
	_emit_type = EMIT_ONE;

	if (_metadata.album_id != -1) {
		CDatabaseConnector* db = CDatabaseConnector::getInstance();
		vector<Album> albums;
		Album album = db->getAlbumByID(_metadata.album_id);

		QStringList artists;
		artists.push_back(_metadata.artist);

		album.artists = artists;

		albums.push_back(album);
		search_covers(albums);
	}

	else {
		Album album;
		album.artists.push_back(_metadata.artist);
		album.name = _metadata.album;

		vector<Album> albums;
		albums.push_back(album);
		search_covers(albums);
	}

}

void CoverLookup::search_covers(const vector<Album> & vec_albums) {

	if (_emit_type != EMIT_ONE
	) _emit_type = EMIT_ALL;
	if (_thread->isRunning())
		_thread->terminate();
	_thread->set_cover_fetch_mode(COV_FETCH_MODE_ALL_ALBUMS);
	_thread->set_cover_source(COV_SRC_LFM);
	_thread->set_albums_to_fetch(vec_albums);
	_thread->start();
}

void CoverLookup::research_cover(const MetaData& md) {


	vector<Album> albums;
	Album album = get_album_from_metadata(md);
	albums.push_back(album);
	album.name = md.album;
	album.artists.clear();
	album.artists.push_back(md.artist);
	_emit_type = EMIT_ONE;
	_thread->set_cover_fetch_mode(COV_FETCH_MODE_SINGLE_ALBUM);
	_thread->set_cover_source(COV_SRC_GOOGLE);
	_thread->set_num_covers_2_fetch(1);
	_thread->start();

}

void CoverLookup::search_alternative_covers(const QString& album) {

	Q_UNUSED(album);
	/*
	 * Maybe for google
	 * */
}

void CoverLookup::search_all_covers() {

	vector<Album> albums;
	CDatabaseConnector::getInstance()->getAllAlbums(albums);
	if (_emit_type != EMIT_ONE
	) _emit_type = EMIT_ALL;
	if (_thread->isRunning())
		_thread->terminate();
	_thread->set_albums_to_fetch(albums);
	_thread->set_cover_fetch_mode(COV_FETCH_MODE_ALL_ALBUMS);
	_thread->set_num_covers_2_fetch(1);
	_thread->start();
}

bool CoverLookup::get_found_cover(int idx, QPixmap& p) {
	if (idx < 0 || idx >= ((int) _alternative_covers.size()))
		return false;

	p = _alternative_covers[idx];
	return true;
}

Album CoverLookup::get_album_from_metadata(const MetaData& md) {

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
