/*
 * CoverLookup.h
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#ifndef COVERLOOKUP_H_
#define COVERLOOKUP_H_

#include "HelperStructs/MetaData.h"

#include <string>
#include <vector>

#include <QObject>
#include <QPixmap>


using namespace std;


class CoverLookup : public QObject{

	Q_OBJECT

	vector<QPixmap> _pixmaps;
	vector<string> 	_cover_adresses;
	string			_artist;
	string			_album;
	int 			_cur_cover;

	signals:
		void cover_found(QPixmap&);
	public slots:
		void search_cover(const MetaData&);
		void showCoverAlternatives();

public:
	CoverLookup();
	virtual ~CoverLookup();


private:
	QPixmap add_new_pixmap(string cover_filename);
	string 	calc_url_adress();
	void 	search_cover();
	void 	download_covers(uint num, bool apply_cover);
};

#endif /* COVERLOOKUP_H_ */
