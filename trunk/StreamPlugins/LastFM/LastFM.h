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
#include "StreamPlugins/LastFM/LFMGlobals.h"
#include "StreamPlugins/LastFM/LFMTrackChangedThread.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QtXml>
#include <string>



// singleton base LastFM API class
// signals and slots are handled by the adapter class
class LastFM : public QObject{

Q_OBJECT

	signals:
		void sig_last_fm_logged_in(bool);
		void sig_similar_artists_available(const QList<int>&);
		void sig_new_radio_playlist(const vector<MetaData>&);
		void sig_track_info_fetched(const MetaData& md, bool loved, bool corrected);

	public slots:
		void scrobble(const MetaData&);
		void track_changed(const MetaData&);
		void login_slot(QString, QString);
		void radio_init(const QString&, int);
		void radio_get_playlist();

	private slots:
		void similar_artists_available(const QString&, const QList<int>&);
		void corrected_data_available(const QString&);

	public:
		static LastFM * getInstance();
		virtual ~LastFM();

		bool update_track(const MetaData&);
		void get_similar_artists(const QString&);
		void get_friends(QStringList& );

		bool login(QString username, QString password);

		/*static QString get_api_key();
		static QString calc_album_lfm_adress(QString album);
		static QString calc_search_album_adress(QString album);
		static QString calc_search_artist_adress(QString album);*/

	private:

		 LastFM();
		 LastFM(const LastFM&);
		 LastFM& operator=(const LastFM&);

		 void init();
		 bool init_track_changed_thread();

		 QString			_class_name;

		bool 			_logged_in;

		QString			_username;
		QString			_auth_token;
		QString			_session_key;
		QString			_session_key2;
		MetaData		_loved_tracks;


	private:
		QString parse_session_answer();
		LFMTrackChangedThread* _track_changed_thread;
		bool parse_playlist_answer(vector<MetaData>& v_md, const QDomDocument& xml);


		bool check_login();
};

#endif /* LASTFM_H_ */
