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


CoverLookup* CoverLookup::getInstance(){
	static CoverLookup instance;
	return &instance;
}



CoverLookup::~CoverLookup() {
	// TODO Auto-generated destructor stub
}

void CoverLookup::thread_finished(){

	vector<QImage> images;
	vector<QPixmap> pixmaps;

	_thread->get_images(images);

	for(uint i=0; i<images.size(); i++){
		pixmaps.push_back(QPixmap::fromImage(images[i]));
	}
	qDebug() << Q_FUNC_INFO << " num pixmaps found: " << pixmaps.size() << " emit type = " << _emit_type;
	//emit covers_found(pixmaps);
	if(pixmaps.size() >= 1 && _emit_type == EMIT_ONE) {
		qDebug() << Q_FUNC_INFO << "Cover found";
		emit cover_found(true);
	}

	else if(pixmaps.size() == 0){
		emit cover_found(false);
		if(_metadata){
			research_cover(*_metadata);
			return;
		}
	}

	_emit_type = EMIT_NONE;
}



void CoverLookup::terminate_thread(){

	if(_thread->isRunning()){
		_thread->terminate();
	}

	vector<QPixmap> pixmaps;
	vector<QImage> images;
	_thread->get_images(images);
	for(uint i=0; i<images.size(); i++){

		pixmaps.push_back(QPixmap::fromImage(images[i]));
	}

	//emit covers_found(pixmaps);
	if(pixmaps.size() >= 1 && _emit_type == EMIT_ONE) {
		qDebug() << "emit";
		qDebug() << Q_FUNC_INFO << "cover found";
		emit cover_found(true);
	}

	else if(pixmaps.size() == 0)
		emit cover_found(false);


	_emit_type = EMIT_NONE;
}


void CoverLookup::search_cover(const MetaData& md){

	_metadata = (MetaData*) &md;

	qDebug() << "Search single cover";

	QString cover_path = Helper::get_cover_path(md.artist, md.album);


	if(QFile::exists(cover_path) && cover_path != ""){
		qDebug() << Q_FUNC_INFO << "File exists on hd";
		emit cover_found(true);
		return;
	}

	if(_thread->isRunning()) return;
	_emit_type = EMIT_ONE;

	if(md.album_id != -1){
		 CDatabaseConnector* db = CDatabaseConnector::getInstance();
		 vector<Album> albums;
		 Album album = db->getAlbumByID(md.album_id);

		 QStringList artists;
		 artists.push_back(md.artist);

		 album.artists = artists;

		 albums.push_back(album);
		 search_covers(albums);
	}

	else{
		Album album;
		album.artists.push_back(md.artist);
		album.name = md.album;

		 vector<Album> albums;
		 albums.push_back(album);
		 search_covers(albums);
	}

}


void CoverLookup::search_covers(const vector<Album> & vec_albums){



	if(_emit_type != EMIT_ONE) _emit_type = EMIT_ALL;
	if(_thread->isRunning()) _thread->terminate();
	_thread->set_cover_fetch_mode(COV_FETCH_MODE_ALL_ALBUMS);
	_thread->set_cover_source(COV_SRC_LFM);
	_thread->set_albums_to_fetch(vec_albums);
	_thread->start();
}



void CoverLookup::research_cover(const MetaData& md){

	_metadata = 0;
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


void CoverLookup::search_alternative_covers(const QString& album){

	Q_UNUSED(album);
	/*
	 * Maybe for google
	 * */
}



void CoverLookup::search_all_covers(){

	_metadata = 0;
	if(_emit_type != EMIT_ONE) _emit_type = EMIT_ALL;
	if(_thread->isRunning()) _thread->terminate();
	_thread->set_cover_fetch_mode(COV_FETCH_MODE_ALL_ALBUMS);
	_thread->set_num_covers_2_fetch(1);
	_thread->start();
}




bool CoverLookup::get_found_cover(int idx, QPixmap& p){
	if(idx < 0 || idx >= ((int) _alternative_covers.size()))
		return false;

	p = _alternative_covers[idx];
	return true;
}



Album CoverLookup::get_album_from_metadata(const MetaData& md){

	Album album;

	if(md.album_id != -1){
		 CDatabaseConnector* db = CDatabaseConnector::getInstance();
		 vector<Album> albums;
		 album = db->getAlbumByID(md.album_id);
		 return album;
	}

	else{

		album.artists.push_back(md.artist);
		album.name = md.album;

		return album;

	}
}
