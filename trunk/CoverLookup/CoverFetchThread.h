/* CoverFetchThread.h */

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
 * CoverFetchThread.h
 *
 *  Created on: Jun 28, 2011
 *      Author: luke
 */

#ifndef COVERFETCHTHREAD_H_
#define COVERFETCHTHREAD_H_

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"

#include <QThread>
#include <QObject>
#include <QImage>

#include <vector>

#define COV_SRC_GOOGLE 0
#define COV_SRC_LFM 1

#define COV_FETCH_MODE_ALBUM_STR 0
#define COV_FETCH_MODE_ARTIST_STR 1
#define COV_FETCH_MODE_SINGLE_ALBUM 2
#define COV_FETCH_MODE_ALL_ALBUMS 3


using namespace std;


class CoverFetchThread : public QThread {



public:
	CoverFetchThread();
	virtual ~CoverFetchThread();

	int		get_cover_fetch_mode();
	void 	get_images(vector<QImage>&);
	bool 	get_certain_image(int idx, QImage& );
	int 	get_num_images();

	int 	get_cover_source();
	void 	reset();

	void	setup_fetch_artist_image(const QString& artist, int source);
	void	setup_fetch_album_covers(const vector<Album>& albums, int source);
	void	setup_fetch_single_album(const Album& album, int source);

protected:
	void run();


private:

	vector<Album> 	_vec_albums;
	vector<QImage> 	_images;
	int 			_cover_source;
	int 			_num_covers_2_fetch;
	QString 		_album_searchstring;
	QString 		_artist_searchstring;
	int 			_cover_fetch_mode;


	void search_images_for_albums();
	void search_images_for_artist(const QString& artist_name, int num);
	void search_images_for_album_str(const QString album_name, int num);
	void search_images_for_artist_str(const QString artist_name, int num);


};

#endif /* COVERFETCHTHREAD_H_ */
