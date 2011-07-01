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


CoverFetchThread::CoverFetchThread() {
	_cover_source = COV_SRC_LFM;
	_num_covers_2_fetch = 1;

}

CoverFetchThread::~CoverFetchThread() {

	// TODO Auto-generated destructor stub
}

void CoverFetchThread::search_covers_for_albums(const vector<Album>& albums){

		vector<QImage> images;
		for(uint i=0; i<albums.size(); i++){
			Album album = albums[i];

			if(album.artists.size() > 0 ){
				for(int j=0; j<album.artists.size(); j++){

					QString path = Helper::get_cover_path( album.artists[j], album.name);

					if(_num_covers_2_fetch > 1 || !QFile::exists(path)){

						QStringList cover_adresses = call_and_parse_google( album.artists[j], album.name, 3, _cover_source);

						// download of covers successful
						if(download_covers(cover_adresses, 1, images)){
							// last image = best quality
							images[images.size() - 1 ].save(path);
						}

						// else only std cover loaded
						_images.push_back(images[images.size() -1]);

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

					if(_num_covers_2_fetch > 1 || !QFile::exists(path)){
						QStringList cover_adresses = call_and_parse_google("", album.name, 3, _cover_source);

						if(download_covers(cover_adresses, 1, images)){

								images[images.size() - 1 ].save(path);
								_images.push_back(images[images.size() - 1 ]);
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


void CoverFetchThread::search_alternative_covers_for_album(){

		QString album_name = "Boogie nights";
		get_alternative_album_covers(album_name, _images);
}


// run through all albums and search their covers
void CoverFetchThread::run(){

	_images.clear();
	if(_num_covers_2_fetch == 1){
		 if(_search_all_covers){

			 CDatabaseConnector* db = CDatabaseConnector::getInstance();
			 vector<Album> albums;
			 db->getAllAlbums(albums);
			 search_covers_for_albums(albums);
			 return;
		 }

		 else{
			 if(_vec_albums.size() == 0) return;
			 search_covers_for_albums(_vec_albums);
		 }
	}

	else{
		search_alternative_covers_for_album();
	}


}

void CoverFetchThread::get_images(vector<QImage>& images){

	images = _images;
}


bool CoverFetchThread::set_albums_to_fetch(const vector<Album> & vec_albums){
	_vec_albums = vec_albums;
}


bool CoverFetchThread::set_search_all_covers(bool all_albums){
	_search_all_covers = all_albums;
}

bool CoverFetchThread::set_cover_source(int source){
	_cover_source = source;

}


bool CoverFetchThread::set_num_covers_2_fetch(int num){
	_num_covers_2_fetch = num;
}
