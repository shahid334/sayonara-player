/*
 * LyricLookup.cpp
 *
 *  Created on: May 21, 2011
 *      Author: luke
 */

#include "LyricLookup/LyricLookup.h"
#include "HelperStructs/Helper.h"
#include <QString>
#include <QDebug>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <QRegExp>
#include <QStringList>

using namespace std;

char* webpage_ll = 0;
size_t webpage_ll_bytes = 0;
size_t first_appearance = -1;
size_t last_appearance = -1;
bool b_save;
QStringList lst;



int find_first_of(char* arr, size_t len, QString str){
	if(len <= 0) return -1;
	for(size_t i=0; i<len; i++){


		bool matched = false;
		for(uint j=0; j<str.length(); j++, i++){

			if(arr[i] != str.at(j).toAscii()) break;

			if(j == str.length() - 1){
				matched = true;
				qDebug() << "matched at " << i;
				break;
			}
		}

		if(matched) return i - str.length();
	}

	return -1;
}


int find_last_of(char* arr, size_t len, QString str){

	if(len <= 0) return -1;

	size_t off_back = 0;
	for(size_t i=len-1; i>=0; i--){

			bool matched = false;
			for(uint j=str.length()-1; j>=0; j--, i--, off_back++){

				if(arr[i] != str.at(j).toAscii()) break;

				if(j == 0){
					matched = true;
					qDebug() << "matched at " << off_back;
					break;
				}
			}

			if(matched) return off_back;
		}

		return -1;
}


size_t get_content_ll( void *ptr, size_t size, size_t nmemb, FILE *userdata){
	char* cptr = (char*) ptr;




	QString tmp = "";
	QRegExp rx("&#(\\d+);|<br");
	for(int i=0; i<size * nmemb; i++){
		tmp += cptr[i];
	}


	//rx.indexIn(tmp);
	QStringList tmplist;;
	 int pos = 0;
	 while ((pos = rx.indexIn(tmp, pos)) != -1) {
	     tmplist.append(rx.cap(1));
	     pos += rx.matchedLength();
	 }

	 if(tmplist.size() > 0)
		 lst.append(tmplist);

	//qDebug() << tmplist;





	fwrite(ptr, size, nmemb, userdata);


	return size * nmemb;
}



LyricLookup::LyricLookup() {
	// TODO Auto-generated constructor stub

}

LyricLookup::~LyricLookup() {
	// TODO Auto-generated destructor stub
}

void LyricLookup::find_lyrics(QString artist, QString song){
		b_save = true;
		webpage_ll = 0;
		webpage_ll_bytes = 0;

		QString url = "http://lyrics.wikia.com/" +
					artist.replace(" ", "_") + ":" +
					song.replace(" ", "_");

		qDebug() << "Url = " << url;

		CURL *curl;
		curl = curl_easy_init();

		if(curl){
			FILE* bla = fopen("/home/luke/file.txt", "w");
			curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_content_ll);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, bla);
			//curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 100);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			fclose(bla);
		}
		qDebug() <<lst;
		for(int i=0; i<lst.size(); i++){
			int n= lst[i].toInt();
			QChar c = (QChar) n;

			if(lst[i] == "" ) c='\n';

			cout << c.toAscii() << flush;
		}

}
