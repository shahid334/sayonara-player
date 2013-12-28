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
#include "HelperStructs/AsyncWebAccess.h"

#include <QThread>
#include <QString>
#include <QStringList>
#include <QMap>

#define CFT_SINGLE 0
#define CFT_MULTI 1

class CoverFetchThread : public QThread {

Q_OBJECT

signals:
    void sig_finished(int);
    void sig_one_image_available();


public:
    CoverFetchThread(QObject* parent, int id, QString url, const QStringList& target_names, QString call_id);
    CoverFetchThread(QObject* parent, int id, QString url, int n_images);
	virtual ~CoverFetchThread();

    QStringList get_found_covers();
    int get_id();
    QString get_call_id();
    void set_run(bool);



protected:
    void run();


private:

    int         _mode;
    int         _id;
    int         _n_images;
    QString     _url;
    QString     _call_id;
    QStringList _target_names;
    QStringList _found_cover_paths;

    int         _cur_awa_idx;
    int         _awa_id;
    QStringList _datalist;
    bool         _run;
    int          _n_running;
    QList<AsyncWebAccess*> _lst;
    QMap<int, AsyncWebAccess*> _map;



    void search_single();
    void search_multi();

private slots:
    void awa_finished(int);
    void awa_terminated(int);

};

#endif /* COVERFETCHTHREAD_H_ */
