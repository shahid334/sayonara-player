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
#include "HelperStructs/CSettingsStorage.h"
#include "StreamPlugins/LastFM/LFMGlobals.h"
#include "StreamPlugins/LastFM/LFMTrackChangedThread.h"
#include "StreamPlugins/LastFM/LFMLoginThread.h"


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
		void sig_new_radio_playlist(const MetaDataList&);
        void sig_radio_initialized(bool);
        void sig_create_playlist(MetaDataList&, bool);
		void sig_track_info_fetched(const MetaData& md, bool loved, bool corrected);
		void sig_user_info_fetched(QMap<QString, QString>&);



	public slots:
		void psl_scrobble(const MetaData&);
		void psl_track_changed(const MetaData&);
		void psl_login(QString, QString);
		void psl_radio_init(const QString&, int);
		void psl_radio_playlist_request();



	private slots:
		void _sl_similar_artists_available(const QString&, const QList<int>&);
		void _sl_corrected_data_available(const QString&);
        void _login_thread_finished();

	public:
		static LastFM * getInstance();


        void lfm_login(QString username, QString password, bool should_emit=true);
		void lfm_get_friends(QStringList& );
		bool lfm_get_user_info(QMap<QString, QString>&);
		bool lfm_is_logged_in();

	private:

		LastFM();
		LastFM(const LastFM&);
		LastFM& operator=(const LastFM&);

	public:

		virtual ~LastFM();

		static LastFM* getInstance(bool init_now){
			static LastFM inst;
			return &inst;
		}

	private:

		bool 	_lfm_init_track_changed_thread();
		bool 	_lfm_update_track(const MetaData&);
		void 	_lfm_get_similar_artists(const QString&);
		QString _lfm_parse_session_answer();
		bool 	_lfm_parse_playlist_answer(MetaDataList& v_md, const QDomDocument& xml);
		bool 	_lfm_check_login();
        bool    _parse_error_message(QString& response, bool force=false);
        void    _show_error_message(QString err_msg);


		QString			_class_name;

		bool 			_logged_in;
        bool            _emit_login;

		QString			_username;
		QString			_auth_token;
		QString			_session_key;
		QString			_session_key2;
		MetaData		_loved_tracks;

		CSettingsStorage* _settings;

		LFMTrackChangedThread* _track_changed_thread;
        LFMLoginThread* _login_thread;
};

#endif /* LASTFM_H_ */
