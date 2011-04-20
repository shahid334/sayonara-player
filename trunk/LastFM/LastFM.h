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
#include <string>


extern "C"{
#include <clastfm.h>

}

class LastFM : public QObject{

	Q_OBJECT
public:
	LastFM(std::string username, std::string password);
	virtual ~LastFM();

	public slots:
		void scrobble(const MetaData&);

	private:
		LASTFM_SESSION* _session;
};

#endif /* LASTFM_H_ */
