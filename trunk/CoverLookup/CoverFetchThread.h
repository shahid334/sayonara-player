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

using namespace std;


class CoverFetchThread : public QThread {



public:
	CoverFetchThread();
	virtual ~CoverFetchThread();

	bool set_albums_to_fetch(const vector<Album> &);
	bool set_search_all_covers(bool);
	bool set_num_covers_2_fetch(int);
	bool set_cover_source(int source);
	void get_images(vector<QImage>&);


protected:
	void run();


private:
	bool _search_all_covers;
	vector<Album> _vec_albums;
	vector<QImage> _images;
	int _cover_source;
	int _num_covers_2_fetch;


	void search_covers_for_albums(const vector<Album>&);
	void search_alternative_covers_for_album();

};

#endif /* COVERFETCHTHREAD_H_ */
