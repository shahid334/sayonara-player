/*
 * LastFM.cpp
 *
 *  Created on: Apr 19, 2011
 *      Author: luke
 */



#include <LastFM/LastFM.h>
#include <HelperStructs/MetaData.h>
#include <QObject>
#include <QDebug>
#include <iostream>
#include <string.h>
#include <string>
#include <ctime>
#include <QCryptographicHash>
#include <stdio.h>
#include <CSettingsStorage.h>

extern "C" {
	#include "clastfm.h"
}



using namespace std;



LastFM::LastFM() {
	_api_key = QString("51d6f9eaef806f603f346844bef326ba");
	_api_secret = QString("1093d769e54858cb0d21d42b35a8f603");

	_session = LASTFM_init(_api_key.toStdString().c_str(), _api_secret.toStdString().c_str());
	_logged_in = false;
}

LastFM::~LastFM() {

}


void LastFM::login(QString username, QString password){
    if (LASTFM_login_MD5(_session, username.toStdString().c_str(), password.toStdString().c_str())==0) {
        CSettingsStorage::getInstance()->setLastFMNameAndPW(username,password);
        _auth_token = QCryptographicHash::hash(username.toUtf8() + password.toUtf8(), QCryptographicHash::Md5).toHex();
        _username = username;
        _logged_in = true;
    }
    else {
        CSettingsStorage::getInstance()->setLastFMNameAndPW("","");
    }






//	qDebug() << "url = " << url;

}


void LastFM::login_slot(QString username, QString password){

	login(username, password);

}

void LastFM::scrobble(const MetaData& metadata){



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


	char* title = new char[metadata.title.toStdString().length()+1];
	char* album = new char[metadata.album.toStdString().length()+1];
	char* artist = new char[metadata.artist.toStdString().length()+1];

	strcpy (title, metadata.title.toStdString().c_str());
	strcpy (artist, metadata.artist.toStdString().c_str());
	strcpy (album, metadata.album.toStdString().c_str());

	LASTFM_track_scrobble(_session, title, album, artist,started, metadata.length_ms /1000,0,0,NULL);

	delete title;
	delete album;
	delete artist;

	cout << "LastFM: " << LASTFM_status(_session) << endl;
}



void LastFM::update_track(const MetaData& metadata){



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


	char* title = new char[metadata.title.toStdString().length()+1];
	char* album = new char[metadata.album.toStdString().length()+1];
	char* artist = new char[metadata.artist.toStdString().length()+1];

	strcpy (title, metadata.title.toStdString().c_str());
	strcpy (artist, metadata.artist.toStdString().c_str());
	strcpy (album, metadata.album.toStdString().c_str());

	LASTFM_track_update_now_playing(_session, title, album, artist, metadata.length_ms/1000, 0, 0);


	delete title;
	delete album;
	delete artist;

	cout << "LastFM: " << LASTFM_status(_session) << endl;


}
