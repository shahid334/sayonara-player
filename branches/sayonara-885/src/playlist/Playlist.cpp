/* Playlist.cpp */

/* Copyright (C) 2011-2014  Lucio Carreras
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

Playlist::Playlist(int idx, QObject* parent) :
	QObject(parent),
	SayonaraClass()
{
	_playlist_idx = idx;
	_playlist_mode = _settings->get(Set::PL_Mode);
    _reports_disabled = false;
    _cur_play_idx = -1;
}

void Playlist::report_changes(bool pl_changed, bool track_changed) {
    if(_reports_disabled) return;

	for(int i=0; i<_v_md.size(); i++){
		if(_v_md[i].pl_selected && i == 11){
			qDebug() << "Report changes " << i << ": true";
		}
	}

	if(pl_changed){
		emit sig_playlist_changed(this);
	}


	if(track_changed) {

        if( _cur_play_idx < 0 ||
            _cur_play_idx >= _v_md.size() ){

			emit sig_stopped();
		}

		else{
			MetaData md = _v_md[_cur_play_idx];
			emit sig_track_changed(md, _cur_play_idx);
		}
    }
}

void Playlist::enable_reports() {
    _reports_disabled = false;
}

void Playlist::disable_reports() {
    _reports_disabled = true;
}

void Playlist::clear() {

    _v_md.clear();
    _cur_play_idx = -1;
    report_changes(true, false);
}

void Playlist::move_track(const int idx, int tgt) {
    QList<int> lst;
    lst << idx;
    move_tracks(lst, tgt);
}

void Playlist::move_tracks(const QList<int>& lst, int tgt) {

    MetaDataList v_md_to_move;
    MetaDataList v_md_before_tgt;
    MetaDataList v_md_after_tgt;

    for(int i=0; i< _v_md.size(); i++) {

        if(!lst.contains(i) && i < tgt)
            v_md_before_tgt.push_back(_v_md[i]);

        else if(!lst.contains(i) && i >= tgt)
            v_md_after_tgt.push_back(_v_md[i]);

        else if(lst.contains(i))
            v_md_to_move.push_back(_v_md[i]);
    }

    _v_md.clear();
    _cur_play_idx = -1;

    foreach(MetaData md, v_md_before_tgt) {
        md.pl_selected = false;
        _v_md.push_back(md);
        if(md.pl_playing) _cur_play_idx = _v_md.size() - 1;
    }

    foreach(MetaData md, v_md_to_move) {
        md.pl_selected = true;
        _v_md.push_back(md);
        if(md.pl_playing) _cur_play_idx = _v_md.size() - 1;
    }

    foreach(MetaData md, v_md_after_tgt) {
        md.pl_selected = false;
        _v_md.push_back(md);
        if(md.pl_playing) _cur_play_idx = _v_md.size() - 1;
    }

    report_changes(true, false);
}


void Playlist::delete_track(const int idx) {
    QList<int> to_remove;
    to_remove << idx;
    delete_tracks(to_remove);
}

void Playlist::delete_tracks(const QList<int>& lst) {

    if(lst.size() == 0) return;

    MetaDataList v_md;
    int first_selected = -1;

    _cur_play_idx = -1;

    for(int i=0; i< _v_md.size(); i++) {

        MetaData md = _v_md[i];
        // do not delete
        if( !lst.contains(i) ) {

			if(md.pl_selected && first_selected == -1) {
				first_selected = i;
			}

            md.pl_selected = false;
            v_md.push_back(md);

            if(md.pl_playing) {
                _cur_play_idx = (v_md.size() -1);
            }
        }
    }

    if( first_selected == -1 &&
        (v_md.size() > lst[0]) ) {
        v_md[lst[0]].pl_selected = true;
    }

    else if( first_selected == -1 &&
             (v_md.size() <= lst[0]) &&
             (v_md.size() > 0) ) {

        v_md[v_md.size() - 1].pl_selected = true;
    }

    else if(first_selected >= 0 && first_selected < v_md.size())
        v_md[first_selected].pl_selected = true;

    else if(first_selected >= (int)v_md.size() && v_md.size() > 0)
        v_md[v_md.size() - 1].pl_selected = true;

    _v_md = v_md;
    report_changes(true, false);
}


void Playlist::insert_track(const MetaData& md, int tgt) {
    MetaDataList v_md;
    v_md.push_back(md);
    insert_tracks(v_md, tgt);
}


void Playlist::insert_tracks(const MetaDataList& lst, int tgt) {

    if(lst.size() == 0) return;
    if(tgt < 0) tgt = 0;

    if(tgt > _v_md.size()) {
        tgt = _v_md.size();
    }

   MetaDataList v_md;
   for(int i=0; i<tgt; i++) {
       const MetaData& md = _v_md[i];
       v_md.push_back(md);
   }

   for(int i=0; i<lst.size(); i++) {
       MetaData md = lst[i];

       md.is_disabled = !(Helper::checkTrack(md));
       md.pl_selected = false;
       v_md.push_back(md);
   }

   for(int i=tgt; i< _v_md.size(); i++) {

       const MetaData& md = _v_md[i];
       v_md.push_back(md);
   }

   _v_md = v_md;

   if(tgt <= _cur_play_idx) _cur_play_idx += lst.size();
   report_changes(true, false);
}

void Playlist::selection_changed(const QList<int>& lst) {

	_selected_tracks.clear();

	for(int i=0; i<_v_md.size(); i++){

		_v_md[i].pl_selected = lst.contains(i);
		if(_v_md[i].pl_selected){
			qDebug() << "Playlist: Set " << i << ": true";
		}

		if(_v_md[i].pl_selected){

			_selected_tracks << _v_md[i];

		}
    }
}


void Playlist::append_track(const MetaData& md) {
    MetaDataList v_md;
    v_md.push_back(md);
    append_tracks(v_md);
}

void Playlist::append_tracks(const MetaDataList& lst) {

    foreach(MetaData md, lst){
        md.is_disabled = !(Helper::checkTrack(md));
        _v_md.push_back(md);
    }

    report_changes(true, false);
}


void Playlist::replace_track(int idx, const MetaData& md) {

    if(idx < 0 || idx >= _v_md.size()) return;

	bool is_selected = _v_md[idx].pl_selected;
	bool is_dragged = _v_md[idx].pl_dragged;
	bool is_playing = _v_md[idx].pl_playing;

    _v_md[idx] = md;
    _v_md[idx].is_disabled = !(Helper::checkTrack(md));
	_v_md[idx].pl_selected = is_selected;
	_v_md[idx].pl_dragged = is_dragged;
	_v_md[idx].pl_playing = is_playing;

    report_changes(true, false);
}

int Playlist::get_cur_track() const {
    return _cur_play_idx;
}

PlaylistType Playlist::get_type() const {
    return _playlist_type;
}

int Playlist::get_idx() const {
	return _playlist_idx;
}

void Playlist::set_playlist_mode(const PlaylistMode& mode) {
    _playlist_mode = mode;
}

PlaylistMode Playlist::get_playlist_mode() const {
	return _playlist_mode;
}


bool Playlist::is_empty() const {

    return (_v_md.size() == 0);
}


PlaylistMode Playlist::playlist_mode_backup(){
	_playlist_mode_backup = _playlist_mode;

	return _playlist_mode;
}

PlaylistMode Playlist::playlist_mode_restore(){
	_playlist_mode = _playlist_mode_backup;

	return _playlist_mode;
}


QStringList Playlist::toStringList() const {
    return _v_md.toStringList();
}

QList<int> Playlist::find_tracks(int idx) const {
	return _v_md.findTracks(idx);
}

QList<int> Playlist::find_tracks(const QString& filepath) const {
	return _v_md.findTracks(filepath);
}
