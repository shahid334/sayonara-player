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


#define UrlParams QMap<QString, QString>

class LastFM : public QObject{

	Q_OBJECT
public:
	LastFM();
	virtual ~LastFM();

	signals:
		void similar_artists_available(QList<int>&);
		void last_fm_logged_in(bool);
		void new_radio_playlist(const vector<MetaData>&);

	public slots:
		void scrobble(const MetaData&);
		void update_track(const MetaData&);
		void login_slot(QString, QString);
		void get_similar_artists(const QString&);
		void radio_init(const QString&, bool);
		void radio_get_playlist();

	private slots:
		void sim_artists_thread_finished();




	private:

		bool 			_logged_in;

		QString			_username;
		QString			_auth_token;
		QString			_session_key;
		QString			_session_key2;


	public:
		void login(QString, QString);
		static QString get_api_key();

		static QString calc_album_lfm_adress(QString album);
		static QString calc_search_album_adress(QString album);
		static QString calc_search_artist_adress(QString album);


	private:
		QString parse_session_answer();
		LFM_SimilarArtists* _similar_artists_thread;
		void parse_playlist_answer(vector<MetaData>& v_md, const QDomDocument& xml);

		QString create_std_url(const QString& base_url, const UrlParams& data);
		QString create_sig_url(const QString& base_url, const UrlParams& sig_data);
		QString create_std_url_post(const QString& base_url, const UrlParams& data, string& post_data);
		QString create_sig_url_post(const QString& base_url, const UrlParams& sig_data, string& post_data);
		QString create_signature(const UrlParams& data);

		bool check_login();




};

#endif /* LASTFM_H_ */
