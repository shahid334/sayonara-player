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
#include "CoverLookup/CoverLocation.h"

#include <QThread>
#include <QMap>
#include <QImage>

class CoverFetchThread : public QThread {

Q_OBJECT

signals:
    void sig_finished(bool);
	void sig_cover_found(QString);

public:

	CoverFetchThread(QObject* parent, const CoverLocation& cl, const int n_covers);
    virtual ~CoverFetchThread();

    void stop();

protected:
    void run();



private:

    QString     _url;
    QString     _target_file;
    int         _n_covers;
    bool        _run;

	int run_single();
	int run_multi();

};

#endif /* COVERFETCHTHREAD_H_ */
