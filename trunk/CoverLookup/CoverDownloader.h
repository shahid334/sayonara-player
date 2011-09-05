/*
 * CoverDownloader.h
 *
 *  Created on: Jun 29, 2011
 *      Author: luke
 */

#ifndef COVERDOWNLOADER_H_
#define COVERDOWNLOADER_H_

#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>

#include <curl/curl.h>

#include <QStringList>
#include <QImage>
#include <QDebug>
#include <QUrl>

using namespace std;


char* webpage;
size_t webpage_bytes;

char* image_data;
size_t image_bytes;

void clear_webpage_data(){
	if(webpage){
		free(webpage);
		webpage = 0;
	}
	webpage_bytes = 0;
}

void clear_image_data(){

	if(image_data){
		free(image_data);
		image_data = 0;
	}
	image_bytes = 0;
}

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


QStringList calc_adresses_from_webpage(uint num, QString qwebpage){

	QStringList adresses;
	if(qwebpage.size() == 0) {
		cout << "No webpage found" << endl;
		return adresses;
	}

	int search_start = 0;

//	qDebug() << qwebpage;

	while(true){

		QString search_string = QString(".jpg");

		int loc_end = qwebpage.indexOf(search_string, search_start);
		if(loc_end == -1) {
			search_string = QString(".png");

			loc_end = qwebpage.indexOf(search_string, search_start);
		}



		if(loc_end != -1){
			loc_end += 4;


			QString tmp_str = "";
			int tmp_idx = loc_end-1;

			while(!tmp_str.startsWith("http") && tmp_idx > search_start){
				tmp_str.prepend(qwebpage.at(tmp_idx));
				tmp_idx--;
			}

			qDebug() << tmp_str;

			bool adress_found = (tmp_str.startsWith("http"));
			if(adress_found){
				QString adress = tmp_str;
				if(!adresses.contains(adress)){

					adresses.push_back(adress);

					if( num <= (uint) adresses.size() ) break;
				}

			}

			search_start = loc_end;
		}


		else {

			return adresses;
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

	//qDebug() << "Bytes: " << size * nmemb;
	return size * nmemb;
}

QStringList call_and_parse_google(QString artist, QString album, int num_adresses, int cover_source){

	QString url_adress;

	if(cover_source == COV_SRC_GOOGLE){
		url_adress = Helper::calc_cover_google_adress(artist, album);
	}

	else {
		url_adress = Helper::calc_cover_lfm_adress(artist, album);
	}

	/* Find images on Google*/
	QStringList cover_adresses;

	CURL *curl_find_img = curl_easy_init();
	if(curl_find_img) {

		//qDebug() << "URL: " << url_adress;

		curl_easy_setopt(curl_find_img, CURLOPT_URL, url_adress.toLocal8Bit().data());
		curl_easy_setopt(curl_find_img, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_find_img, CURLOPT_WRITEFUNCTION, get_content);
		curl_easy_setopt(curl_find_img, CURLOPT_BUFFERSIZE, 3000);
		curl_easy_setopt(curl_find_img, CURLOPT_TIMEOUT_MS, 3000);

	}


	curl_easy_perform(curl_find_img);

	int time2go = 3000000;
	while(webpage_bytes == 0){
		usleep(5000);
		time2go-=5000;
		if(time2go <= 0) break;
	}

	if(webpage_bytes== 0 || !webpage) qDebug() << "URL: " << url_adress;
	else cover_adresses = calc_adresses_from_webpage(num_adresses, QString(webpage));



	curl_easy_cleanup(curl_find_img);

	return cover_adresses;
}



QStringList call_and_parse_lfm_artist(QString artist, int num_adresses){

	QString url_adress = Helper::calc_search_artist_adress(artist);


	/* Find images on Google*/
	QStringList cover_adresses;

	CURL *curl_find_img = curl_easy_init();
	if(curl_find_img) {

		//qDebug() << "URL: " << url_adress;

		curl_easy_setopt(curl_find_img, CURLOPT_URL, url_adress.toLocal8Bit().data());
		curl_easy_setopt(curl_find_img, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_find_img, CURLOPT_WRITEFUNCTION, get_content);
		curl_easy_setopt(curl_find_img, CURLOPT_BUFFERSIZE, 3000);
		curl_easy_setopt(curl_find_img, CURLOPT_TIMEOUT_MS, 3000);

	}


	curl_easy_perform(curl_find_img);

	int time2go = 3000000;
	while(webpage_bytes == 0){
		usleep(5000);
		time2go-=5000;
		if(time2go <= 0) break;
	}

	if(webpage_bytes== 0 || !webpage) qDebug() << "URL: " << url_adress;
	else cover_adresses = calc_adresses_from_webpage(num_adresses, QString(webpage));



	curl_easy_cleanup(curl_find_img);

	return cover_adresses;
}



QStringList call_and_parse_lfm_album(QString album, int num_adresses){

	Q_UNUSED(album);
	Q_UNUSED(num_adresses);

	QStringList list;

	return list;
}




bool download_covers(QStringList adresses, uint num_covers_to_fetch, vector<QImage>& vec_images){

	vec_images.clear();
	image_data = 0;
	image_bytes = 0;

	bool found = false;

	if(adresses.size() == 0) {
		// get a few more adresses, maybe many images are dead
		adresses = calc_adresses_from_webpage(num_covers_to_fetch * 2, QString(webpage));
		if(adresses.size() == 0) {
			QImage img = QImage(Helper::getIconPath() + "append.png");
			vec_images.push_back(img);


			if(webpage_bytes > 0){

			free(webpage);
				webpage = 0;
			}
			webpage_bytes = 0;

			return false;
		}
	}



		for(int i=adresses.size() -1; i>=0; i--){

			QString adress = adresses[i];

			CURL *curl_save_img = curl_easy_init();

			clear_image_data();


			if(curl_save_img){

				curl_easy_setopt(curl_save_img, CURLOPT_URL, adress.toLocal8Bit().data());
				curl_easy_setopt(curl_save_img, CURLOPT_WRITEFUNCTION, save_image);
				curl_easy_setopt(curl_save_img, CURLOPT_BUFFERSIZE, 3000);
				//curl_easy_setopt(curl_save_img, CURLOPT_TIMEOUT_MS, 100);

				curl_easy_perform(curl_save_img);

				int time2go = 100000;
				while(image_bytes == 0){
					usleep(5000);
					time2go-=5000;
					if(time2go <= 0) break;
				}

				if(image_bytes > 0 && image_data){

					QImage img;
					bool good_image = false;


					good_image = img.loadFromData( (const uchar*) image_data, image_bytes);

					if(good_image && !img.isNull() && image_bytes > 1000){

						vec_images.push_back(img);

						found = true;
						clear_image_data();
						if(vec_images.size() == num_covers_to_fetch) {
							qDebug() << "vec size = " << vec_images.size();
							break;
						}

					}

					clear_image_data();
					curl_easy_cleanup(curl_save_img);
				}



			} // curl init successful
		} // for all cover adresses


	if(vec_images.size() == 0){

		vec_images.push_back(QImage(Helper::getIconPath() + "append.png"));
	}



	if(webpage_bytes > 0){

	free(webpage);
		webpage = 0;
	}
	webpage_bytes = 0;


	return found;
}



void get_alternative_album_covers(QString url, vector<QImage>& images, int num){

	clear_webpage_data();

	CURL *curl_lfm = curl_easy_init();
	if(curl_lfm) {
		qDebug() << "Url = " << url;
		curl_easy_setopt(curl_lfm, CURLOPT_URL, url.toLocal8Bit().data());
		curl_easy_setopt(curl_lfm, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl_lfm, CURLOPT_WRITEFUNCTION, get_content);
	}


	curl_easy_perform(curl_lfm);

	int time2go = 100000;
	while(webpage_bytes == 0){
		usleep(5000);
		time2go-=5000;
		if(time2go <= 0) break;
	}

	if(webpage_bytes== 0 || !webpage) qDebug() << "BAD URL: " << url;
	else {

		QString qwebpage(webpage);
		QStringList tagList = qwebpage.split('>');
		QStringList adresses;
		for(int i=0; i<tagList.size(); i++){
			tagList[i] = tagList[i].trimmed();
			if(tagList[i].contains("image size=\"large\"")){
				adresses.push_back(tagList[i+1].left(tagList[i+1].indexOf('<')));
			}
		}

		qDebug() << "adresses " << adresses;
		download_covers( adresses, num, images);

	}


	curl_easy_cleanup(curl_lfm);
}



void search_covers_by_album_name(QString album, vector<QImage>& images, int num){
	QString url = Helper::calc_search_album_adress(album);
	get_alternative_album_covers(url, images, num);
}

void search_covers_by_artist_name(QString artist, vector<QImage>& images, int num){
	QString url = Helper::calc_search_artist_adress(artist);
	get_alternative_album_covers(url, images, num);
}



#endif /* COVERDOWNLOADER_H_ */
