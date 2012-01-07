/* LyricLookup.cpp */

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

size_t first_appearance = -1;
size_t last_appearance = -1;
bool b_save;
QStringList lst;

int find_first_of(char* arr, size_t len, QString str){
	if(len <= 0) return -1;
	for(size_t i=0; i<len; i++){

		bool matched = false;
		for(int j=0; j<str.length(); j++, i++){

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
	for(size_t i=len-1; i>0; i--){

			bool matched = false;
			for(uint j=str.length()-1; j>0; j--, i--, off_back++){

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

	(void) userdata;

	char* cptr = (char*) ptr;

	QString tmp = "";
	QRegExp rx("&#(\\d+);|<br");
	for(uint i=0; i<size * nmemb; i++){
		tmp += cptr[i];
	}

	QStringList tmplist;;
	 int pos = 0;
	 while ((pos = rx.indexIn(tmp, pos)) != -1) {
	     tmplist.append(rx.cap(1));
	     pos += rx.matchedLength();
	 }

	 if(tmplist.size() > 0)
		 lst.append(tmplist);

	return size * nmemb;
}



LyricLookup::LyricLookup() {
	// TODO Auto-generated constructor stub

}

LyricLookup::~LyricLookup() {
	// TODO Auto-generated destructor stub
}

QString LyricLookup::find_lyrics(QString artist, QString song){
		b_save = true;

		QString url = "http://lyrics.wikia.com/" +
					artist.replace(" ", "_") + ":" +
					song.replace(" ", "_");

		qDebug() << "Url = " << url;

		CURL *curl;
		curl = curl_easy_init();

		if(curl){
			curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_content_ll);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}

		QString str;
		for(int i=0; i<lst.size(); i++){
			int n= lst[i].toInt();
			QChar c = (QChar) n;

			if(lst[i] == "" ) {
				str.append("<br />");
			}

			else str.append(c);
		}

		str = str.trimmed();
		lst.clear();

		return str;


}
