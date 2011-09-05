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

	bool 	set_albums_to_fetch(const vector<Album> &);
	bool	set_album_searchstring(const QString &);
	bool 	set_artist_searchstring(const QString &);
	bool	set_cover_fetch_mode(int);
	bool 	set_search_all_covers(bool);
	bool 	set_num_covers_2_fetch(int);
	bool 	set_cover_source(int source);
	void 	get_images(vector<QImage>&);
	bool 	get_certain_image(int idx, QImage& );
	int 	get_num_images();


protected:
	void run();


private:

	vector<Album> _vec_albums;
	vector<QImage> _images;
	int _cover_source;
	int _num_covers_2_fetch;
	QString _album_searchstring;
	QString _artist_searchstring;
	int _cover_fetch_mode;


	void search_covers_for_albums(const vector<Album>&);
	void search_covers_for_album_str(const QString album_name, int num);
	void search_covers_for_artist_str(const QString artist_name, int num);


};

#endif /* COVERFETCHTHREAD_H_ */
