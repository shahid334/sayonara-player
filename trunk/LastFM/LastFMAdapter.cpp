/* LastFMAdapter.cpp

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Apr 29, 2012 
 *
 */

#include "LastFM/LastFMAdapter.h"
#include "LastFM/LastFM.h"

LastFMAdapter::LastFMAdapter() {
	lfm_api = LastFM::getInstance();

}

LastFMAdapter::~LastFMAdapter() {
	// TODO Auto-generated destructor stub
}


void LastFMAdapter::scrobble(const MetaData& md){
	lfm_api->scrobble(md);
}

void LastFMAdapter::get_track_info(const MetaData& md){
	QMap<QString, QString> map;
	lfm_api->getTrackInfo(md.artist, md.title, map);

	bool corrected = false;
	bool loved = (map[LFM_TAG_TRACK_LOVED].toInt() == 1);

	QString artist = map[LFM_TAG_TRACK_ARTIST];
	QString title = map[LFM_TAG_TRACK_TITLE];

	MetaData md_copy = md;


	if(artist.toLower() != md.artist.toLower() ||
		title.toLower() != md.title.toLower() ){
		corrected = true;
		md_copy.artist = artist;
		md_copy.title = title;
	}

	emit track_info_fetched(md_copy, loved, corrected);
}

void LastFMAdapter::update_track(const MetaData& md){

	lfm_api->update_track(md);
	get_similar_artists(md.artist);
	get_track_info(md);

}

void LastFMAdapter::login_slot(QString username, QString password){
	bool logged_in = lfm_api->login(username, password);
	emit last_fm_logged_in(logged_in);
}

void LastFMAdapter::get_similar_artists(const QString& artist){
	QList<int> artist_ids;
	if( lfm_api->get_similar_artists(artist, artist_ids) ){
		emit similar_artists_available(artist_ids);
	}
}

void LastFMAdapter::radio_init(const QString& radio_str, bool artist){
	int lfm_radio_mode;
	if(artist) lfm_radio_mode = LFM_RADIO_MODE_ARTIST;
	else lfm_radio_mode = LFM_RADIO_MODE_TAG;

	lfm_api->radio_init(radio_str, lfm_radio_mode);

	radio_get_playlist();
}

void LastFMAdapter::radio_get_playlist(){
	vector<MetaData> v_md;
	if( lfm_api->radio_get_playlist(v_md) ){
		emit new_radio_playlist(v_md);
	}
}





