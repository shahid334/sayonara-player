/* LastFM.h */

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
 * LastFM.h
 *
 *  Created on: Apr 19, 2011
 *      Author: luke
 */

#ifndef LASTFM_H_
#define LASTFM_H_


#include "HelperStructs/MetaData.h"
#include "LastFM/LFMSimilarArtistsThread.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QtXml>
#include <string>

#define LFM_RADIO_MODE_ARTIST 0
#define LFM_RADIO_MODE_TAG 1
#define LFM_RADIO_MODE_RECOMMENDED 2
#define LFM_RADIO_MODE_USER_LIBRARY 3


#define LFM_TAG_TRACK_TITLE "track.name"
#define LFM_TAG_TRACK_ALBUM "track.album.title"
#define LFM_TAG_TRACK_ARTIST "track.artist.name"
#define LFM_TAG_TRACK_DURATION "track.duration"
#define LFM_TAG_TRACK_LOVED "track.userloved"
#define LFM_TAG_TRACK_USERPLAYCOUNT "track.userplaycount"


#define UrlParams QMap<QString, QString>

// singleton base LastFM API class
// signals and slots are handled by the adapter class
class LastFM : public QObject{

Q_OBJECT

	signals:
		void last_fm_logged_in(bool);
		void similar_artists_available(QList<int>&);
		void new_radio_playlist(const vector<MetaData>&);
		void track_info_fetched(const MetaData& md, bool loved, bool corrected);

	public slots:
		void scrobble(const MetaData&);
		void track_changed(const MetaData&);
		void login_slot(QString, QString);
		void radio_init(const QString&, int);
		void radio_get_playlist();

	private slots:
		void sim_artists_thread_finished();

	public:
		static LastFM * getInstance();
		virtual ~LastFM();

		bool update_track(const MetaData&);
		void get_similar_artists(const QString&);
		void get_friends(QStringList& );

		bool login(QString username, QString password);

		static QString get_api_key();
		static QString calc_album_lfm_adress(QString album);
		static QString calc_search_album_adress(QString album);
		static QString calc_search_artist_adress(QString album);

		QString get_artist_info(const QString& artist);
		QString get_album_info(const QString& artist, const QString& album);

		bool get_track_info(const MetaData& md, bool emit_sig=true);
		bool get_track_info(const MetaData& md, QMap<QString, QString>& values, bool emit_sig=true);


	private:

		 LastFM();
		 LastFM(const LastFM&);
		 LastFM& operator=(const LastFM&);

		 void init();

		bool 			_logged_in;

		QString			_username;
		QString			_auth_token;
		QString			_session_key;
		QString			_session_key2;
		MetaData		_loved_tracks;

	private:
		QString parse_session_answer();
		LFM_SimilarArtists* _similar_artists_thread;
		bool parse_playlist_answer(vector<MetaData>& v_md, const QDomDocument& xml);

		QString create_std_url(const QString& base_url, const UrlParams& data);
		QString create_sig_url(const QString& base_url, const UrlParams& sig_data);
		QString create_std_url_post(const QString& base_url, const UrlParams& data, string& post_data);
		QString create_sig_url_post(const QString& base_url, const UrlParams& sig_data, string& post_data);
		QString create_signature(const UrlParams& data);

		bool check_login();
};

#endif /* LASTFM_H_ */
