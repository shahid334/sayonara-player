/* LFMTrackChangedThread.cpp

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
 * Jul 18, 2012 
 *
 */

#include "StreamPlugins/LastFM/LFMTrackChangedThread.h"
#include "StreamPlugins/LastFM/LFMWebAccess.h"
#include "StreamPlugins/LastFM/LFMGlobals.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"

#include <QMap>
#include <QString>
#include <QStringList>
#include <qdom.h>
#include <QUrl>
#include <QList>

#define UrlParams QMap<QString, QString>

LFMTrackChangedThread::LFMTrackChangedThread(QString target_class) {
	_target_class = target_class;
}


LFMTrackChangedThread::~LFMTrackChangedThread() {

}

void LFMTrackChangedThread::setSessionKey(QString session_key){
	_session_key = session_key;
}

void LFMTrackChangedThread::setUsername(QString username){
	_username = username;
}

void LFMTrackChangedThread::setTrackInfo(const MetaData& md){
	_md = md;
}

void LFMTrackChangedThread::setAlbumName(QString album_name){
	_album_name = album_name;
}

void LFMTrackChangedThread::setArtistName(QString artist_name){
	_artist_name = artist_name;
}

void LFMTrackChangedThread::setThreadTask(int task){
	_thread_tasks = task;
}

void LFMTrackChangedThread::setTargetClass(QString name){
	_target_class = name;
}


void LFMTrackChangedThread::run(){

	bool success = false;

	_album_data.clear();
	_artist_data.clear();

	if(_thread_tasks & LFM_THREAD_TASK_UPDATE_TRACK){
		success = update_now_playing();
		if(!success)
			qDebug() << "Could not update current played track";
	}



	if(_thread_tasks & LFM_THREAD_TASK_SIM_ARTISTS){
		success = search_similar_artists();
		if(success)
			emit sig_similar_artists_available(_target_class, _chosen_ids);
	}


	if(_thread_tasks & LFM_THREAD_TASK_FETCH_TRACK_INFO){
		MetaData md = _md;
		bool loved;
		bool corrected;

		success = get_corrected_track_info(md, loved, corrected);
		_corrected_success = success;
		_loved = loved;
		_corrected = corrected;
		_md_corrected = md;

		if(success){
			emit sig_corrected_data_available(_target_class);
		}
	}

	if(_thread_tasks & LFM_THREAD_TASK_FETCH_ALBUM_INFO){
		success = get_album_info(_artist_name, _album_name);
		if(success)
			emit sig_album_info_available(_target_class);
	}

	if(_thread_tasks & LFM_THREAD_TASK_FETCH_ARTIST_INFO){
		success = get_artist_info(_artist_name);
		if(success)
			emit sig_artist_info_available(_target_class);
	}

	_thread_tasks = 0;

}


bool LFMTrackChangedThread::update_now_playing(){

	QString artist = _md.artist;
	QString title = _md.title;

	UrlParams sig_data;
		sig_data["api_key"] = LFM_API_KEY;
		sig_data["artist"] = artist;
		sig_data["duration"] = QString::number(_md.length_ms / 1000);
		sig_data["method"] = QString("track.updatenowplaying").toLocal8Bit();
		sig_data["sk"] = _session_key;
		sig_data["track"] =  title;


	string post_data;
	QString url = lfm_wa_create_sig_url_post(QString("http://ws.audioscrobbler.com/2.0/"), sig_data, post_data);
	QString response;

	bool success = lfm_wa_call_post_url(url, post_data, response);
	if(!success || response.contains("failed") ){
		qDebug() << "url: " << url << "\n" << response;
		return false;
	}

	return true;
}


bool LFMTrackChangedThread::search_similar_artists(){

	bool success;
	QString artist_name = _md.artist;
	artist_name.replace("&", "&amp;");

	 srand ( time(NULL) );

		QString url = 	QString("http://ws.audioscrobbler.com/2.0/?");
		QString encoded = QUrl::toPercentEncoding( artist_name );
		url += QString("method=artist.getsimilar&");
		url += QString("artist=") + encoded + QString("&");
		url += QString("api_key=") + LFM_API_KEY;

		QDomDocument doc("similar_artists");

		success = lfm_wa_call_url_xml(url, doc);
		if(!success) return false;

		QDomElement docElement = doc.documentElement();
		QDomNode similarartists = docElement.firstChild();			// similarartists

		// 0= very good match
		// 1= good match
		// 2= ok match
		QMap<QString, double> artist_match[3];

		if(similarartists.hasChildNodes()){

			QString artist_name = "";
			double match = -1.0;

			for(int idx_artist=0; idx_artist < similarartists.childNodes().size(); idx_artist++){
				QDomNode artist = similarartists.childNodes().item(idx_artist);

				if(artist.nodeName().toLower().compare("artist") != 0) continue;

				if(!artist.hasChildNodes()) continue;

				for(int idx_content = 0; idx_content <artist.childNodes().size(); idx_content++){
					QDomNode content = artist.childNodes().item(idx_content);
					if(content.nodeName().toLower().contains("name")){
						QDomElement e = content.toElement();
						if(!e.isNull()){
							artist_name = e.text();
						}
					}

					if(content.nodeName().toLower().contains("match")){
						QDomElement e = content.toElement();
						if(!e.isNull()){
							match = e.text().toDouble();
						}
					}

					if(artist_name.size() > 0 && match >= 0) {
						if(match > 0.1){
							artist_match[0][artist_name] = match;
						}

						else if(match > 0.03){
							artist_match[1][artist_name] = match;
						}

						else {
							artist_match[2][artist_name] = match;
						}
						artist_name = "";
						match = -1.0;
						break;
					}
				}
			}

		}

		else
			return false;

		doc.clear();

		// get random list where to search the artist in
		int idx = 0;
		int rnd = rand() % 1000;
		if(rnd > 250) idx = 0;			// [250-999]
		else if(rnd > 50) idx = 1;		// [50-250]
		else idx = 2;

		QMap<QString, int> possible_artists =
				filter_available_artists(artist_match, idx);

		if(possible_artists.size() == 0) return false;

		_chosen_ids.clear();
		for(QMap<QString, int>::iterator it = possible_artists.begin(); it != possible_artists.end(); it++){

			_chosen_ids.push_back(it.value());
		}

		return (_chosen_ids.size() > 0);
}



QMap<QString, int> LFMTrackChangedThread::filter_available_artists(QMap<QString, double> *artists, int idx){

		int start_idx = idx;

		QMap<QString, int> possible_artists;

		while(possible_artists.keys().size() == 0){

			foreach(QString key, artists[idx].keys()){

				int artist_id = CDatabaseConnector::getInstance()->getArtistID(key);
				if(artist_id != -1){
					possible_artists[key] = artist_id;
				}
			}

			if(possible_artists.keys().size() == 0) {
				idx--;
				if(idx == -1) idx = 2;
				if(start_idx == idx) break;
			}
			else break;
		}

		return possible_artists;
}

bool LFMTrackChangedThread::fetch_corrections(MetaData& md, bool& loved, bool& corrected){

	md = _md_corrected;
	loved = _loved;
	corrected = _corrected;

	return _corrected_success;

}

bool LFMTrackChangedThread::fetch_album_info(QMap<QString, QString>& info){
	info = _album_data;
	return(info.keys().count() > 0);
}

bool LFMTrackChangedThread::fetch_artist_info( QMap<QString, QString>& info){
	info = _artist_data;
	return(info.keys().count() > 0);
}



bool LFMTrackChangedThread::get_corrected_track_info(MetaData& md, bool& loved, bool& corrected){
	QString retval;
	QMap<QString, QString> values;

		UrlParams params;
		params["artist"] = QUrl::toPercentEncoding(_md.artist);
		params["track"] = QUrl::toPercentEncoding(_md.title);
		params["username"] = _username;
		params["method"] = QString("track.getinfo");
		params["autocorrect"] = QString("1");
		params["api_key"] = LFM_API_KEY;

		QString url_getTrackInfo = lfm_wa_create_std_url("http://ws.audioscrobbler.com/2.0/", params);

		bool success = lfm_wa_call_url(url_getTrackInfo, retval);

		if(!success) {
			return false;
		}

		QStringList search_list;
		search_list << LFM_TAG_TRACK_USERPLAYCOUNT;
		search_list << LFM_TAG_TRACK_LOVED;
		search_list << LFM_TAG_TRACK_ALBUM;
		search_list << LFM_TAG_TRACK_ARTIST;
		search_list << LFM_TAG_TRACK_DURATION;
		search_list << LFM_TAG_TRACK_TITLE;
		foreach(QString str2search, search_list){
			QString str = Helper::easy_tag_finder(str2search , retval);
			values[str2search] = str;
		}

		corrected = false;
		loved = (values[LFM_TAG_TRACK_LOVED].toInt() == 1);

		QString artist = values[LFM_TAG_TRACK_ARTIST];
		QString title = values[LFM_TAG_TRACK_TITLE];

		if(artist.toLower() != md.artist.toLower() ||
			title.toLower() != md.title.toLower() ){
			corrected = true;
			md.artist = artist;
			md.title = title;
		}

		return true;
}



bool LFMTrackChangedThread::get_artist_info(QString artist){

	QString retval;

	UrlParams params;
	params["artist"] = QUrl::toPercentEncoding(artist);
	params["username"] = _username;
	params["method"] = QString("artist.getinfo");
	params["api_key"] = LFM_API_KEY;


	QString url_getArtistInfo = lfm_wa_create_std_url("http://ws.audioscrobbler.com/2.0/", params);

	bool success = lfm_wa_call_url(url_getArtistInfo, retval);
	if(!success) {

		return false;
	}

	_artist_data["LastFM plays"] = Helper::easy_tag_finder(QString("artist.stats.userplaycount"), retval);

	return true;

}

bool LFMTrackChangedThread::get_album_info(QString artist, QString album){

	QString retval;

	UrlParams params;
	params["artist"] = QUrl::toPercentEncoding(artist);
	params["album"] = QUrl::toPercentEncoding(album);
	params["username"] = _username;
	params["method"] = QString("album.getinfo");
	params["api_key"] = LFM_API_KEY;


	QString url_getAlbumInfo = lfm_wa_create_std_url("http://ws.audioscrobbler.com/2.0/", params);


	bool success = lfm_wa_call_url(url_getAlbumInfo, retval);
	if(!success) {
		return false;
	}

	_album_data["Release Date"] = Helper::easy_tag_finder(QString("album.releasedate"), retval);
	_album_data["LastFM plays"] = Helper::easy_tag_finder(QString("album.userplaycount"), retval);

	return true;
}



