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
#include "DatabaseAccess/CDatabaseConnector.h"

#include <string>
#include <vector>

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QDir>
#include <QMap>


class CoverLookup : public QObject{

	Q_OBJECT

signals:
    void sig_cover_found(const QString&);
    void sig_finished(bool);


private slots:
   void thread_finished(bool);
   void cover_found(QString);


public:

   static void get_target_path(QString& album_name, const QString& artist_name);
   static void get_target_path(QString& album_name, const QStrinLiss& artists_name);
   static void get_target_path(int album_id);
   static void get_target_path(Album album);
   static void get_target_path(const Artist& artist);
   static void get_target_path(const QString& artist);


    void fetch_album_cover_standard(const QString& artist, const QString& album_name);
    void fetch_album_cover_sampler(const QStringList& artists, const QString& album_name);
    void fetch_album_cover_by_id(const int album_id);
    void fetch_album_cover(const Album& album);

    void fetch_artist_cover_standard(const QString& artist);
    void fetch_artist_cover_by_id(const int artist_id);
    void fetch_artist_cover(const Artist& artist);

    void fetch_cover_by_searchstring(const QString& searchstring, const QString& target_name);

    CoverLookup(QObject* parent, int n_covers=1);
	virtual ~CoverLookup();


private:

    CoverFetchThread* _cft;
    CDatabaseConnector* _db;
    int _n_covers;

    void start_new_thread(const QString& url, const QString& target_name);
};


class CoverLookupAlternative : public QObject
{


    enum SearchType {

        ST_Standard=0,
        ST_Sampler=1,
        ST_ByID=2,
        ST_ByAlbum=3,
        ST_ByArtistName = 4,
        ST_ByArtist = 5
    } _search_type;


    Q_OBJECT

signals:
    void sig_new_cover(QString);
    void sig_finished();

private:

    CoverLookup*        _cl;
    QString             _album_name;
    QString             _artist_name;
    QStringList         _artists_name;
    int                 _album_id;
    Album               _album;
    Artist              _artist;

    int _n_covers;
    bool _run;


public:

    CoverLookupAlternative(QObject* parent, const QString& album_name, const QString& artist_name, int n_covers);
    CoverLookupAlternative(QObject* parent, const QString& album_name, const QStringList& artists_name, int n_covers);
    CoverLookupAlternative(QObject* parent, const Album& album, int n_covers);
    CoverLookupAlternative(QObject* parent, int album_id, int n_covers);
    CoverLookupAlternative(QObject* parent, const QString& artist, int n_covers);
    CoverLookupAlternative(QObject* parent, const Artist& artist, int n_covers);

    ~CoverLookupAlternative();

    void stop();
    void start();



private slots:
    void cover_found(QString);
    void finished(bool);


};


class CoverLookupAll : public QObject
{

    Q_OBJECT

signals:
    void sig_new_cover(QString);


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

    void stop();
    void start();

protected:
    void run();
};


#endif /* COVERLOOKUP_H_ */
