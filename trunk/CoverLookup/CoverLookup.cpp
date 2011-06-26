/*
 * CoverLookup.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#include "CoverLookup/CoverLookup.h"
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
#include <QThread>


#include <curl/curl.h>

using namespace std;

char* webpage = 0;
size_t webpage_bytes = 0;

char* image_data =0;
size_t image_bytes = 0;



int find_jpg_file_ending(char* ptr, size_t size, char* search_string, size_t search_size, int beginning){

	for(size_t i = beginning; i<size; i++){

		if(ptr[i] == search_string[0] && !isspace(ptr[i]) ){

			size_t j=0;

			while(j < search_size){

				if(search_string[j] != ptr[i]) {
					break;
				}

				i++; j++;
				while(isspace(ptr[i])) i++;
				while(isspace(search_string[j])) j++;
			}

			if(j == search_size){
				return i;
			}
		}
	}

	return -1;
}


int find_jpg_file_beginning(char* ptr, int beginning){
	int i = beginning;
	while(i > 0){

		if(	ptr[i-3] == 'h' &&
			ptr[i-2] == 't' &&
			ptr[i-1] == 't' &&
			ptr[i] == 'p'){return i-3;}


		i--;
	}

	return -1;

}


vector<string> calc_adresses_from_webpage(uint num){

	vector<string> adresses;
	if(webpage == 0) {
		cout << "No webpage found" << endl;
		return adresses;
	}

	int search_start = 0;

	while(true){

		string search_string = ".jpg";

		int loc_start = -1;
		int loc_end = find_jpg_file_ending(webpage, webpage_bytes, (char*) search_string.c_str(), search_string.size(), search_start);


		if(loc_end != -1){
			loc_start = find_jpg_file_beginning(webpage, loc_end);
			search_start = loc_end;
		}

		else break;

		if(loc_start != -1){
			size_t adress_length = (loc_end - loc_start) + 1;

			char* str = new char[adress_length];
			memset(str, ' ', adress_length);
			memcpy(str, webpage + loc_start, adress_length);
			str[adress_length-1] = '\0';

			string adress = string(str);

			delete str;

			bool already_there = false;
			for(vector<string>::iterator it=adresses.begin(); it != adresses.end(); it++){
				if(it->compare(adress) == 0){
					already_there = true;
					break;
				}
			}

			if(!already_there){
				adresses.push_back(adress);

				//cout << "adress= " << adress << endl;
				if(adresses.size() >= num) break;
			}
		}
	}

	return adresses;
}


size_t save_image( void *ptr, size_t size, size_t nmemb, FILE *userdata){

	(void) userdata;
	if(image_bytes == 0){
		image_bytes = size * nmemb;
		image_data = (char*) (malloc(image_bytes));
		memcpy ( image_data, ptr, image_bytes );
	}

	else {
		image_data = (char*) (realloc(image_data, image_bytes + size * nmemb));
		memcpy(image_data + image_bytes, ptr, size*nmemb);
		image_bytes += size * nmemb;
	}

	return nmemb * size;

}


size_t get_content( void *ptr, size_t size, size_t nmemb, FILE *userdata){

	(void) userdata;

	if(webpage_bytes == 0){
		webpage_bytes = size * nmemb;
		webpage = (char*) (malloc(webpage_bytes));
		memcpy ( webpage, ptr, webpage_bytes );
	}

	else {
		webpage = (char*) (realloc(webpage, webpage_bytes + size * nmemb));
		memcpy(webpage + webpage_bytes, ptr, size*nmemb);
		webpage_bytes += size * nmemb;
	}

	return size * nmemb;
}




CoverLookup::CoverLookup() {
	// TODO Auto-generated constructor stub

}



CoverLookup::~CoverLookup() {
	// TODO Auto-generated destructor stub
}


QString CoverLookup::get_cover_path(QString artist, QString album){
		QString cover_token = QCryptographicHash::hash(artist.toUtf8() + album.toUtf8(), QCryptographicHash::Md5).toHex();
			QString cover_path =  QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + cover_token + ".jpg";

			if(QFile::exists(cover_path)) return cover_path;
			else return QString("");

	}

void CoverLookup::search_cover(bool emit_signal){

	_pixmaps.clear();

	CURL *curl_find_img = curl_easy_init();

	string url_adress = calc_url_adress();

	/* Find images on Google*/
	if(curl_find_img) {

		cout << "URL: " << url_adress << endl;

		curl_easy_setopt(curl_find_img, CURLOPT_URL, url_adress.c_str());
		curl_easy_setopt(curl_find_img, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_find_img, CURLOPT_WRITEFUNCTION, get_content);



		curl_easy_perform(curl_find_img);
		curl_easy_cleanup(curl_find_img);
	}

	_cover_adresses = calc_adresses_from_webpage(3);

	if(webpage != 0){
		free(webpage);
		webpage = 0;
	}
	webpage_bytes = 0;

	download_covers(1);
	if(emit_signal) emit cover_found(_pixmaps[0]);
}


void CoverLookup::search_cover(const MetaData& metadata, bool emit_signal){
	_artist = string(metadata.artist.trimmed().replace(" ", "+").toUtf8().constData());
	_album = string(metadata.album.trimmed().replace(" ", "+").toUtf8().constData());

	//_filepath = metadata.filepath.left(metadata.filepath.lastIndexOf("/"));

	_cover_token = QCryptographicHash::hash(metadata.artist.toUtf8() + metadata.album.toUtf8(), QCryptographicHash::Md5).toHex();
	_cover_path = QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + _cover_token + ".jpg";

	if(!QFile::exists(QDir::homePath() + QDir::separator() +".Sayonara" + QDir::separator() + "covers")){
		QDir().mkdir(QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers");
		_cur_cover = -1;
		search_cover(emit_signal);

	}

	else if(QFile::exists(_cover_path)){
		QPixmap pixmap = QPixmap::fromImage(QImage(_cover_path));
		if(emit_signal) emit cover_found(pixmap);
	}

	else{
		_cur_cover = -1;
		search_cover(emit_signal);
	}
}


void CoverLookup::showCoverAlternatives(){

	/*download_covers(10, false);

	for(vector<QPixmap>::iterator it = _pixmaps.begin(); it != _pixmaps.end(); it++){


	}*/

}



void CoverLookup::download_covers(uint num){

	image_data = 0;
	image_bytes = 0;

	_pixmaps.clear();
	if(_cover_adresses.size() == 0) {
		// get a few more adresses, maybe many images are dead
		_cover_adresses = calc_adresses_from_webpage(num * 2);
		if(_cover_adresses.size() == 0) {
			QPixmap pixmap = QPixmap::fromImage(QImage(Helper::getIconPath() + "append.png"));
			_pixmaps.push_back(pixmap);

			return;
		}
	}

	/* save image of first hit */
	CURL *curl_save_img;

	for(vector<string>::iterator it = _cover_adresses.begin(); it != _cover_adresses.end(); it++){

		curl_save_img = curl_easy_init();

		if(curl_save_img){

			curl_easy_setopt(curl_save_img, CURLOPT_URL, it->c_str());
			curl_easy_setopt(curl_save_img, CURLOPT_WRITEFUNCTION, save_image);
			curl_easy_setopt(curl_save_img, CURLOPT_BUFFERSIZE, 3000);
			curl_easy_setopt(curl_save_img, CURLOPT_TIMEOUT_MS, 3000);

			curl_easy_perform(curl_save_img);
			curl_easy_cleanup(curl_save_img);

			QImage img = QImage::fromData((const uchar*) image_data, image_bytes);

			if(!img.isNull() && image_bytes > 500){

				img.save( _cover_path );
				QPixmap pixmap = QPixmap::fromImage(img);
				_pixmaps.push_back(pixmap);

				if(_pixmaps.size() == num) break;

			}

			if(image_data != 0){
				free(image_data);
				image_data = 0;
			}
			image_bytes = 0;

		} // curl init successful

	} // for all cover adresses
}

void CoverLookup::search_all_covers(){


	QMessageBox msgBox;
	 msgBox.setText("This function sucks, lasts very long and you cannot abort it.");
	 msgBox.setInformativeText("Proceed?");
	 msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	 msgBox.setDefaultButton(QMessageBox::No);
	 int ret = msgBox.exec();

	 if(ret != QMessageBox::Yes) return;



	_terminate_cover_fetch = false;
	QProgressDialog dialog(0);

		dialog.setModal(true);
		dialog.setValue(1);


		connect(&dialog, SIGNAL(canceled()), this, SLOT(terminate_cover_fetch()));

		dialog.show();


	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	vector<Album> albums;
	db->getAllAlbums(albums);

	for(uint i=0; i<albums.size() && !_terminate_cover_fetch; i++){
		Album album = albums[i];
		QString artist = "";
		MetaData md;
		md.album = album.name;

		dialog.setLabelText(album.name);
		if(album.artists.size() > 0 ){
			for(int j=0; j<album.artists.size(); j++){
				QString artist = album.artists[j];

				md.artist = artist;

				search_cover(md, false);
			}
		}


		else {
			if(album.name != "") search_cover(md, false);
		}

		double val = (100.0 * i) / ( 1.0 *albums.size());
		//qDebug() << val;
		dialog.setValue((int) val );
	}

	dialog.close();
}

QPixmap CoverLookup::add_new_pixmap(string cover_filename){

/*	IplImage* img = cvLoadImage(cover_filename.c_str());
	double ratio = img->width * 1.0 / img->height;

	IplImage* img_resized = cvCreateImage(cvSize(256, 256 / ratio), IPL_DEPTH_8U, 3);
	cvResize(img, img_resized, CV_INTER_AREA);

	QImage qimg;

	qimg = QImage( (const uchar *)img_resized->imageData,
				img_resized->width,
				img_resized->height,
				QImage::Format_RGB888).rgbSwapped();
*/

	QPixmap pixmap = QPixmap(cover_filename.c_str());


	_pixmaps.push_back(pixmap);

	/*cvReleaseImage(&img);
	cvReleaseImage(&img_resized);*/

	return pixmap;
}


string CoverLookup::calc_url_adress(){

	string url = string("http://www.google.de/images?q=");

	string artist = _artist;
	string album = _album;

	artist = Helper::replace_chars(artist, '&', 'n');
	artist = Helper::replace_chars(artist, '?', '+');

	album = Helper::replace_chars(album, '&', 'n');
	album = Helper::replace_chars(album, '?', '+');

	url += artist + "+" + album;
	url +=  "&tbs=isz:s,ift:jpg";			// klein*/

	return url;
}


void CoverLookup::terminate_cover_fetch(){

	_terminate_cover_fetch = true;
}
