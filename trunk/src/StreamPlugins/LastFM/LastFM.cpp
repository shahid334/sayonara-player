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

#include "StreamPlugins/LastFM/LastFM.h"
#include "StreamPlugins/LastFM/LFMTrackChangedThread.h"
#include "StreamPlugins/LastFM/LFMWebAccess.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QDomDocument>
#include <QMessageBox>
#include <QUrl>

#include <curl/curl.h>
#include <string>
#include <time.h>


using namespace std;

LastFM* LastFM::getInstance() {
	static LastFM instance;
	return &instance;

}


LastFM::LastFM() :
	QObject(),
	SayonaraClass()
{
	lfm_wa_init();
	_class_name = QString("LastFM");
	_logged_in = false;
	_track_changed_thread = 0;
    _login_thread = new LFMLoginThread();

	connect(_login_thread, SIGNAL(finished()), this, SLOT(_login_thread_finished()));

	REGISTER_LISTENER_NO_CALL(Set::LFM_Login, psl_login);
	REGISTER_LISTENER(Set::LFM_Active, psl_login);
}



LastFM::~LastFM() {

}

void LastFM::get_login(QString& user, QString& pw){
	QStringList user_pw;
	user_pw = Settings::getInstance()->get(Set::LFM_Login);

	if(user_pw.size() > 1){
		user = user_pw[0];
		pw = user_pw[1];
	}
}


bool LastFM::_lfm_init_track_changed_thread() {

    _track_changed_thread = new LFMTrackChangedThread(_class_name);

	if(_track_changed_thread) {
			connect( _track_changed_thread, SIGNAL(sig_corrected_data_available(const QString&)),
					 this, 					SLOT(_sl_corrected_data_available(const QString&)));

			connect( _track_changed_thread, SIGNAL(sig_similar_artists_available(const QString&, const QList<int>&)),
					 this, 					SLOT(_sl_similar_artists_available(const QString&, const QList<int>&)));

			return true;
	}

	return false;
}


bool LastFM::_lfm_check_login() {

	if(!_logged_in || _session_key.size() != 32) {

		QString username, password;
		LastFM::get_login(username, password);

		if(!username.isEmpty() && !password.isEmpty()){
			psl_login();
		}

		if(!_logged_in || _session_key.size() != 32) {
			return false;
		}

		return true;
	}

	else {
		return true;
	}
}


bool LastFM::lfm_is_logged_in() {
	return _logged_in;
}


void LastFM::psl_login() {

	if(!_settings->get(Set::LFM_Active)){
		return;
	}

	QString username, password;
	get_login(username, password);

    _logged_in = false;
    _username = username;

    _login_thread->start();
}


void LastFM::_login_thread_finished() {

    LFMLoginStuff login_info = _login_thread->getLoginStuff();

    if(login_info.logged_in) {
        login_info.logged_in = _parse_error_message(login_info.error, true);
    }

    _logged_in = login_info.logged_in;
    _auth_token = login_info.token;
    _session_key = login_info.session_key;

	_settings->set(Set::LFM_SessionKey, _session_key);

	if(!_logged_in){
		emit sig_last_fm_logged_in(_logged_in);
	}
}



void LastFM::psl_track_changed(const MetaData& md, bool start_play) {

	PlaylistMode pl_mode = _settings->get(Set::PL_Mode);

    if(!_track_changed_thread) {
        if(!_lfm_init_track_changed_thread()) return;
    }

    if(_track_changed_thread->isRunning()) _track_changed_thread->terminate();

	bool lfm_active = _settings->get(Set::LFM_Active);

	if(! lfm_active || !_logged_in) {


		if(! pl_mode.dynamic ) return;

        _track_changed_thread->setThreadTask( LFM_THREAD_TASK_SIM_ARTISTS );
        _track_changed_thread->setTrackInfo(md);
        _track_changed_thread->start();

        return;
     }

    int thread_task = LFM_THREAD_TASK_UPDATE_TRACK | LFM_THREAD_TASK_FETCH_TRACK_INFO;


	if( pl_mode.dynamic )
        thread_task |= LFM_THREAD_TASK_SIM_ARTISTS;

    _track_changed_thread->setThreadTask(thread_task);


	_track_changed_thread->setTrackInfo(md);
	_track_changed_thread->setUsername(_username);
	_track_changed_thread->setSessionKey(_session_key);

    _track_changed_thread->start();
}


void LastFM::psl_scrobble(const MetaData& metadata) {

	bool lfm_active =_settings->get(Set::LFM_Active);

	if(! lfm_active || !_logged_in) {
		return;
	}

	if(!_lfm_check_login())	return;

    time_t rawtime, started;
    rawtime = time(NULL);
    struct tm* ptm = localtime(&rawtime);
    started = mktime(ptm);

	QString artist = metadata.artist;
	QString title = metadata.title;

	UrlParams sig_data;
		sig_data["api_key"] = LFM_API_KEY;
		sig_data["artist"] = artist;
		sig_data["duration"] = QString::number(metadata.length_ms / 1000);
		sig_data["method"] = QString("track.scrobble");
		sig_data["sk"] = _session_key;
        sig_data["timestamp"] = QString::number(started);
		sig_data["track"] = title;

    string post_data;
	QString url = lfm_wa_create_sig_url_post(QString("http://ws.audioscrobbler.com/2.0/"), sig_data, post_data);
	QString response;

    lfm_wa_call_post_url(url, post_data, response);
    _parse_error_message(response);

}


// private slot
void LastFM::_sl_similar_artists_available(const QString& target_class, const QList<int>& ids) {
	if(target_class.compare(_class_name) != 0) return;

	emit sig_similar_artists_available(ids);
}

// private slot
void LastFM::_sl_corrected_data_available(const QString& target_class) {

	if(target_class.compare(_class_name) != 0) return;

	MetaData md;
	bool loved;
	bool corrected;

	if( _track_changed_thread->fetch_corrections(md, loved, corrected) )
		emit sig_track_info_fetched(md, loved, corrected);
}



void LastFM::lfm_get_friends(QStringList& friends) {

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

    if(!_parse_error_message(retval)) return;

	friends << _username;
	QString username = Helper::easy_tag_finder("user.name", retval);
	if(username.size() > 0)
		friends << username;
	int idx = 0;
	while(idx >= 0) {
		idx = retval.indexOf("<user>");
		retval = retval.right(retval.size() - idx - 2);
		username = Helper::easy_tag_finder("user.name", retval);
		if(username.size() > 0)
			friends << username;
	}


}


bool LastFM::lfm_get_user_info(QMap<QString, QString>& userinfo) {

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

    _parse_error_message(retval);

	userinfo["playcount"] = Helper::easy_tag_finder(QString("user.playcount"), retval);
	userinfo["register_date"] = Helper::easy_tag_finder(QString("user.registered"), retval);
	return true;
}


bool LastFM::_parse_error_message(QString& response, bool force) {

	QString error_msg;

	bool lfm_show_errors = _settings->get(Set::LFM_ShowErrors);

    if(response.left(100).contains("failed")) {

		error_msg = Helper::easy_tag_finder("lfm.error", response);

		if( lfm_show_errors || force) {
			_show_error_message(error_msg);
		}

        return false;
    }

    return true;
}

void LastFM::_show_error_message(QString err_msg) {

    QMessageBox::warning(NULL, "Last.fm Error", err_msg);
}
