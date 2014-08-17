/* CoverFetchThread.cpp */

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
 * CoverFetchThread.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: luke
 */

#include "CoverLookup/CoverFetchThread.h"
#include "CoverLookup/CoverDownloader.h"


#include <QDebug>
#include <QFile>
#include <QDir>



CoverFetchThread::CoverFetchThread(QObject* parent, const QString& url, const QString& target_file, int n_covers) :
    QThread(parent),
    _url(url),
    _target_file(target_file),
    _n_covers(n_covers),
    _run(true)
{

}


CoverFetchThread::~CoverFetchThread() {

}


int CoverFetchThread::run_single(){

    QStringList adresses = CoverDownloader::cov_call_and_parse(_url, 10);

    foreach(QString adress, adresses){

        if(!_run) return 0;

        QImage img;

        bool success = CoverDownloader::cov_download_cover(adress, &img);

        if(success){

            img.save(_target_file);
            emit sig_cover_found(_target_file);
            return 1;
        }
    }

    return 0;
}


int CoverFetchThread::run_multi(){

    int idx=1;
    QStringList adresses = CoverDownloader::cov_call_and_parse(_url, _n_covers * 2);

    foreach(QString adress, adresses){

        if(!_run) return idx;

        QImage img;

        bool success = CoverDownloader::cov_download_cover(adress, &img);

        if(success){

            QString cover_path;
            cover_path = QString("_") +
                    QString::number(idx) +
                    _target_file;

            img.save( cover_path );

            emit sig_cover_found(cover_path);

            if( idx == n_covers ) break;

            idx++;
        }
    }

   return idx;
}

void CoverFetchThread::stop(){

    _run = false;
}

void CoverFetchThread::run(){

    int n_covers_found;

    if( _n_covers == 1 ){
        n_covers_found = run_single();
    }

    else {
        n_covers_found = run_multi();
    }



    if( n_covers_found >= _n_covers ){
        emit sig_finished(true);
    }

    else {
        emit sig_finished(false);
    }

}


