/* CoverLookup.h */

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
 * CoverLookup.h
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#ifndef COVERLOOKUP_H_
#define COVERLOOKUP_H_

#include "HelperStructs/MetaData.h"
#include "CoverLookup/CoverFetchThread.h"

#include <string>
#include <vector>

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QCryptographicHash>
#include <QDir>

#define EMIT_NONE 0
#define EMIT_ONE 1
#define EMIT_ALL 2


using namespace std;


class CoverLookup : public QObject{

	Q_OBJECT

	signals:
		// caller class, pixmaps
		void sig_covers_found(QString, vector<QPixmap>&);		/* emit if multiple covers are found (player) */
		// caller class, path
		void sig_cover_found(QString, QString);					/* emit if single cover is found (player) */
		// caller class, pixmap
		void sig_new_cover_found(QString, const QPixmap&);		/* emit if a new cover is found (alternate covers) */
		void sig_multi_covers_found(QString, int);

	public slots:
		void search_artist_image(const QString& artist); /* artist image */

        void search_cover(const MetaData& md);			 /* search a cover for certain metadata */
		void search_covers(const AlbumList&);		 /* search multiple covers */

		void search_images_by_searchstring(QString searchstring, int n_images, bool search_for_album);


		void research_cover(const MetaData&);	/* search alternative covers for one album */
		void search_all_covers();					/* search all covers*/
		void terminate_thread();					/* stop to search for covers */

	private slots:
		void thread_finished();


public:

	CoverLookup(QString caller_class);
	virtual ~CoverLookup();
	bool get_found_cover(int idx, QPixmap& p);


private:


	MetaData			_metadata;
	CoverFetchThread* 	_thread;
	vector<QPixmap> 	_alternative_covers;

	int 				_emit_type;
	bool				_research_done;
	QString				_caller_class;

	void 	_download_covers(QStringList adresses, uint num_covers_to_fetch, vector<QPixmap>& vec_pixmaps);
	Album 	_get_album_from_metadata(const MetaData& md);

};

#endif /* COVERLOOKUP_H_ */
