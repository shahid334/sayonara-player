/* CoverFetchThread.cpp */

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
 * CoverFetchThread.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: luke
 */

#include "CoverLookup/CoverFetchThread.h"
#include "CoverLookup/CoverDownloader.h"
#include "HelperStructs/MetaData.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include <QDebug>
#include <QMessageBox>
#include <QImage>
#include <QStringList>
#include <QFile>
#include <QDir>

#include <vector>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

using namespace std;


CoverFetchThread::CoverFetchThread() {
	_cover_source = COV_SRC_LFM;
	_num_covers_2_fetch = 1;

}

CoverFetchThread::~CoverFetchThread() {

	// TODO Auto-generated destructor stub
}

void CoverFetchThread::search_images_for_artist(const QString& artist_name, int num){

    qDebug() << "search images for artists";
    vector<QImage> images;

	QString path = Helper::get_artist_image_path(artist_name);
	if( num == 1 && QFile::exists(path)){
		_images.push_back(QImage(path));
		return;
	}

	QStringList image_adresses = call_and_parse_artist(artist_name, num + num / 2, _cover_source);
	_n_found_images = download_covers(image_adresses, _num_covers_2_fetch, images, true);

	for(int c=0; c<_n_found_images; c++){
		_images.push_back(images[c]);
		if(c == 0){
			images[c].save(path);
		}
	}
}

void CoverFetchThread::search_images_for_albums(){

    qDebug() << "search images for albums";
		vector<QImage> images;
		for(uint i=0; i<_vec_albums.size(); i++){
			Album album = _vec_albums[i];

			if(album.artists.size() > 0 ){

				for(int j=0; j<album.artists.size(); j++){

					QString path = Helper::get_cover_path( album.artists[j], album.name);

					if( _num_covers_2_fetch > 0 || !QFile::exists(path)){

						QStringList cover_adresses = call_and_parse_album( album.artists[j], album.name, 3, _cover_source);

						// download of covers successful

						_n_found_images = download_covers(cover_adresses, _num_covers_2_fetch, images);
						if(_n_found_images){

							for(int c=0; c<_n_found_images; c++){

								if(c==0) {
									images[c].save(path);
								}
								_images.push_back(images[c]);
							}
						}

						usleep(50000);
					}

					// image found on HD
					else{
						_images.push_back(QImage(path));
					}

				} // for all artists

			} //one or more artists

			else { // no artist
				if(album.name != ""){
					QString path = Helper::get_cover_path("", album.name);

					if(_num_covers_2_fetch > 0 || !QFile::exists(path)){
						QStringList cover_adresses = call_and_parse_album("", album.name, 3, _cover_source);

						_n_found_images = download_covers(cover_adresses, _num_covers_2_fetch, images);
						if(_n_found_images > 0){

							for(int c=0; c<_n_found_images; c++){
								if(c==0){
									images[c].save(path);
								}
								_images.push_back(images[c]);
							}
						 }

						usleep(50000);
					}

					else{
						_images.push_back(QImage(path));
					}

				}
			}
		}
}





void CoverFetchThread::search_images_for_album_str(const QString album_name, int num){

	//search_covers_by_album_name(album_name, _images, num);

}

void CoverFetchThread::search_images_for_artist_str(const QString artist_name, int num){

	QString tmp_artist_name = artist_name;

	search_images_for_artist(tmp_artist_name, num);

}

void CoverFetchThread::search_images_for_searchstring(){


	QString adress = Helper::calc_google_image_search_adress(_universal_searchstring, GOOGLE_IMG_SMALL, GOOGLE_FT_JPG);



	QStringList lst = call_and_parse(adress, _num_covers_2_fetch * 2);

	if(lst.size() > 0)
		_n_found_images = download_covers(lst, _num_covers_2_fetch, _images, true );
	else
		qDebug() << "cannot find cover paths";

}


// run through all albums and search their covers
void CoverFetchThread::run(){

	this->_images.clear();

	switch(_cover_fetch_mode){

		case COV_FETCH_MODE_ALBUM_STR:
			search_images_for_album_str(_album_searchstring, _num_covers_2_fetch);
		break;

		case COV_FETCH_MODE_ARTIST_STR:
			search_images_for_artist_str(_artist_searchstring, _num_covers_2_fetch);
		break;

		case COV_FETCH_MODE_SINGLE_ALBUM:
		case COV_FETCH_MODE_ALL_ALBUMS:
			search_images_for_albums();
		break;

		case COV_FETCH_MODE_SEARCHSTRING:
			search_images_for_searchstring();
			break;

		default: break;
	}

}


/***************************
 * GETTER / SETTER
 ***************************/

void CoverFetchThread::get_images(vector<QImage>& images){

	images = _images;
}

bool CoverFetchThread::get_certain_image(int idx, QImage& img){
	if(idx >= (int) _images.size()) return false;
	img = _images[idx];
	return true;

}

int CoverFetchThread::get_num_images(){
	return _images.size();
}


int CoverFetchThread::get_cover_source(){
		return _cover_source;
}


int CoverFetchThread::get_cover_fetch_mode(){
	return _cover_fetch_mode;
}


void CoverFetchThread::setup_fetch_artist_image(const QString& artist, int source, int n_covers){
	_album_searchstring = "";
	_artist_searchstring = artist;
	_cover_source = source;
	_cover_fetch_mode = COV_FETCH_MODE_ARTIST_STR;
	_num_covers_2_fetch = n_covers;
	_vec_albums.clear();
	_images.clear();
}


void CoverFetchThread::setup_fetch_album_covers(const AlbumList& albums, int source, int n_covers){

	_album_searchstring = "";
	_artist_searchstring = "";
	_cover_source = source;
	_cover_fetch_mode = COV_FETCH_MODE_ALL_ALBUMS;
	_num_covers_2_fetch = n_covers;
	_vec_albums = albums;
	_images.clear();
}

void CoverFetchThread::setup_fetch_single_album(QString album_str, QString artist_str, int source){
	Album album;
	album.artists << artist_str;
	album.name = album_str;
	setup_fetch_single_album(album, source);

}

void CoverFetchThread::setup_fetch_single_album(const Album& album, int source, int n_covers){
	_album_searchstring = album.name;
	_artist_searchstring = album.artists[0];
	_cover_source = source;
	_cover_fetch_mode = COV_FETCH_MODE_SINGLE_ALBUM;
	_num_covers_2_fetch = n_covers;
	_vec_albums.clear();
	_vec_albums.push_back(album);
	_images.clear();
}

void CoverFetchThread::setup_fetch_by_searchstring(QString searchstring, int n_covers){


	_universal_searchstring = searchstring;
	_cover_source = COV_SRC_GOOGLE;
	_num_covers_2_fetch = n_covers;
	_cover_fetch_mode = COV_FETCH_MODE_SEARCHSTRING;
	_images.clear();
}

void CoverFetchThread::reset(){
	_vec_albums.clear();
	_images.clear();
	_cover_source = -1;
	_num_covers_2_fetch = -1;
	_album_searchstring = "";
	_artist_searchstring = "";
	_universal_searchstring = "";
	_cover_fetch_mode = -1;

}
