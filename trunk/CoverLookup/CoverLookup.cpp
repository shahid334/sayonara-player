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



CoverLookup::~CoverLookup() {
	// TODO Auto-generated destructor stub
}

void CoverLookup::thread_finished(){

	vector<QImage> images;
	vector<QPixmap> pixmaps;

	_thread->get_images(images);

	for(uint i=0; i<images.size(); i++){
		pixmaps.push_back(QPixmap::fromImage(images[i]));
		alternative_covers.push_back(QPixmap::fromImage(images[i]));
	}

	//emit covers_found(pixmaps);
	if(pixmaps.size() >= 1 && _emit_type == EMIT_ONE) {
		qDebug() << "emit";
		emit cover_found(pixmaps[0]);
	}

	if(pixmaps.size() >= 1 && _emit_type == EMIT_ALL){

		emit cover_found(pixmaps[rand()%pixmaps.size()]);
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
		emit cover_found(pixmaps[0]);
	}


	_emit_type = EMIT_NONE;
}


void CoverLookup::search_cover(const MetaData& md){
	QString cover_path = Helper::get_cover_path(md.artist, md.album);


	if(QFile::exists(cover_path) && cover_path != ""){
		QPixmap pixmap = QPixmap::fromImage(QImage(cover_path));
		emit cover_found(pixmap);
		return;
	}

	if(_thread->isRunning()) return;
	_emit_type = EMIT_ONE;

	if(md.album_id != -1){
		 CDatabaseConnector* db = CDatabaseConnector::getInstance();
		 vector<Album> albums;
		 Album album = db->getAlbumByID(md.album_id);

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
	_thread->set_search_all_covers(false);
	_thread->set_cover_source(COV_SRC_LFM);
	_thread->set_albums_to_fetch(vec_albums);
	_thread->start();


}


void CoverLookup::search_alternative_covers(const QString& album){

	if(alternative_covers.size() > 0){
		emit cover_found(alternative_covers[rand()%alternative_covers.size()]);
		return;
	}

	if(_thread->isRunning()) return;

	_emit_type = EMIT_ONE;

	_thread->set_search_all_covers(false);
	_thread->set_cover_source(COV_SRC_GOOGLE);
	_thread->set_num_covers_2_fetch(10);
	_emit_type = EMIT_ALL;

	_thread->start();

}




void CoverLookup::search_all_covers(){

	if(_emit_type != EMIT_ONE) _emit_type = EMIT_ALL;
	if(_thread->isRunning()) _thread->terminate();
	qDebug() << "start...";
	_thread->set_search_all_covers(true);
	_thread->start();
}

