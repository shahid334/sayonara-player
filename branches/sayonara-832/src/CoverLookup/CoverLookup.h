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
#include "CoverLookup/CoverLocation.h"
#include "CoverLookup/CoverFetchThread.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QDir>
#include <QMap>




class CoverLookupInterface : public QObject{

	Q_OBJECT

	signals:
		void sig_cover_found(const QString&);
		void sig_finished(bool);

	public slots:
		virtual void stop()=0;

	public:
		CoverLookupInterface(QObject* parent=0);

};


class CoverLookup : public CoverLookupInterface {

	Q_OBJECT


private slots:
	void cover_found(QString);
	void finished(bool);


public:

   CoverLookup(QObject* parent, int n_covers=1);
   virtual ~CoverLookup();

	bool fetch_cover(const CoverLocation& cl);
	bool fetch_album_cover_standard(const QString& artist, const QString& album_name);
	bool fetch_album_cover_sampler(const QStringList& artists, const QString& album_name);
	bool fetch_album_cover(const Album& album);
	bool fetch_album_cover_by_id(const int album_id);

	bool fetch_artist_cover_standard(const QString& artist);
	bool fetch_artist_cover(const Artist& artist);

	bool fetch_cover_by_searchstring(const QString& searchstring, const QString& target_name);

	void emit_standard_cover();
	virtual void stop();


private:

	int _n_covers;
    CoverFetchThread* _cft;
    CDatabaseConnector* _db;


	void start_new_thread(const CoverLocation& cl);
};


class CoverLookupAlternative : public CoverLookupInterface
{

    Q_OBJECT

private:

	CoverLookupAlternative(QObject* parent, int n_covers);

    CoverLookup*        _cl;
    QString             _album_name;
    QString             _artist_name;
    QStringList         _artists_name;
	int                 _album_id;
	Album               _album;
    Artist              _artist;

	int					_n_covers;
	bool				_run;


	enum SearchType {

		ST_Standard=0,
		ST_Sampler=1,
		ST_ByID=2,
		ST_ByAlbum=3,
		ST_ByArtistName = 4,
		ST_ByArtist = 5
	} _search_type;


public:

    CoverLookupAlternative(QObject* parent, const QString& album_name, const QString& artist_name, int n_covers);
    CoverLookupAlternative(QObject* parent, const QString& album_name, const QStringList& artists_name, int n_covers);
    CoverLookupAlternative(QObject* parent, const Album& album, int n_covers);
	CoverLookupAlternative(QObject* parent, int album_id, int n_covers);
	CoverLookupAlternative(QObject* parent, const QString& artist, int n_covers);
    CoverLookupAlternative(QObject* parent, const Artist& artist, int n_covers);

    ~CoverLookupAlternative();

	virtual void stop();
    void start();


private slots:
	void cover_found(QString);
	void finished(bool);
};


class CoverLookupAll : public CoverLookupInterface
{

    Q_OBJECT


private:
    CoverLookup* _cl;
    AlbumList _album_list;
    bool _run;


private slots:
	void cover_found(QString);
	void finished(bool);


public:
    CoverLookupAll(QObject* parent, const AlbumList& albumlist);
    ~CoverLookupAll();

	virtual void stop();
    void start();

protected:
    void run();
};


#endif /* COVERLOOKUP_H_ */
