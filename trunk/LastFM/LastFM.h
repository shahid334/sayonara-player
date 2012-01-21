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
#include <string>




class LastFM : public QObject{

	Q_OBJECT
public:
	LastFM();
	virtual ~LastFM();

	signals:
		void similar_artists_available(QList<int>&);
		void last_fm_logged_in(bool);

	public slots:
		void scrobble(const MetaData&);
		void update_track(const MetaData&);
		void login_slot(QString, QString);
		void get_similar_artists(const QString&);
		void get_radio(const QString&, bool);

	private slots:
		void sim_artists_thread_finished();




	private:

		bool 			_logged_in;
		QString			_api_key;
		QString			_api_secret;
		QString			_username;
		QString			_auth_token;
		QString			_session_key;
		QString			_session_key2;


	public:
		void login(QString, QString);
		void login_session(QString, QString);
		QString get_api_key();
		QString create_signature(QString fn_name);

	private:
		QString parse_session_answer();
		LFM_SimilarArtists* _similar_artists_thread;
		void parse_playlist_answer(vector<MetaData>& v_md, QString xml);


};

#endif /* LASTFM_H_ */
