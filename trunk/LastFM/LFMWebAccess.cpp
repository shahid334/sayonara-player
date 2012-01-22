/* LFMWebAccess.cpp */

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
 * LFMWebAccess.cpp
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */
#include "LastFM/LFMWebAccess.h"

#include <QString>
#include <QDebug>
#include <qdom.h>

#include <curl/curl.h>

#include <string>
#include <stdlib.h>
#include <ctime>

using namespace std;

size_t lfm_webpage_bytes;
char* lfm_webpage;


void lfm_wa_init(){
	lfm_webpage = 0;
	lfm_webpage_bytes = 0;
}


void lfm_wa_free_webpage(){

	if(lfm_webpage != 0){
		free(lfm_webpage);
		lfm_webpage = 0;
	}
	lfm_webpage_bytes = 0;

}


const char* lfm_wa_get_url_enc(QString str){

	const char* c_s = str.toLocal8Bit().data();

	CURL *curl = curl_easy_init();
	return curl_easy_escape(curl, c_s, 0);

}



size_t lfm_wa_get_answer( void *ptr, size_t size, size_t nmemb, FILE *userdata){

	(void) userdata;

	if(lfm_webpage_bytes == 0){
		lfm_webpage_bytes = size * nmemb;
		lfm_webpage = (char*) (malloc(lfm_webpage_bytes));
		memcpy ( lfm_webpage, ptr, lfm_webpage_bytes );
	}

	else {
		lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + size * nmemb));
		memcpy(lfm_webpage + lfm_webpage_bytes, ptr, size*nmemb);
		lfm_webpage_bytes += size * nmemb;
	}

	return size * nmemb;
}




bool lfm_wa_call_session_url(QString url){

	CURL *curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, lfm_wa_get_answer);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000);

		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + 1));
	lfm_webpage[lfm_webpage_bytes] = '\0';

	if(lfm_webpage_bytes > 0)
		return true;
	else return false;
}


bool lfm_wa_call_scrobble_url(string url, string post_data){

	CURL* curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1) ;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, lfm_wa_get_answer);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());

	}
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + 1));
	lfm_webpage[lfm_webpage_bytes] = '\0';


	if(lfm_webpage_bytes > 0){
		return true;
	}

	else {

		qDebug() <<  Q_FUNC_INFO << "Webpage = null";
		return false;
	}
}


bool lfm_wa_call_lfm_url(QString url, QDomDocument& doc){

	CURL *curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, lfm_wa_get_answer);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000);

		curl_easy_perform(curl);

	}

	long int t = 5000000;
	while(lfm_webpage_bytes == 0){
#ifdef Q_OS_WIN
		Sleep(100000);
#else
		usleep(100000);
#endif
	
		t -= 100000;
		if( t <= 0) break;
	}


	lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + 1));
	lfm_webpage[lfm_webpage_bytes] = '\0';

	if(lfm_webpage_bytes > 0){
		QString xmlString = QString(lfm_webpage);
		doc.setContent(xmlString, false);

		lfm_wa_free_webpage();

		curl_easy_cleanup(curl);

		return true;

	}

	else {
		lfm_wa_free_webpage();
		qDebug() <<  Q_FUNC_INFO << "webpage is null";
		curl_easy_cleanup(curl);
		return false;
	}

}


QString lfm_wa_parse_session_answer(){

	QString str_key = "";
	QDomDocument doc("answer");
	QString content = QString(lfm_webpage);
	doc.setContent(content);

	QDomNodeList nodeList =  doc.documentElement().elementsByTagName("session");

	for(int i=0; i<nodeList.size(); i++){
		QDomNodeList nl_session = nodeList.at(i).toElement().elementsByTagName("key");
		bool found = false;

		for(int j=0; j<nl_session.size(); j++){
			str_key = nl_session.at(j).toElement().text();
			if(str_key.size() != 0){
				found = true;
				break;
			}
		}

		if(found) break;
	}

	doc.clear();
	return str_key;
}



QString lfm_wa_parse_token_answer(){

	QString str_key = "";
	QDomDocument doc("answer");
	QString content = QString(lfm_webpage);
	doc.setContent(content);

	QDomNodeList nodeList =  doc.documentElement().elementsByTagName("token");

	for(int i=0; i<nodeList.size(); i++){
		str_key = nodeList.at(i).toElement().text();
	}

	doc.clear();
	return str_key;
}


