/* LastFM.cpp */

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
#include <QList>
#include <QtWebKit/QWebView>


using namespace std;
extern size_t lfm_webpage_bytes;
extern char* lfm_webpage;

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

	_username = username;

	_logged_in = false;
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
		qDebug() << Q_FUNC_INFO << " Could not connect to " << url;

		lfm_wa_free_webpage();
		return;
	}

	_session_key = lfm_wa_parse_session_answer();

	if(_session_key.size() != 0) {
		_logged_in = true;
		emit last_fm_logged_in(true);
		qDebug() << Q_FUNC_INFO <<  "Logged in to LastFM";

	}
	else{
		qDebug() << Q_FUNC_INFO <<  "Session key error";
		emit last_fm_logged_in(false);

	}

	lfm_wa_free_webpage();


	QString url_handshake = QString("http://ws.audioscrobbler.com/radio/handshake.php?");

	url_handshake += QString("version=1.5") +
				QString("&platform=linux") +
				QString("&username=luciocarreras") +
				QString("&passwordmd5=") + password;

		qDebug() << "Curl perform";
		qDebug() << "url = " << url_handshake;

		CURL* curl = curl_easy_init();
		if(curl){
			curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
			curl_easy_setopt(curl, CURLOPT_URL, url_handshake.toLocal8Bit().data());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, lfm_wa_get_answer);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

		}
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + 1));
		lfm_webpage[lfm_webpage_bytes] = '\0';


		if(lfm_webpage_bytes == 0 || lfm_webpage == 0){
			qDebug() <<  Q_FUNC_INFO << "Webpage is null";

		}

		qDebug() << lfm_webpage;
		QString webpage = QString(lfm_webpage);
		webpage.replace("session=", "");
		_session_key2 = webpage.left(32);

		lfm_wa_free_webpage();
}




void LastFM::login_session(QString username, QString password){


	QString session_key = CSettingsStorage::getInstance()->getLastFMSessionKey();
	if(session_key.size() > 0){
		qDebug() << "Got session key " << session_key;
		_session_key = session_key;
		_logged_in = true;
		return;
	}

	_logged_in = false;

	QString url = 	QString("http://ws.audioscrobbler.com/2.0/?") +
					QString("method=auth.gettoken&") +
					QString("api_key=") + _api_key;

	bool success = true;
	success = lfm_wa_call_session_url(url);

	if(!success){
		qDebug() << Q_FUNC_INFO << " Cannot fetch token " << url;

		lfm_wa_free_webpage();
		return;
	}

	_auth_token = lfm_wa_parse_token_answer();

	qDebug() << "token = " << _auth_token;


	QString token_webpage = QString("firefox \"http://www.last.fm/api/auth/?api_key=") + _api_key + QString("&token=") + _auth_token + "\"";

	system(token_webpage.toLocal8Bit().data());




	success = false;

	long int max_time = 30000000;

	while(max_time > 0){

			qDebug() << "Got LFM token " << _auth_token;
			lfm_wa_free_webpage();

			_username = username;

			QString signature = QString("api_key") + _api_key +
							QString("method") + QString("auth.getsession") +
							QString("token") + _auth_token + _api_secret;

			signature = QCryptographicHash::hash(signature.toUtf8(), QCryptographicHash::Md5).toHex();

			url = 	QString("http://ws.audioscrobbler.com/2.0/?") +
								QString("method=auth.getsession&") +
								QString("token=") + _auth_token + QString("&") +
								QString("api_key=") + _api_key + QString("&") +
								QString("api_sig=") + signature;


			success = lfm_wa_call_session_url(url);

			if(!success){
				qDebug() << Q_FUNC_INFO << " Cannot get session " << url;

				lfm_wa_free_webpage();
				return;
			}

			_session_key = lfm_wa_parse_session_answer();

			if(_session_key.size() != 0) {
				_logged_in = true;
				emit last_fm_logged_in(true);
				qDebug() << Q_FUNC_INFO <<  "Logged in to LastFM";
				break;

			}
			else{
				_session_key = "";
				qDebug() << Q_FUNC_INFO <<  "Session key error";
				emit last_fm_logged_in(false);
			}

			usleep(1000000);
			max_time -= 1000000;

	}

	CSettingsStorage::getInstance()->setLastFMSessionKey(_session_key);

	lfm_wa_free_webpage();
}



void LastFM::login_slot(QString username, QString password){

	login(username, password);

}

void LastFM::scrobble(const MetaData& metadata){

	lfm_wa_free_webpage();

	if(!_logged_in){
		qDebug() << Q_FUNC_INFO <<  "Not logged in to LastFM!";
		return;
	}

	time_t rawtime;
	time(&rawtime);

	tm* ptm = gmtime( &rawtime );
	qDebug() << "Offset = " << ptm->tm_gmtoff;
	//ptm->tm_hour += 1;


	time_t started = mktime(ptm);
	if(!ptm->tm_isdst){ // if(no summertime)
		ptm->tm_hour += 1;
		started = mktime(ptm);
	}




	QString url = 	QString("http://ws.audioscrobbler.com/2.0/");


	string signature =// string("album") + metadata.album.toLocal8Bit().data() +
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
						//string("album=") + lfm_wa_get_url_enc(metadata.album) + string("&") +
						string("api_key=") + _api_key.toStdString() + string("&") +
						string("api_sig=") + signature_md5.toStdString() + string("&") +
						string("artist=") + metadata.artist.toLocal8Bit().data() + string("&") +
						string("duration=") + QString::number(metadata.length_ms / 1000).toStdString()  + string("&") +
						string("method=") + "track.scrobble" + string("&") +
						string("timestamp=") + QString::number(started).toStdString() + string("&") +
						string("track=") + metadata.title.toLocal8Bit().data() + string("&") +
						string("trackNumber=0") + string("&") +
						string("sk=") + _session_key.toStdString();


	lfm_wa_call_scrobble_url(url.toStdString(), post_data);
	lfm_wa_free_webpage();

}



void LastFM::update_track(const MetaData& metadata){


	lfm_wa_free_webpage();

	if(!_logged_in){
		qDebug() <<  Q_FUNC_INFO << "Not logged in to LastFM!";
		return;
	}

	QString url = 	QString("http://ws.audioscrobbler.com/2.0/");


	string signature =  string("api_key") + _api_key.toStdString() +
						string("artist") + metadata.artist.toLocal8Bit().data()+
						string("duration") + QString::number(metadata.length_ms / 1000).toStdString() +
						string("method") + "track.updatenowplaying" +
						string("sk") + _session_key.toStdString() +
						string("track") + metadata.title.toLocal8Bit().data() +
						string("trackNumber0") +
						_api_secret.toStdString();

	QString signature_md5 = QCryptographicHash::hash(signature.c_str(), QCryptographicHash::Md5).toHex();

	string post_data =  string("api_key=") + _api_key.toStdString() + string("&") +
						string("api_sig=") + signature_md5.toStdString() + string("&") +
						string("artist=") + metadata.artist.toLocal8Bit().data() + string("&") +
						string("duration=") + QString::number(metadata.length_ms / 1000).toStdString()  + string("&") +
						string("method=") + "track.updatenowplaying" + string("&") +
						string("track=") + metadata.title.toLocal8Bit().data() + string("&") +
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

	QList<int> ids = _similar_artists_thread->get_chosen_ids();
	if(ids.size() > 0)
		emit similar_artists_available(ids);
}

QString LastFM::get_api_key(){
	return _api_key;
}

void LastFM::radio_init(const QString& str, bool artist){

	QString url_radio_station = QString("http://ws.audioscrobbler.com/radio/adjust.php?");
	QString lfm_radio_station = QString("lastfm%3A%2F%2F");

	if(!artist)
		lfm_radio_station += QString("globaltags%2F") + QUrl::toPercentEncoding(str);
	else
		lfm_radio_station += QString("artist%2F") + QUrl::toPercentEncoding(str) + QString("%2Fsimilarartists");


	url_radio_station += QString("session=") + _session_key2 +
						QString("&lang=en") +
						QString("&url=") + lfm_radio_station;




	//lastfm://artist/cher/similarartists
	//lastfm://user/last.hq/recommended
	//lastfm://user/last.hq/library

	CURL* curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url_radio_station.toLocal8Bit().data());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, lfm_wa_get_answer);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

	}
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + 1));
	lfm_webpage[lfm_webpage_bytes] = '\0';


	if(lfm_webpage_bytes == 0 || lfm_webpage == 0){
		qDebug() <<  Q_FUNC_INFO << "Webpage is null";

	}

	lfm_wa_free_webpage();

	radio_get_playlist();
}

void LastFM::radio_get_playlist(){

	// get playlist
	QString url_tracks = QString("http://ws.audioscrobbler.com/radio/xspf.php?");
	url_tracks += QString("sk=") + _session_key2 +
				"&discovery=0" +
				"&desktop=1.5";

		qDebug() << "Curl perform";
		qDebug() << "url = " << url_tracks;

		CURL* curl_track = curl_easy_init();
		if(curl_track){
			curl_easy_setopt(curl_track, CURLOPT_NOSIGNAL, 1);
			curl_easy_setopt(curl_track, CURLOPT_URL, url_tracks.toLocal8Bit().data());
			curl_easy_setopt(curl_track, CURLOPT_WRITEFUNCTION, lfm_wa_get_answer);
			curl_easy_setopt(curl_track, CURLOPT_FOLLOWLOCATION, 1);

		}
		curl_easy_perform(curl_track);
		curl_easy_cleanup(curl_track);

		lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + 1));
		lfm_webpage[lfm_webpage_bytes] = '\0';


		if(lfm_webpage_bytes == 0 || lfm_webpage == 0){
			qDebug() <<  Q_FUNC_INFO << "Webpage is null";
		}

		vector<MetaData> v_md;
		parse_playlist_answer(v_md, QString(lfm_webpage));
		lfm_wa_free_webpage();

		qDebug() << "Got " << v_md.size() << " tracks";
		if(v_md.size() > 0){
			emit new_radio_playlist(v_md);
		}
}


void LastFM::parse_playlist_answer(vector<MetaData>& v_md, const QString& xml){

	v_md.clear();
	QDomDocument doc("radio");
	doc.setContent(xml, false);
	QDomElement docElement = doc.documentElement();
	QDomNode tracklist = docElement.firstChildElement("trackList");

	if(!tracklist.hasChildNodes())return;

		for(int idx_track=0; idx_track < tracklist.childNodes().size(); idx_track++){

			QDomNode track = tracklist.childNodes().item(idx_track);
			if(!track.hasChildNodes()) continue;

			MetaData md;

			for(int idx_track_content = 0; idx_track_content <track.childNodes().size(); idx_track_content++){

				md.is_extern = true;
				md.bitrate = 128000;

				QDomNode content = track.childNodes().item(idx_track_content);
				QString nodename = content.nodeName().toLower();
				QDomElement e = content.toElement();
				if(!nodename.compare("location")){
					md.filepath = e.text();
					qDebug() << md.filepath;
				}

				else if(!nodename.compare("title")){
					md.title = e.text();
					qDebug() << md.title;
				}

				else if(!nodename.compare("album")){
					md.album = e.text();
					qDebug() << md.album;
				}

				else if(!nodename.compare("creator")){
					md.artist = e.text();
					qDebug() << md.artist;
				}

				else if(!nodename.compare("duration")){
					md.length_ms = e.text().toLong();
					qDebug() << md.length_ms;
				}

			}

			v_md.push_back(md);
		}

}
