/* LastFMAdapter.h

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
 * Apr 29, 2012 
 *
 */

#ifndef LASTFMADAPTER_H_
#define LASTFMADAPTER_H_

#include "LastFM/LastFM.h"
#include <QObject>


class LastFMAdapter : public QObject {

	Q_OBJECT

public:
	LastFMAdapter();
	virtual ~LastFMAdapter();

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


private:
		LastFM* 	lfm_api;


};

#endif /* LASTFMADAPTER_H_ */
