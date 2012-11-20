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
#include "HelperStructs/Helper.h"
#include "HelperStructs/globals.h"
#include "StreamPlugins/LastFM/LastFM.h"
#include "StreamPlugins/LastFM/LFMTrackChangedThread.h"
#include "StreamPlugins/LastFM/LFMGlobals.h"
#include "StreamPlugins/LastFM/LFMWebAccess.h"
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
#include <QMap>
#include <QString>


using namespace std;

LastFM* LastFM::getInstance(){
	static LastFM instance;
	return &instance;

}


LastFM::LastFM() {
	lfm_wa_init();
	_class_name = QString("LastFM");
	_logged_in = false;
	_track_changed_thread = 0;
    _login_thread = new LFMLoginThread();
    this->connect(_login_thread, SIGNAL(finished()), this, SLOT(_login_thread_finished()));
	_settings = CSettingsStorage::getInstance();
}



LastFM::~LastFM() {

}




bool LastFM::_lfm_init_track_changed_thread(){

    _track_changed_thread = new LFMTrackChangedThread(_class_name);

	if(_track_changed_thread){
			connect( _track_changed_thread, SIGNAL(sig_corrected_data_available(const QString&)),
					 this, 					SLOT(_sl_corrected_data_available(const QString&)));

			connect( _track_changed_thread, SIGNAL(sig_similar_artists_available(const QString&, const QList<int>&)),
					 this, 					SLOT(_sl_similar_artists_available(const QString&, const QList<int>&)));

			return true;
	}

	return false;



}


bool LastFM::_lfm_check_login(){
	if(!_logged_in || _session_key.size() != 32){

		QString username, password;
		CSettingsStorage::getInstance()->getLastFMNameAndPW(username, password);

		if(!username.isEmpty() && !password.isEmpty())
			lfm_login(username, password);

		if(!_logged_in || _session_key.size() != 32){
			return false;
		}

		return true;
	}

	else return true;
}

bool LastFM::lfm_is_logged_in(){
	return _logged_in;
}


void LastFM::_login_thread_finished(){

    LFMLoginStuff login_info = _login_thread->getLoginStuff();

    _logged_in = login_info.logged_in;
    _auth_token = login_info.authToken;
    _session_key = login_info.sessionKey;
    _session_key2 = login_info.sessionKey2;

    //qDebug() << "Logged in? " << _logged_in << ", Auth token? " << _auth_token << ", session key? " << _session_key << ", session key2? " << _session_key2;


    if(!_logged_in)
        emit sig_last_fm_logged_in(_logged_in);

}

void LastFM::lfm_login(QString username, QString password, bool should_emit){

	_logged_in = false;
    _username = username;
    _emit_login = should_emit;

    if(!_settings->getLastFMActive()){

            //emit sig_last_fm_logged_in(false);
    }

    _login_thread->setup_login_thread(username, password);
    _login_thread->start();

}


void LastFM::psl_login(QString username, QString password){
    lfm_login(username, password, false);
}


void LastFM::psl_track_changed(const MetaData& md){


    if(!_track_changed_thread) {
        if(!_lfm_init_track_changed_thread()) return;
    }

    if(_track_changed_thread->isRunning()) _track_changed_thread->terminate();


    if(!_settings->getLastFMActive() || !_logged_in) {

        _track_changed_thread->setThreadTask( LFM_THREAD_TASK_SIM_ARTISTS );
        _track_changed_thread->setTrackInfo(md);
        _track_changed_thread->start();

        return;
     }


	_track_changed_thread->setThreadTask(
			LFM_THREAD_TASK_UPDATE_TRACK 		|
			LFM_THREAD_TASK_SIM_ARTISTS 		|
			LFM_THREAD_TASK_FETCH_TRACK_INFO);

	_track_changed_thread->setTrackInfo(md);
	_track_changed_thread->setUsername(_username);
	_track_changed_thread->setSessionKey(_session_key);

	_track_changed_thread->start();
}


void LastFM::psl_scrobble(const MetaData& metadata){

	if(!_settings->getLastFMActive() || !_logged_in) return;

	if(!_lfm_check_login())	return;

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
	QString url = lfm_wa_create_sig_url_post(QString("http://ws.audioscrobbler.com/2.0/"), sig_data, post_data);
	QString response;

    bool success = lfm_wa_call_post_url(url, post_data, response);
	if(!success || response.contains("failed")){
		return;
	}

}



void LastFM::psl_radio_init(const QString& str, int radio_mode){

	if(!_logged_in){

		return;
	}

	if(_session_key2.size() != 32){
		if(!_lfm_check_login()){
			return;
		}

		else if(_session_key2.size() != 32){
			return;
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

		case LFM_RADIO_MODE_RECOMMENDED:
			lfm_radio_station += "user/" + str + "/recommended";
			break;

		case LFM_RADIO_MODE_USER_LIBRARY:
			lfm_radio_station += "user/" + str + "/library";
			break;

		default:
			break;
	}

	UrlParams data;
		data["session"] = _session_key2;
		data["lang"] = QString("en");
		data["url"] = QUrl::toPercentEncoding( lfm_radio_station );

	QString url = lfm_wa_create_std_url( QString("http://ws.audioscrobbler.com/radio/adjust.php"), data );
	QString response;
	bool success = lfm_wa_call_url(url, response);
	if( success ) psl_radio_playlist_request();

}


void LastFM::psl_radio_playlist_request(){

	if(_session_key2.size() != 32){
		if(!_lfm_check_login()){
			return;
		}

		else if(_session_key2.size() != 32){
			return;
		}
	}

	QDomDocument xml_response;
	MetaDataList v_md;

	UrlParams data;
	data["sk"] = _session_key2;
	data["discovery"] = "0";
	data["desktop"] = "1.5";

	QString url = lfm_wa_create_std_url(QString("http://ws.audioscrobbler.com/radio/xspf.php"), data);


	bool success = lfm_wa_call_url_xml(url, xml_response);
	if( !success ){
		qDebug() << "LFM: Cannot get playlist";
		qDebug() << "LFM: Url = " << url;
		return;
	}


	_lfm_parse_playlist_answer(v_md, xml_response);
	xml_response.clear();

	if(v_md.size() > 0){
		emit sig_new_radio_playlist(v_md);
		return;
	}
}



// private slot
void LastFM::_sl_similar_artists_available(const QString& target_class, const QList<int>& ids){
	if(target_class.compare(_class_name) != 0) return;

	emit sig_similar_artists_available(ids);
}

// private slot
void LastFM::_sl_corrected_data_available(const QString& target_class){

	if(target_class.compare(_class_name) != 0) return;

	MetaData md;
	bool loved;
	bool corrected;

	if( _track_changed_thread->fetch_corrections(md, loved, corrected) )
		emit sig_track_info_fetched(md, loved, corrected);
}





bool LastFM::_lfm_parse_playlist_answer(MetaDataList& v_md, const QDomDocument& doc){

	v_md.clear();

	QDomElement docElement = doc.documentElement();
	QDomNode tracklist = docElement.firstChildElement("trackList");

	if(!tracklist.hasChildNodes()) return false;

	for(int idx_track=0; idx_track < tracklist.childNodes().size(); idx_track++){

		QDomNode track = tracklist.childNodes().item(idx_track);
		if(!track.hasChildNodes()) continue;

		MetaData md;

		for(int idx_track_content = 0; idx_track_content <track.childNodes().size(); idx_track_content++){

            md.is_extern = false;
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

        md.radio_mode = RADIO_LFM;

		v_md.push_back(md);
	}

	return (v_md.size() > 0);
}


void LastFM::lfm_get_friends(QStringList& friends){

	if(!_logged_in)
		return;

	UrlParams params;

	params["user"] = _username;
	params["api_key"] = LFM_API_KEY;
	params["method"] = QString("user.getFriends");

	QString url_getFriends = lfm_wa_create_std_url("http://ws.audioscrobbler.com/2.0/", params);

	QString retval;

	bool success = lfm_wa_call_url(url_getFriends, retval);
	if(!success) {
		qDebug() << "LFM: Cannot fetch friends";
		return;
	}

	friends << _username;
	QString username = Helper::easy_tag_finder("user.name", retval);
	if(username.size() > 0)
		friends << username;
	int idx = 0;
	while(idx >= 0){
		idx = retval.indexOf("<user>");
		retval = retval.right(retval.size() - idx - 2);
		username = Helper::easy_tag_finder("user.name", retval);
		if(username.size() > 0)
			friends << username;
	}

	qDebug() << "Found " << friends;

}


bool LastFM::lfm_get_user_info(QMap<QString, QString>& userinfo){

	if(!_logged_in) {
		qDebug() << "Not logged in " << _username;

		return false;
	}
	QString retval;


	UrlParams params;
	params["user"] = _username;
	params["method"] = QString("user.getinfo");
	params["api_key"] = LFM_API_KEY;

	QString url_get_user_info = lfm_wa_create_std_url("http://ws.audioscrobbler.com/2.0/", params);

	bool success = lfm_wa_call_url(url_get_user_info, retval);
	if(!success) {
		qDebug() << "Could not fetch userdata";
		return false;
	}

	userinfo["playcount"] = Helper::easy_tag_finder(QString("user.playcount"), retval);
	userinfo["register_date"] = Helper::easy_tag_finder(QString("user.registered"), retval);
	return true;
}
