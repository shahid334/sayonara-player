/* LFMSimilarArtistsThread.h */

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
 * LFMSimilarArtists.h
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#ifndef LFMSIMILARARTISTS_H_
#define LFMSIMILARARTISTS_H_

#include <QThread>
#include <QString>
#include <QList>
#include <QMap>


class LFM_SimilarArtists: public QThread {

Q_OBJECT
public:
	LFM_SimilarArtists(QString api_key);
	virtual ~LFM_SimilarArtists();

	void set_artist_name(QString artist_name);
	QList<int> get_chosen_ids();

protected:
	void run();

private:
	QString 		_api_key;
	QString 		_artist_name;
	QList<int>		_chosen_ids;

	QMap<QString, int> filter_available_artists(QMap<QString, double> *artists, int idx);



};

#endif /* LFMSIMILARARTISTS_H_ */
