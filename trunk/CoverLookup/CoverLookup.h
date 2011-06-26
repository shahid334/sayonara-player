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
#include <QCryptographicHash>
#include <QDir>


using namespace std;


class CoverLookup : public QObject{

	Q_OBJECT

	vector<QPixmap> _pixmaps;
	vector<string> 	_cover_adresses;
	string			_artist;
	string			_album;
	QString			_cover_token;
	QString			_filepath;
	QString			_cover_path;
	int 			_cur_cover;
	bool			_terminate_cover_fetch;

	signals:
		void cover_found(QPixmap&);

	public slots:
		void search_cover(const MetaData&, bool emit_signal=true);
		void showCoverAlternatives();
		void search_all_covers();

	private slots:
		void terminate_cover_fetch();

public:
	CoverLookup();
	virtual ~CoverLookup();
	static QString get_cover_path(QString artist, QString album);


private:
	QPixmap add_new_pixmap(string cover_filename);
	string 	calc_url_adress();
	void 	search_cover(bool emit_signal);
	void 	download_covers(uint num);
};

#endif /* COVERLOOKUP_H_ */
