/* Playlist.cpp */

/* Copyright (C) 2013  Lucio Carreras
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



#include "playlist/Playlist.h"
#include <QDir>

Playlist::Playlist(QObject* parent) : QObject(parent){

    _reports_disabled = false;
    _cur_play_idx = -1;
}

void Playlist::report_changes(bool pl_changed, bool track_changed){
    if(_reports_disabled) return;

    if(pl_changed)
        emit sig_playlist_changed(_v_md, _cur_play_idx);

    if(track_changed){
        if(_cur_play_idx < 0 || _cur_play_idx >= (int) _v_md.size() )
            emit sig_stopped();
        else
            emit sig_track_changed(_v_md[_cur_play_idx], _cur_play_idx);
    }
}

void Playlist::enable_reports(){
    _reports_disabled = false;
}

void Playlist::disable_reports(){
    _reports_disabled = true;
}

void Playlist::clear(){

    _v_md.clear();
    _cur_play_idx = -1;
    report_changes(true, false);
}

void Playlist::move_track(const int idx, int tgt){
    QList<int> lst;
    lst << idx;
    move_tracks(lst, tgt);
}

void Playlist::move_tracks(const QList<int>& lst, int tgt){

    MetaDataList v_md_to_move;
    MetaDataList v_md_before_tgt;
    MetaDataList v_md_after_tgt;

    for(int i=0; i< (int) _v_md.size(); i++){

        if(!lst.contains(i) && i < tgt)
            v_md_before_tgt.push_back(_v_md[i]);

        else if(!lst.contains(i) && i >= tgt)
            v_md_after_tgt.push_back(_v_md[i]);

        else if(lst.contains(i))
            v_md_to_move.push_back(_v_md[i]);
    }

    _v_md.clear();
    _cur_play_idx = -1;

    foreach(MetaData md, v_md_before_tgt){
        md.pl_selected = false;
        _v_md.push_back(md);
        if(md.pl_playing) _cur_play_idx = _v_md.size() - 1;
    }

    foreach(MetaData md, v_md_to_move){
        md.pl_selected = true;
        _v_md.push_back(md);
        if(md.pl_playing) _cur_play_idx = _v_md.size() - 1;
    }

    foreach(MetaData md, v_md_after_tgt){
        md.pl_selected = false;
        _v_md.push_back(md);
        if(md.pl_playing) _cur_play_idx = _v_md.size() - 1;
    }

    report_changes(true, false);
}


void Playlist::delete_track(const int idx){
    QList<int> to_remove;
    to_remove << idx;
    delete_tracks(to_remove);
}

void Playlist::delete_tracks(const QList<int>& lst){

    if(lst.size() == 0) return;

    MetaDataList v_md;
    int first_selected = -1;

    _cur_play_idx = -1;

    for(int i=0; i< (int) _v_md.size(); i++){

        MetaData md = _v_md[i];
        // do not delete
        if( !lst.contains(i) ){

            if(md.pl_selected && first_selected == -1) first_selected = i;

            md.pl_selected = false;
            v_md.push_back(md);
            if(md.pl_playing){
                _cur_play_idx = (int) (v_md.size() -1);
            }
        }
    }

    if(first_selected == -1 && ((int) v_md.size()) > lst[0]){
        v_md[lst[0]].pl_selected = true;
    }

    else if(first_selected == -1 && ((int) v_md.size() <= lst[0]) && v_md.size() > 0){
        v_md[v_md.size() - 1].pl_selected = true;
    }

    else if(first_selected >= 0 && first_selected < (int) v_md.size())
        v_md[first_selected].pl_selected = true;

    else if(first_selected >= (int)v_md.size() && v_md.size() > 0)
        v_md[v_md.size() - 1].pl_selected = true;

    _v_md = v_md;
    report_changes(true, false);
}


void Playlist::insert_track(const MetaData& md, int tgt){
    MetaDataList v_md;
    v_md.push_back(md);
    insert_tracks(v_md, tgt);
}


void Playlist::insert_tracks(const MetaDataList& lst, int tgt){

    if(lst.size() == 0) return;
    if(tgt < 0) tgt = 0;

    if(tgt > (int) _v_md.size()){
        tgt = (int) _v_md.size();
    }

   MetaDataList v_md;
   for(int i=0; i<tgt; i++){
       MetaData md = _v_md[i];
       v_md.push_back(md);
   }

   for(uint i=0; i<lst.size(); i++){
       MetaData md = lst[i];
       md.pl_selected = false;
       v_md.push_back(md);
   }

   for(int i=tgt; i<(int) _v_md.size(); i++){
       MetaData md = _v_md[i];
       v_md.push_back(md);
   }

   _v_md = v_md;

   if(tgt <= _cur_play_idx) _cur_play_idx += lst.size();
   report_changes(true, false);
}

void Playlist::selection_changed(const QList<int> & lst){
    for(int i=0; i<(int) _v_md.size(); i++){
        _v_md[i].pl_selected = lst.contains(i);
    }
}


void Playlist::append_track(const MetaData& md){
    MetaDataList v_md;
    v_md.push_back(md);
    append_tracks(v_md);
}

void Playlist::append_tracks(const MetaDataList& lst){

    for(uint i=0; i<lst.size(); i++){
         _v_md.push_back(lst[i]);
    }


    report_changes(true, false);
}

int Playlist::find_track_by_id(int id){
    if(id == -1) return -1;

    int idx = 0;
    foreach(MetaData md, _v_md){
        if(md.id == id) return idx;
        idx++;
    }

    return -1;
}

int Playlist::find_track_by_path(QString path){
    if(path.size() == 0) return -1;

    int idx = 0;

    QString new_path = QDir(path).absolutePath();

    foreach(MetaData md, _v_md){
        QString old_path = QDir(md.filepath).absolutePath();
        if(old_path.compare(new_path, Qt::CaseSensitive) == 0) return idx;
        idx++;
    }

    return -1;
}



void Playlist::replace_track(int idx, const MetaData& md){
    if(idx < 0 || idx >= (int) _v_md.size()) return;

    _v_md[idx] = md;
    report_changes(true, false);
}



int Playlist::get_cur_track(){
    return _cur_play_idx;
}

PlaylistType Playlist::get_type(){
    return _playlist_type;
}

void Playlist::set_playlist_mode(Playlist_Mode mode){
    _playlist_mode = mode;
}

QStringList Playlist::toStringList(){

    QStringList playlist_lst;
    foreach(MetaData md, _v_md){

        if(md.id >= 0) playlist_lst << QString::number(md.id);
        else
            playlist_lst << md.filepath;
    }

    return playlist_lst;
}


bool Playlist::is_empty(){

    return (_v_md.size() == 0);
}
