/*
 * LastFM.cpp
 *
 *  Created on: Apr 19, 2011
 *      Author: luke
 */




#include "HelperStructs/MetaData.h"
#include "HelperStructs/CSettingsStorage.h"
#include "LastFM/LastFM.h"
#include "LastFM/LFMSimilarArtistsThread.h"
#include "LastFM/LFMWebAccess.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <iostream>
#include <curl/curl.h>

#include <string>
#include <ctime>

#include <QObject>
#include <QDebug>
#include <QDomDocument>
#include <QCryptographicHash>
#include <QUrl>
#include <QtXml>


using namespace std;


LastFM::LastFM() {


	_api_key = QString("51d6f9eaef806f603f346844bef326ba");
	_api_secret = QString("1093d769e54858cb0d21d42b35a8f603");

	_logged_in = false;

	_similar_artists_thread = new LFM_SimilarArtists(_api_key);
	connect(_similar_artists_thread, SIGNAL(finished()), this, SLOT(sim_artists_thread_finished()));
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


	bool success = lfm_wa_call_session_url(url);

	if(!success){
		qDebug() << "Could not connect to " << url;

		lfm_wa_free_webpage();
		return;
	}


	QString session_key = lfm_wa_parse_session_answer();
	_session_key = session_key;


	if(_session_key.size() != 0) {
		_logged_in = true;
		qDebug() << "Logged in to LastFM";

	}
	else{
		qDebug() << "Session key error";

	}

	lfm_wa_free_webpage();
}


void LastFM::login_slot(QString username, QString password){

	login(username, password);

}

void LastFM::scrobble(const MetaData& metadata){

	lfm_wa_free_webpage();

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
						string("album=") + lfm_wa_get_url_enc(metadata.album) + string("&") +
						string("api_key=") + _api_key.toStdString() + string("&") +
						string("api_sig=") + signature_md5.toStdString() + string("&") +
						string("artist=") + lfm_wa_get_url_enc(metadata.artist) + string("&") +
						string("duration=") + QString::number(metadata.length_ms / 1000).toStdString()  + string("&") +
						string("method=") + "track.scrobble" + string("&") +
						string("timestamp=") + QString::number(started).toStdString() + string("&") +
						string("track=") + lfm_wa_get_url_enc(metadata.title) + string("&") +
						string("trackNumber=0") + string("&") +
						string("sk=") + _session_key.toStdString();


	lfm_wa_call_scrobble_url(url.toStdString(), post_data);

	/*if(QString(lfm_webpage).contains("\"ok\""))
		qDebug() << "scrobbled";
	else
		qDebug() << "scrobbling not successful";*/

	lfm_wa_free_webpage();

}



void LastFM::update_track(const MetaData& metadata){


	lfm_wa_free_webpage();

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
							string("album=") + lfm_wa_get_url_enc(metadata.album) + string("&") +
							string("api_key=") + _api_key.toStdString() + string("&") +
							string("api_sig=") + signature_md5.toStdString() + string("&") +
							string("artist=") + lfm_wa_get_url_enc(metadata.artist) + string("&") +
							string("duration=") + QString::number(metadata.length_ms / 1000).toStdString()  + string("&") +
							string("method=") + "track.updatenowplaying" + string("&") +
							string("track=") + lfm_wa_get_url_enc(metadata.title) + string("&") +
							string("trackNumber=0") + string("&") +
							string("sk=") + _session_key.toStdString();


	lfm_wa_call_scrobble_url(url.toStdString(), post_data);

	lfm_wa_free_webpage();
}


void LastFM::get_similar_artists(const QString& artistname){

	_similar_artists_thread->set_artist_name(artistname);
	_similar_artists_thread->start();

}

void LastFM::sim_artists_thread_finished(){

	if(_similar_artists_thread->get_chosen_id() != -1){
		emit similar_artists_available(_similar_artists_thread->get_chosen_id());
	}

}

QString LastFM::get_api_key(){
	return _api_key;
}
