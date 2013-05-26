/* CoverLookup.cpp */

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
 * CoverLookup.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#include "CoverLookup/CoverLookup.h"
#include "CoverLookup/CoverFetchThread.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"

#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QImage>
#include <unistd.h>


using namespace std;


CoverLookup::CoverLookup() {
    _db = CDatabaseConnector::getInstance();
    _cur_thread_id = 0;
    _finish_locked = false;
}

CoverLookup::~CoverLookup() {

}


void CoverLookup::start_new_thread(QString url, const QStringList& target_names, QString call_id ){

    CoverFetchThread* thread = new CoverFetchThread(this, _cur_thread_id, url, target_names, call_id);

    connect(thread, SIGNAL(sig_finished(int)), this, SLOT(thread_finished(int)));

    _cur_thread_id ++;
    _threads.push_back(thread);

    thread->start();
}


void CoverLookup::fetch_cover_album(const int album_id, QString call_id, bool for_all_tracks){

    if(album_id == -1) return;

    Album album;
    bool success = _db->getAlbumByID(album_id, album);
    if(!success) return;

    fetch_cover_album(album, call_id, for_all_tracks);
}


void CoverLookup::fetch_cover_album(const Album& album, QString call_id, bool for_all_tracks){

    Album tmp_album = album;
    if(tmp_album.artists.size() == 0) tmp_album.artists.push_back("");

    QStringList target_names;
    QStringList existing_names;

    // one google adress for various artist/album combinations
    if(for_all_tracks){

        QString url_artist_name = "";

        if (tmp_album.artists.size() == 1) url_artist_name = tmp_album.artists[0];
        else url_artist_name = "Various";

        QString url = Helper::calc_google_album_adress(url_artist_name, tmp_album.name);

        foreach(QString artist, tmp_album.artists){

            QString target_name = Helper::get_cover_path(artist, tmp_album.name);

            if(QFile::exists(target_name)){
                existing_names << target_name;
            }

            else{

                target_names << target_name;
            }
        }

        if(target_names.size() > 0)
            start_new_thread(url, target_names, call_id);
    }

    else {

        foreach(QString artist, tmp_album.artists){
            QString target_name = Helper::get_cover_path(artist, tmp_album.name);

            QString url = Helper::calc_google_album_adress(artist, tmp_album.name);

            if(QFile::exists(target_name)){
                existing_names << target_name;
            }

            else{
                target_names << target_name;
                start_new_thread(url, target_names, call_id);
            }
        }
    }

    if(existing_names.size() > 0){
        emit sig_covers_found(existing_names, call_id);
    }
}


void CoverLookup::fetch_cover_artist(const int artist_id, QString call_id){
    if(artist_id == -1) return;

    Artist artist;
    bool success = _db->getArtistByID(artist_id, artist);
    if(!success) return;

    fetch_cover_artist(artist, call_id);
}


void CoverLookup::fetch_cover_artist(const Artist& artist, QString call_id){

    QStringList target_names;
    QStringList existing_names;

    QString target_name = Helper::get_artist_image_path(artist.name);
    QString url = Helper::calc_google_artist_adress(artist.name);

    if(QFile::exists(target_name)){
        existing_names << target_name;
        emit sig_covers_found(existing_names, call_id);
    }

    else{
        target_names << target_name;
        start_new_thread(url, target_names, call_id);
    }
}




void CoverLookup::thread_finished(int id){

    if(_finish_locked && !_finished_queue.contains(id)) {
        _finished_queue.push_back(id);
        return;
    }

    _finish_locked = true;
    int finished_idx = _finished_queue.indexOf(id);
    if(finished_idx >= 0 && finished_idx < _finished_queue.size()) _finished_queue.removeAt(finished_idx);
    _finished_queue.removeAt(_finished_queue.indexOf(id));

    int idx = 0;

    // search this thread
    bool found = false;
    CoverFetchThread* the_thread = NULL;
    foreach(CoverFetchThread* thread, _threads){
        int tid = thread->get_id();
        if(tid == id){
            the_thread = thread;
            found = true;
            break;
        }

        idx++;
    }

    if(found) _threads.removeAt(idx);
    if(!the_thread) {
        _finish_locked = false;
        return;
    }

    QString call_id = the_thread->get_call_id();
    QStringList found_cover_paths = the_thread->get_found_covers();


    if(found_cover_paths.size() > 0)
        emit sig_covers_found(found_cover_paths, call_id);


    // vlt probleme
    int max = 10;
    while(the_thread->isRunning() && max) {
        the_thread->quit();
        usleep(50000);
        max --;
    }
    if(the_thread->isRunning()) the_thread->terminate();
    else delete the_thread;

    if(_finished_queue.size() > 0)
        thread_finished(_finished_queue[0]);

    _finish_locked = false;
}
