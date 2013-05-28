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



CoverFetchThread::CoverFetchThread(QObject* parent, int id, QString url, const QStringList& target_names, QString call_id): QThread(parent) {
    _id = id;
    _url = url;
    _target_names = target_names;
    _call_id = call_id;
    _mode = CFT_SINGLE;



}

CoverFetchThread::CoverFetchThread(QObject* parent, int id, QString url, int n_images) : QThread(parent){

    _id = id;
    _url = url;
    _n_images = n_images;
    _mode = CFT_MULTI;
    _cur_awa_idx = 1;
    _run = true;
}

CoverFetchThread::~CoverFetchThread() {

    // TODO Auto-generated destructor stub
}

void CoverFetchThread::run(){

    if(_mode == CFT_SINGLE)
        search_single();
    else
        search_multi();

}

void CoverFetchThread::search_single(){

    QStringList adresses = cov_call_and_parse(_url, 10);

    _found_cover_paths.clear();
    foreach(QString adress, adresses){
        QImage img;
        bool success = cov_download_cover(adress, img);

        if(success){

            for(int i=0; i<_target_names.size(); i++){
                QString target_name = _target_names[i];
                img.save(target_name);
                _found_cover_paths << target_name;
            }
        }

        if(_found_cover_paths.size() > 0) break;
    }

    emit sig_finished(_id);
}

void CoverFetchThread::search_multi(){

      //QStringList adresses = cov_call_and_parse(_url, _n_images);
    QStringList adresses = cov_call_and_parse(_url, 50);

      _cur_awa_idx = 1;
      _n_running = 0;

      int id=100;
      foreach(QString adress, adresses){

          AsyncWebAccess* awa = new AsyncWebAccess(0, id);
          _map.insert(id, awa);

          awa->set_url(adress);
          connect(awa, SIGNAL(finished(int)), this, SLOT(awa_finished(int)));

          awa->start();
          _n_running ++;
          id++;

      }

      QString path = Helper::getSayonaraPath() + "/tmp/";
      QDir dir(Helper::getSayonaraPath());
      dir.mkpath("tmp");

      while(_run){
          usleep(100000);
          if(_datalist.size() == 0) {
              continue;
          }

          QString data = _datalist[0];
          _datalist[0].clear();
          _datalist.removeAt(0);
          if(data.size() == 0){
              continue;
          }

          QImage img;
          bool success = img.loadFromData(data.toAscii());
          if(!success){
              continue;
          }

          success = img.save(path + "img_" + QString::number(_cur_awa_idx) + ".jpg");
          _cur_awa_idx++;
      }

      qDebug() << "Alternative cover thread dies";
}



QStringList CoverFetchThread::get_found_covers(){

    return _found_cover_paths;
}

int CoverFetchThread::get_id(){
    return _id;
}

QString CoverFetchThread::get_call_id(){
    return _call_id;
}

void CoverFetchThread::awa_finished(int id){

    qDebug() << "awa finished";
    QString data;
    AsyncWebAccess* awa = _map.value(id);
    awa->get_data(data);



    _map.remove(id);

    _datalist << data;
    awa->deleteLater();
    _n_running --;
}

void CoverFetchThread::set_run(bool run){
    _n_running = 0;
    _run = run;
}

