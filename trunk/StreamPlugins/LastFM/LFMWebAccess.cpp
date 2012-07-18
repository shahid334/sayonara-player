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
#include "StreamPlugins/LastFM/LFMGlobals.h"
#include "StreamPlugins/LastFM/LFMWebAccess.h"


#include <QString>
#include <QDebug>
#include <QCryptographicHash>
#include <qdom.h>

#include <curl/curl.h>

#include <string>
#include <stdlib.h>
#include <ctime>

using namespace std;

static size_t lfm_webpage_bytes;
static char* lfm_webpage;


static void lfm_wa_free_webpage(){

	if(lfm_webpage != 0){
		free(lfm_webpage);
		lfm_webpage = 0;
	}
	lfm_webpage_bytes = 0;

}



void lfm_wa_init(){
	lfm_webpage = 0;
	lfm_webpage_bytes = 0;
}

/*
const char* lfm_wa_get_url_enc(QString str){

	const char* c_s = str.toLocal8Bit().data();

	CURL *curl = curl_easy_init();
	return curl_easy_escape(curl, c_s, 0);
}
*/


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

bool lfm_wa_call_url(const QString& url, QString& response){

	lfm_wa_free_webpage();

	CURL *curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.toLocal8Bit().data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, lfm_wa_get_answer);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 2000);

		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + 1));
	lfm_webpage[lfm_webpage_bytes] = '\0';

	if(lfm_webpage_bytes > 0){
		response = QString::fromUtf8(lfm_webpage, lfm_webpage_bytes + 1);
		lfm_wa_free_webpage();
		return true;
	}

	else {
		lfm_wa_free_webpage();
		return false;
	}
}

bool lfm_wa_call_post_url(const QString& url, string post_data){
	QString response;
	return lfm_wa_call_post_url(url, post_data, response);

}

bool lfm_wa_call_post_url(const QString& url, const string& post_data, QString& response){

	response.clear();

	lfm_wa_free_webpage();

	CURL* curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.toLocal8Bit().data());
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
		response = QString::fromUtf8(lfm_webpage, lfm_webpage_bytes + 1);
		lfm_wa_free_webpage();
		return true;
	}

	else {
		lfm_wa_free_webpage();
		qDebug() <<  "LFM: Webpage = null";
		return false;
	}
}


bool lfm_wa_call_url_xml(const QString& url, QDomDocument& doc){

	lfm_wa_free_webpage();

	CURL *curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.toLocal8Bit().data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, lfm_wa_get_answer);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000);

		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
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
		QString xmlString = QString::fromUtf8(lfm_webpage, lfm_webpage_bytes + 1);
		doc.setContent(xmlString, false);
		lfm_wa_free_webpage();

		return true;
	}

	else {
		lfm_wa_free_webpage();
		qDebug() <<  "LFM: webpage is null " << url;
		return false;
	}
}


QString lfm_wa_parse_session_answer(const QString& content){

	QString str_key = "";
	QDomDocument doc("answer");
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




QString lfm_wa_create_signature(const UrlParams& data){

	QString signature;

	for(UrlParams::const_iterator it=data.begin(); it != data.end(); it++){
		signature += it.key();
		signature += it.value();
	}

	signature += LFM_API_SECRET;
	return QCryptographicHash::hash(signature.toUtf8(), QCryptographicHash::Md5).toHex();
}


QString lfm_wa_create_std_url(const QString& base_url, const UrlParams& data){
	string post_data;
	QString url = lfm_wa_create_std_url_post(base_url, data, post_data);
	return QString(url + QString("?") + post_data.c_str());
}

QString lfm_wa_create_std_url_post(const QString& base_url, const UrlParams& data, string& post_data){
	post_data = "";
	QString url = base_url;


	post_data.clear();

	for(UrlParams::const_iterator it=data.begin(); it != data.end(); it++){

		post_data += string(it.key().toLocal8Bit().data()) + string("=");
		post_data += string(it.value().toLocal8Bit().replace("&", "%26").data()) + string("&");
	}

	// remove the last &
	post_data = post_data.substr(0, post_data.size() -1);

	return url;
}


QString lfm_wa_create_sig_url(const QString& base_url, const UrlParams& sig_data){
	string post_data;
	QString url = lfm_wa_create_sig_url_post(base_url, sig_data, post_data);
	return QString(url + QString("?") + post_data.c_str());
}


QString lfm_wa_create_sig_url_post(const QString& base_url, const UrlParams& sig_data, string& post_data){

	post_data.clear();

	QString signature;
	signature = lfm_wa_create_signature(sig_data);

	QString url = base_url;

	UrlParams data_copy = sig_data;
	data_copy["api_sig"] = signature;
	QString session_key;

	for(UrlParams::iterator it=data_copy.begin(); it != data_copy.end(); it++){

		post_data += string(it.key().toLocal8Bit().data()) + string("=");
		post_data += string(it.value().replace("&", "%26").toLocal8Bit().data()) + string("&");
	}

	post_data = post_data.substr(0, post_data.size() -1);

	return url;
}


