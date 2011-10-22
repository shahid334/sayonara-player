/*
 * LastFM.h
 *
 *  Created on: Apr 19, 2011
 *      Author: luke
 */

#ifndef LASTFM_H_
#define LASTFM_H_

#include <HelperStructs/MetaData.h>
#include <QObject>
#include <QString>
#include <string>


class LastFM : public QObject{

	Q_OBJECT
public:
	LastFM();
	virtual ~LastFM();

	signals:
		void similar_artists_available(const int&);

	public slots:
		void scrobble(const MetaData&);
		void update_track(const MetaData&);
		void login_slot(QString, QString);
		void get_similar_artists(const QString&);
		QString create_signature(QString fn_name);




	private:

		bool 			_logged_in;
		QString			_api_key;
		QString			_api_secret;
		QString			_username;
		QString			_auth_token;
		QString			_session_key;


	public:
		void login(QString, QString);
		QString get_api_key();

	private:
		QString parse_session_answer();

};

#endif /* LASTFM_H_ */
