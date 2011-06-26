/*
 * LastFM.cpp
 *
 *  Created on: Apr 19, 2011
 *      Author: luke
 */




#include "HelperStructs/MetaData.h"
#include "HelperStructs/CSettingsStorage.h"
#include "LastFM/LastFM.h"

#include <iostream>
#include <curl/curl.h>

#include <string>
#include <ctime>

#include <QObject>
#include <QDebug>
#include <QDomDocument>
#include <QCryptographicHash>


using namespace std;

size_t lfm_webpage_bytes = 0;
char* lfm_webpage = 0;

void free_webpage(){

	if(lfm_webpage != 0){
		free(lfm_webpage);
		lfm_webpage = 0;
	}
	lfm_webpage_bytes = 0;

}


const char* get_url_enc(QString str){

	const char* c_s = str.toLocal8Bit().data();

	CURL *curl = curl_easy_init();
	return curl_easy_escape(curl, c_s, 0);

}



size_t get_answer( void *ptr, size_t size, size_t nmemb, FILE *userdata){

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




bool call_session_url(QString url){

	CURL *curl = curl_easy_init();

	if(curl) {

		curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_answer);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000);

		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + 1));
	lfm_webpage[lfm_webpage_bytes] = '\0';

	//qDebug() << lfm_webpage << endl;
	if(lfm_webpage_bytes > 0)
		return true;
	else return false;
}


bool call_scrobble_url(string url, string post_data){

	CURL* curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1) ;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_answer);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());

	}
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + 1));
	lfm_webpage[lfm_webpage_bytes] = '\0';


	if(lfm_webpage_bytes > 0)
		return true;

	else return false;



}


QString LastFM::parse_session_answer(){

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



LastFM::LastFM() {
	_api_key = QString("51d6f9eaef806f603f346844bef326ba");
	_api_secret = QString("1093d769e54858cb0d21d42b35a8f603");

	_logged_in = false;
}

LastFM::~LastFM() {

}

QString LastFM::create_signature(QString fn_name){

	QString str = 	QString("api_key") + _api_key +
					QString("authToken") + _auth_token +
					QString("method") + fn_name +
					QString("username") + _username +
					_api_secret;

	QString signature = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5).toHex();
	return signature;

}

void LastFM::login(QString username, QString password){

	_logged_in = false;
	_username = username;
	_auth_token = QCryptographicHash::hash(username.toUtf8() + password.toUtf8(), QCryptographicHash::Md5).toHex();

	QString signature = create_signature(QString("auth.getmobilesession"));

	QString url = 	QString("http://ws.audioscrobbler.com/2.0/?") +
					QString("method=auth.getmobilesession&") +
					QString("username=") + _username + QString("&") +
					QString("authToken=") + _auth_token + QString("&") +
					QString("api_key=") + _api_key + QString("&") +
					QString("api_sig=") + signature;


	bool success = call_session_url(url);

	if(!success){
		qDebug() << "Could not connect to " << url;
		qDebug() << lfm_webpage;
		free_webpage();
		return;
	}


	QString session_key = parse_session_answer();
	_session_key = session_key;


	if(_session_key.size() != 0) {
		_logged_in = true;
		qDebug() << "Logged in to LastFM";

	}
	else{
		qDebug() << "Session key error";
		qDebug() << lfm_webpage;
	}

   	free_webpage();
}


void LastFM::login_slot(QString username, QString password){

	login(username, password);

}

void LastFM::scrobble(const MetaData& metadata){

	free_webpage();

	if(!_logged_in){
		qDebug() << "Not logged in to LastFM!";
		return;
	}

	time_t rawtime;
	time(&rawtime);

	tm* ptm = gmtime( &rawtime );


	time_t started = mktime(ptm);
	if(ptm->tm_isdst){
		ptm->tm_hour += 1;
		started = mktime(ptm);
	}

	QString url = 	QString("http://ws.audioscrobbler.com/2.0/");


	string signature = string("album") + metadata.album.toLocal8Bit().data() +
							string("api_key") + _api_key.toStdString() +
							string("artist") + metadata.artist.toLocal8Bit().data() +
							string("duration") + QString::number(metadata.length_ms / 1000).toStdString() +
							string("method") + "track.scrobble" +
							string("sk") + _session_key.toStdString() +
							string("timestamp") + QString::number((uint)started).toStdString() +
							string("track") + metadata.title.toLocal8Bit().data() +
							string("trackNumber0") +
							_api_secret.toStdString();

		QString signature_md5 = QCryptographicHash::hash(signature.c_str(), QCryptographicHash::Md5).toHex();



		string post_data =
						string("album=") + get_url_enc(metadata.album) + string("&") +
						string("api_key=") + _api_key.toStdString() + string("&") +
						string("api_sig=") + signature_md5.toStdString() + string("&") +
						string("artist=") + get_url_enc(metadata.artist) + string("&") +
						string("duration=") + QString::number(metadata.length_ms / 1000).toStdString()  + string("&") +
						string("method=") + "track.scrobble" + string("&") +
						string("timestamp=") + QString::number(started).toStdString() + string("&") +
						string("track=") + get_url_enc(metadata.title) + string("&") +
						string("trackNumber=0") + string("&") +
						string("sk=") + _session_key.toStdString();


	call_scrobble_url(url.toStdString(), post_data);

	/*if(QString(lfm_webpage).contains("\"ok\""))
		qDebug() << "scrobbled";
	else
		qDebug() << "scrobbling not successful";*/

	free_webpage();

}



void LastFM::update_track(const MetaData& metadata){

	free_webpage();

	if(!_logged_in){
		qDebug() << "Not logged in to LastFM!";
		return;
	}



	QString url = 	QString("http://ws.audioscrobbler.com/2.0/");


	string signature = string("album") + metadata.album.toLocal8Bit().data() +
								string("api_key") + _api_key.toStdString() +
								string("artist") + metadata.artist.toLocal8Bit().data()+
								string("duration") + QString::number(metadata.length_ms / 1000).toStdString() +
								string("method") + "track.updatenowplaying" +
								string("sk") + _session_key.toStdString() +
								string("track") + metadata.title.toLocal8Bit().data() +
								string("trackNumber0") +
								_api_secret.toStdString();

	QString signature_md5 = QCryptographicHash::hash(signature.c_str(), QCryptographicHash::Md5).toHex();




	string post_data =
							string("album=") + get_url_enc(metadata.album) + string("&") +
							string("api_key=") + _api_key.toStdString() + string("&") +
							string("api_sig=") + signature_md5.toStdString() + string("&") +
							string("artist=") + get_url_enc(metadata.artist) + string("&") +
							string("duration=") + QString::number(metadata.length_ms / 1000).toStdString()  + string("&") +
							string("method=") + "track.updatenowplaying" + string("&") +
							string("track=") + get_url_enc(metadata.title) + string("&") +
							string("trackNumber=0") + string("&") +
							string("sk=") + _session_key.toStdString();


	call_scrobble_url(url.toStdString(), post_data);
/*
	qDebug() << lfm_webpage;

	if(QString(lfm_webpage).contains("\"ok\""))
		qDebug() << "updated";
	else{
		qDebug() << "error when updating";
	}
*/
	free_webpage();


}
