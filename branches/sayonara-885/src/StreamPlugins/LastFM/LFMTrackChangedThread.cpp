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
#include "HelperStructs/CSettingsStorage.h"

#include <QMap>
#include <QStringList>
#include <qdom.h>
#include <QUrl>

#define UrlParams QMap<QString, QString>

LFMTrackChangedThread::LFMTrackChangedThread(QString target_class) {
	_target_class = target_class;

	ArtistList artists;
	CDatabaseConnector::getInstance()->getAllArtists(artists);

	_smart_comparison = new SmartComparison(artists);
}


LFMTrackChangedThread::~LFMTrackChangedThread() {
	delete _smart_comparison;
}

void LFMTrackChangedThread::setSessionKey(QString session_key) {
	_session_key = session_key;
}

void LFMTrackChangedThread::setUsername(QString username) {
	_username = username;
}

void LFMTrackChangedThread::setTrackInfo(const MetaData& md) {
	_md = md;
}

void LFMTrackChangedThread::setAlbumName(QString album_name) {
	_album_name = album_name;
}

void LFMTrackChangedThread::setArtistName(QString artist_name) {
	_artist_name = artist_name;
}

void LFMTrackChangedThread::setThreadTask(int task) {
	_thread_tasks = task;
}

void LFMTrackChangedThread::setTargetClass(QString name) {
	_target_class = name;
}


void LFMTrackChangedThread::run() {

	bool success = false;

	_album_data.clear();
	_artist_data.clear();

	if(_thread_tasks & LFM_THREAD_TASK_UPDATE_TRACK) {
		success = update_now_playing();
	}

    bool dynamic = CSettingsStorage::getInstance()->getPlaylistMode().dynamic;
    if(dynamic && (_thread_tasks & LFM_THREAD_TASK_SIM_ARTISTS)) {
		success = search_similar_artists();
        if(success){
            emit sig_similar_artists_available(_target_class, _chosen_ids);
        }
	}


	if(_thread_tasks & LFM_THREAD_TASK_FETCH_TRACK_INFO) {
		MetaData md = _md;
		bool loved;
		bool corrected;

		success = get_corrected_track_info(md, loved, corrected);
		_corrected_success = success;
		_loved = loved;
		_corrected = corrected;
		_md_corrected = md;

		if(success) {
			emit sig_corrected_data_available(_target_class);
		}
	}

	if(_thread_tasks & LFM_THREAD_TASK_FETCH_ALBUM_INFO) {
		success = get_album_info(_artist_name, _album_name);
        if(success){
            emit sig_album_info_available(_target_class);
        }
	}

	if(_thread_tasks & LFM_THREAD_TASK_FETCH_ARTIST_INFO) {
		success = get_artist_info(_artist_name);
        if(success){
            emit sig_artist_info_available(_target_class);
        }
    }

	_thread_tasks = 0;
}


bool LFMTrackChangedThread::update_now_playing() {

	QString artist = _md.artist;
	QString title = _md.title;

    if(artist.trimmed().size() == 0) artist = "Unknown";
    artist.replace("&", "&amp;");

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
	if(!success || response.contains("failed") ) {

		return false;
	}

	return true;
}


bool LFMTrackChangedThread::search_similar_artists() {

    bool success;
    QString artist_name = _md.artist;

    bool callLFM = true;



    if(artist_name.trimmed().size() == 0) return false;
    artist_name.replace("&", "&amp;");


    ArtistMatch artist_match;
    artist_match.artist = artist_name;


    // check if already in cache
    if(_sim_artists_cache.keys().contains(artist_name)) {
        artist_match = _sim_artists_cache.value(artist_name);
        callLFM = false;
    }

    srand ( time(NULL) );

    if(callLFM) {

        QString url = 	QString("http://ws.audioscrobbler.com/2.0/?");
        QString encoded = QUrl::toPercentEncoding( artist_name );
        url += QString("method=artist.getsimilar&");
        url += QString("artist=") + encoded + QString("&");
        url += QString("api_key=") + LFM_API_KEY;

        QDomDocument doc("similar_artists");

        success = lfm_wa_call_url_xml(url, doc);
        if(!success) {
            qDebug() << "could not call url: " << url;
            return false;
        }

        QDomElement docElement = doc.documentElement();
        QDomNode similarartists = docElement.firstChild();			// similarartists

        if(similarartists.hasChildNodes()) {

            QString artist_name = "";
            double match = -1.0;

            for(int idx_artist=0; idx_artist < similarartists.childNodes().size(); idx_artist++) {
                QDomNode artist = similarartists.childNodes().item(idx_artist);

                if(artist.nodeName().toLower().compare("artist") != 0) continue;

                if(!artist.hasChildNodes()) continue;

                for(int idx_content = 0; idx_content <artist.childNodes().size(); idx_content++) {
                    QDomNode content = artist.childNodes().item(idx_content);
                    if(content.nodeName().toLower().contains("name")) {
                        QDomElement e = content.toElement();
                        if(!e.isNull()) {
                            artist_name = e.text();
                        }
                    }

                    if(content.nodeName().toLower().contains("match")) {
                        QDomElement e = content.toElement();
                        if(!e.isNull()) {
                            match = e.text().toDouble();
                        }
                    }

                    if(artist_name.size() > 0 && match > 0) {
                        artist_match.add(artist_name, match);
                        artist_name = "";
                        match = -1.0;
                        break;
                    }
                }
            }
        }

        else{
            qDebug() << "empty xml document from " << url;
            return false;
        }

        doc.clear();
        _sim_artists_cache[artist_name] = artist_match;
    } // end found in cache



	// if we always take the best, it's boring
    Quality quality, quality_org;
    int rnd = rand() % 1000;
    if(rnd > 250) quality = Quality_Very_Good;			// [250-999]
    else if(rnd > 50) quality = Quality_Well;		// [50-250]
    else quality = Quality_Poor;

    quality_org = quality;
    QMap<QString, int> possible_artists;

    while(possible_artists.size() == 0) {

        QMap<QString, double> quality_map = artist_match.get(quality);
        possible_artists = filter_available_artists(quality_map);

		switch(quality){
			case Quality_Poor:
				quality = Quality_Very_Good;
				break;
			case Quality_Well:
				quality = Quality_Poor;
				break;
			case Quality_Very_Good:
				quality = Quality_Well;
				break;
			default:
				quality = quality_org;
				break;
		}

        if(quality == quality_org) break;
    }

	if(possible_artists.size() == 0){
		return false;
	}

    _chosen_ids.clear();
    for(QMap<QString, int>::iterator it = possible_artists.begin(); it != possible_artists.end(); it++) {
        _chosen_ids.push_back(it.value());
    }

    return (_chosen_ids.size() > 0);
}



QMap<QString, int> LFMTrackChangedThread::filter_available_artists(const QMap<QString, double>& artist_match) {

		CDatabaseConnector* db = CDatabaseConnector::getInstance();
        QMap<QString, int> possible_artists;

        foreach(QString key, artist_match.keys()) {

#if SMART_COMP

            QMap<QString, float> sc_map = _smart_comparison->get_similar_strings(key);
            foreach( QString sc_key, sc_map.keys() ){
				int artist_id = db->getArtistID(sc_key);
				if(artist_id >= 0 && sc_map[sc_key] > 5.0f){

					possible_artists[sc_key] = artist_id;
				}

			}

#else
                int artist_id = db->getArtistID(key);
                if(artist_id >= 0 ){

                    possible_artists[key] = artist_id;
                }

#endif
        }

        return possible_artists;
}

bool LFMTrackChangedThread::fetch_corrections(MetaData& md, bool& loved, bool& corrected) {

	md = _md_corrected;
	loved = _loved;
	corrected = _corrected;

	return _corrected_success;

}

bool LFMTrackChangedThread::fetch_album_info(QMap<QString, QString>& info) {
	info = _album_data;
	return(info.keys().count() > 0);
}

bool LFMTrackChangedThread::fetch_artist_info( QMap<QString, QString>& info) {
	info = _artist_data;
	return(info.keys().count() > 0);
}



bool LFMTrackChangedThread::get_corrected_track_info(MetaData& md, bool& loved, bool& corrected) {
	QString retval;
	QMap<QString, QString> values;

    QString artist = _md.artist;
    if(artist.trimmed().size() == 0) artist = "Unknown";
    artist.replace("&", "&amp;");

		UrlParams params;
        params["artist"] = QUrl::toPercentEncoding(artist);
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

        foreach(QString str2search, search_list) {
			QString str = Helper::easy_tag_finder(str2search , retval);
			values[str2search] = str;
		}

		corrected = false;
		loved = (values[LFM_TAG_TRACK_LOVED].toInt() == 1);

                artist = values[LFM_TAG_TRACK_ARTIST];
        QString album = values[LFM_TAG_TRACK_ALBUM];
		QString title = values[LFM_TAG_TRACK_TITLE];

        bool artist_cor = (artist.compare(md.artist, Qt::CaseInsensitive) != 0);
        bool title_cor = (title.compare(md.title, Qt::CaseInsensitive) != 0);
        bool album_cor = (album.compare(md.album, Qt::CaseInsensitive) != 0);

        if(artist_cor || title_cor || album_cor) {

			corrected = true;
			md.artist = artist;
            md.album = album;
			md.title = title;
		}

		return true;
}



bool LFMTrackChangedThread::get_artist_info(QString artist) {

	QString retval;

    if(artist.trimmed().size() == 0) artist = "Unknown";
    artist.replace("&", "&amp;");

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

    QString lfm_playcount = tr("Last.fm plays");
    _artist_data[lfm_playcount] = Helper::easy_tag_finder(QString("artist.stats.userplaycount"), retval);

	return true;

}

bool LFMTrackChangedThread::get_album_info(QString artist, QString album) {

	QString retval;

    if(artist.trimmed().size() == 0) artist = "Unknown";
    artist.replace("&", "&amp;");

    if(album.trimmed().size() == 0) artist = "Unknown";
    album.replace("&", "&amp;");

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

    QString release_date = tr("Release Date");
    QString lfm_playcount = tr("Last.fm plays");

    _album_data[release_date] = Helper::easy_tag_finder(QString("album.releasedate"), retval);
    _album_data[lfm_playcount] = Helper::easy_tag_finder(QString("album.userplaycount"), retval);

	return true;
}



