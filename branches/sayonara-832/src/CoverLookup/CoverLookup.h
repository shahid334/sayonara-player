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
#include <QCryptographicHash>
#include <QDir>
#include <QMap>

#define EMIT_NONE 0
#define EMIT_ONE 1
#define EMIT_ALL 2


using namespace std;

typedef QString WWWAdress;
typedef QString TargetFileName;



class CoverLookup;

class CoverLookupAllCoverThread : public QThread
{

    AlbumList _albumlist;
    CoverLookup* _cover_lookup;
    bool _run;

public:


    CoverLookupAllCoverThread(QObject* parent, const AlbumList& albumlist);
    ~CoverLookupAllCoverThread();


    void stop();
    CoverLookup* getCoverLookup();

protected:
    void run();
};


class CoverLookup : public QObject{

	Q_OBJECT

	signals:
    void sig_covers_found(const QStringList&, QString call_id);
    void sig_alt_cover_found(const QString&, QString call_id);

	public slots:

    void fetch_cover_album(const int album_id, QString call_id="", bool for_all_tracks=true);
    void fetch_cover_album(const Album& album, QString call_id="", bool for_all_tracks=true);

    void fetch_cover_artist(const int artist_id, QString call_id="");
    void fetch_cover_artist(const Artist& artist, QString call_id="");

    void fetch_all_album_covers();
    void fetch_all_album_covers_stop();



private slots:
   void thread_finished(int id);
   void fetch_all_album_covers_destroyed();


public:

    CoverLookup();
	virtual ~CoverLookup();




private:

    CDatabaseConnector* _db;
    CoverLookupAllCoverThread* _all_covers_thread;
    QList<CoverFetchThread*> _threads;
    QList<int> _finished_queue;
    bool _finish_locked;
    int _cur_thread_id;


    void start_new_thread(QString url, const QStringList& target_names, QString call_id );


};

#endif /* COVERLOOKUP_H_ */
