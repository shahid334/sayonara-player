/*
 * LastFM.cpp
 *
 *  Created on: Apr 19, 2011
 *      Author: luke
 */



#include <LastFM/LastFM.h>
#include <HelperStructs/MetaData.h>
#include <QObject>
#include <iostream>
#include <string.h>


extern "C" {
	#include "clastfm.h"
}
#include <iostream>

using namespace std;



LastFM::LastFM(string username, string password) {
	_session = LASTFM_init("51d6f9eaef806f603f346844bef326ba", "1093d769e54858cb0d21d42b35a8f603");
	LASTFM_login(_session, username.c_str(), password.c_str());

}

LastFM::~LastFM() {

}

void LastFM::scrobble(const MetaData& metadata){



	time_t started;
	time(&started);


	char* title = new char[metadata.title.toStdString().length()+1];
	char* album = new char[metadata.album.toStdString().length()+1];
	char* artist = new char[metadata.artist.toStdString().length()+1];

	strcpy (title, metadata.title.toStdString().c_str());
	strcpy (artist, metadata.artist.toStdString().c_str());
	strcpy (album, metadata.album.toStdString().c_str());

	LASTFM_track_scrobble(_session, title, album, artist,started, metadata.length_ms ,0,0,NULL);

	delete title;
	delete album;
	delete artist;




	cout << "LastFM: " << LASTFM_status(_session) << endl;
}
