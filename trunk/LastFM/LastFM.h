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


extern "C"{
#include <clastfm.h>

}

class LastFM : public QObject{

	Q_OBJECT
public:
	LastFM();
	virtual ~LastFM();

	public slots:
		void scrobble(const MetaData&);
		void login_slot(QString, QString);

	private:
		LASTFM_SESSION* _session;
		bool 			_logged_in;

	public:
		void login(std::string, std::string);


};

#endif /* LASTFM_H_ */
