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
#include "HelperStructs/globals.h"
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


using namespace std;


LastFM* LastFM::getInstance(){
	static LastFM inst;
	return &inst;

}

LastFM::LastFM() {

	init();
}

void LastFM::init(){
	lfm_wa_init();


	_logged_in = false;

	_similar_artists_thread = new LFM_SimilarArtists(LFM_API_KEY);
	connect(_similar_artists_thread, SIGNAL(finished()), this, SLOT(sim_artists_thread_finished()));
}

LastFM::~LastFM() {

}

QString LastFM::get_api_key(){
	return LFM_API_KEY;
}

QString LastFM::create_signature(const UrlParams& data){

	QString signature;

	for(UrlParams::const_iterator it=data.begin(); it != data.end(); it++){
		signature += it.key();
		signature += it.value();
	}

	signature += LFM_API_SECRET;
	return QCryptographicHash::hash(signature.toUtf8(), QCryptographicHash::Md5).toHex();
}


QString LastFM::create_std_url(const QString& base_url, const UrlParams& data){
	string post_data;
	QString url = create_std_url_post(base_url, data, post_data);
	return QString(url + QString("?") + post_data.c_str());
}

QString LastFM::create_std_url_post(const QString& base_url, const UrlParams& data, string& post_data){
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


QString LastFM::create_sig_url(const QString& base_url, const UrlParams& sig_data){
	string post_data;
	QString url = create_sig_url_post(base_url, sig_data, post_data);
	return QString(url + QString("?") + post_data.c_str());
}

QString LastFM::create_sig_url_post(const QString& base_url, const UrlParams& sig_data, string& post_data){

	post_data.clear();

	QString signature;
	signature = create_signature(sig_data);

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


bool LastFM::check_login(){
	if(!_logged_in || _session_key.size() != 32){

		QString username, password;
		CSettingsStorage::getInstance()->getLastFMNameAndPW(username, password);

		if(!username.isEmpty() && !password.isEmpty())
			login(username, password);

		if(!_logged_in || _session_key.size() != 32){
			return false;
		}

		return true;
	}

	else return true;
}

bool LastFM::login(QString username, QString password){

	_username = username;
	_logged_in = false;
	_auth_token = QCryptographicHash::hash(username.toUtf8() + password.toUtf8(), QCryptographicHash::Md5).toHex();

	UrlParams signature_data;
		signature_data["api_key"] = LFM_API_KEY;
		signature_data["authToken"] = _auth_token;
		signature_data["method"] = QString("auth.getmobilesession");
		signature_data["username"] = _username;

	QString url = create_sig_url(QString("http://ws.audioscrobbler.com/2.0/"), signature_data);

	QString response;
	bool success = lfm_wa_call_url(url, response);

	if(!success){
		qDebug() << " Could not connect to " << url;
		return false;
	}

	_session_key = lfm_wa_parse_session_answer(response);

	if(_session_key.size() != 0) {
		_logged_in = true;
		qDebug() << "LFM: Logged in to LastFM";
	}

	else{
		qDebug() << "LFM: Session key error";
		qDebug() << response;
		return false;
	}

	UrlParams handshake_data;
		handshake_data["version"] = QString::number(1.5);
		handshake_data["platform"] = QString("linux");
		handshake_data["username"] = _username.toLower();
		handshake_data["passwordmd5"] = password;

	QString url_handshake = create_std_url("http://ws.audioscrobbler.com/radio/handshake.php", handshake_data);
	QString resp_handshake;

	success = lfm_wa_call_url(url_handshake, resp_handshake);
	if( !success ){
		_session_key2 = "";
		qDebug() << "LFM: Handshake was not successful";
		qDebug() << "LFM: url = " << url_handshake;
		qDebug() << "LFM: " << resp_handshake;
		return false;
	}

	resp_handshake.replace("session=", "");
	_session_key2 = resp_handshake.left(32);

	return true;
}






bool LastFM::update_track(const MetaData& metadata){

	if(!check_login()){
		return false;
	}

	QString artist = metadata.artist;
	QString title = metadata.title;

	UrlParams sig_data;
		sig_data["api_key"] = LFM_API_KEY;
		sig_data["artist"] = artist;
		sig_data["duration"] = QString::number(metadata.length_ms / 1000);
		sig_data["method"] = QString("track.updatenowplaying").toLocal8Bit();
		sig_data["sk"] = _session_key;
		sig_data["track"] =  title;


	string post_data;
	QString url = create_sig_url_post(QString("http://ws.audioscrobbler.com/2.0/"), sig_data, post_data);
	QString response;

	bool success = lfm_wa_call_post_url(url, post_data, response);
	if(!success || response.contains("failed") ){
		/*qDebug() << "Track cannot be updated";
		qDebug() << "url = " << url;
		qDebug() << "post = " << post_data.c_str();
		qDebug() << response;*/
		return false;

	}

	return true;

}


bool LastFM::scrobble(const MetaData& metadata){

	if(!check_login())	return false;

	time_t rawtime;
	time(&rawtime);

	tm* ptm = gmtime( &rawtime );

	time_t started = mktime(ptm);
	if(!ptm->tm_isdst){ // if(no summertime)
		ptm->tm_hour += 1;
		started = mktime(ptm);
	}

	QString artist = metadata.artist;
	QString title = metadata.title;

	UrlParams sig_data;
		sig_data["api_key"] = LFM_API_KEY;
		sig_data["artist"] = artist;
		sig_data["duration"] = QString::number(metadata.length_ms / 1000);
		sig_data["method"] = QString("track.scrobble");
		sig_data["sk"] = _session_key;
		sig_data["timestamp"] = QString::number((uint)started);
		sig_data["track"] = title;

	string post_data;
	QString url = create_sig_url_post(QString("http://ws.audioscrobbler.com/2.0/"), sig_data, post_data);
	QString response;

	bool success = lfm_wa_call_post_url(url, post_data, response);
	if(!success || response.contains("failed")){
		return false;
		/*qDebug() << "Track scrobbling failed";
		qDebug() << "Url = " << url;
		qDebug() << "Post data = " << post_data.c_str();
		qDebug() << response;*/
	}

	return true;
}




bool LastFM::get_similar_artists(const QString& artistname, QList<int>& artist_ids){


	_similar_artists_thread->set_artist_name(artistname);
	_similar_artists_thread->start();

	long int max = 3000000; // 3 secs

	while(!_similar_artists_thread->isFinished() && max > 0){
		usleep(10000); // 0.1 sec
		max -= 10000;
	}

	if(max <= 0) return false;

	artist_ids.clear();
	_similar_artists_thread->get_chosen_ids();
	return true;
}



bool LastFM::radio_init(const QString& str, int radio_mode){

	if(_session_key2.size() != 32){
		if(!check_login()){
			return false;
		}

		else if(_session_key2.size() != 32){
			return false;
		}
	}

	QString lfm_radio_station = QString("lastfm://");
	QString tag_string = str;

	tag_string.replace("&", "and");

	switch(radio_mode){
		case LFM_RADIO_MODE_ARTIST:
			lfm_radio_station += QString("artist/") + str + QString("/similarartists");
			break;
		case LFM_RADIO_MODE_TAG:
			lfm_radio_station += QString("globaltags/") + str;
			break;
		default:
			break;

	}


	UrlParams data;
		data["session"] = _session_key2;
		data["lang"] = QString("en");
		data["url"] = QUrl::toPercentEncoding( lfm_radio_station );

	QString url = create_std_url( QString("http://ws.audioscrobbler.com/radio/adjust.php"), data );
	QString response;
	bool success = lfm_wa_call_url(url, response);
	if( !success ){
		qDebug() << "LFM: Radio not initialized";
		qDebug() << "LFM: url = " << url;
		qDebug() << "LFM: " << response;
		return false;
	}

	return true;
}


bool LastFM::radio_get_playlist(vector<MetaData>& v_md){

	if(_session_key2.size() != 32){
		if(!check_login()){
			return false;
		}

		else if(_session_key2.size() != 32){
			return false;
		}
	}

	UrlParams data;
	data["sk"] = _session_key2;
	data["discovery"] = "0";
	data["desktop"] = "1.5";

	QString url = create_std_url(QString("http://ws.audioscrobbler.com/radio/xspf.php"), data);
	QDomDocument xml_response;

	bool success = lfm_wa_call_url_xml(url, xml_response);
	if( !success ){
		qDebug() << "LFM: Cannot get playlist";
		qDebug() << "LFM: Url = " << url;
		return false;
	}


	parse_playlist_answer(v_md, xml_response);
	xml_response.clear();

	if(v_md.size() > 0){
		return true;
	}

	return false;
}


bool LastFM::parse_playlist_answer(vector<MetaData>& v_md, const QDomDocument& doc){

	v_md.clear();

	QDomElement docElement = doc.documentElement();
	QDomNode tracklist = docElement.firstChildElement("trackList");

	if(!tracklist.hasChildNodes()) return false;

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
			}

			else if(!nodename.compare("title")){
				md.title = e.text();
			}

			else if(!nodename.compare("album")){
				md.album = e.text();
			}

			else if(!nodename.compare("creator")){
				md.artist = e.text();
			}

			else if(!nodename.compare("duration")){
				md.length_ms = e.text().toLong();
			}

		}

		v_md.push_back(md);
	}

	return (v_md.size() > 0);
}


QString LastFM::getArtistInfo(const QString& artist){
	QString retval;

	if(!_logged_in) {
		qDebug() << "not logged in";
		return "";
	}

	UrlParams params;
	params["artist"] = QUrl::toPercentEncoding(artist);
	params["username"] = _username;
	params["method"] = QString("artist.getinfo");
	params["api_key"] = LFM_API_KEY;


	QString url_getArtistInfo = create_std_url("http://ws.audioscrobbler.com/2.0/", params);

	bool success = lfm_wa_call_url(url_getArtistInfo, retval);
	if(!success) {
		return "";
	}

	QString str2search = QString("<userplaycount>");
	int idx = retval.indexOf(str2search);
	idx += str2search.size();
	QString playcount = "";
	for(int i=idx; retval.at(i).isDigit(); i++){
		playcount += retval.at(i);
	}

	return playcount;

}

QString LastFM::getAlbumInfo(const QString& artist, const QString& album){
	QString retval;
	return retval;
}

QString LastFM::getTrackInfo(const QString& artist, const QString& title){
	QString retval;
	return retval;

}


