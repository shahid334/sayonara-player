/* PlayManager.cpp */

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



#include "src/PlayManager.h"


PlayManager::PlayManager(QObject* parent) :
	QObject(parent),
	SayonaraClass()
{


	bool load_playlist = _settings->get(Set::PL_Load);
	bool load_last_track = _settings->get(Set::PL_LoadLastTrack);

	if(load_playlist && load_last_track){
		_position = _settings->get(Set::Engine_CurTrackPos_s);
	}

	else{
		_position = 0;
	}
}

PlayManager::~PlayManager(){
}

PlayManager::PlayState PlayManager::get_play_state(){
	return _playstate;
}

quint64 PlayManager::get_cur_position_ms(){
	return _position;
}

quint64 PlayManager::get_duration_ms(){
	return _duration;
}

MetaData PlayManager::get_cur_track(){
	return _md;
}



void PlayManager::play(){

	//if(playlist_dead()) return;

	if(_playstate == PlayManager::PlayState_Stopped && _cur_idx == -1){
		_playstate = PlayManager::PlayState_Playing;
		next();
		return;
	}

	_playstate = PlayManager::PlayState_Playing;

	emit sig_playstate_changed(_playstate);
}

void PlayManager::play_pause(){

	//if(playlist_dead()) return;

	if(_playstate == PlayManager::PlayState_Playing){
		pause();
	}

	else{
		play();
	}
}

void PlayManager::pause(){

	//if(playlist_dead()) return;

	if(_playstate == PlayManager::PlayState_Stopped){
		_playstate = PlayManager::PlayState_Paused;
		next();
		return;
	}

	_playstate = PlayManager::PlayState_Paused;

	emit sig_playstate_changed(_playstate);
}

void PlayManager::previous(){
	//if(playlist_dead()) return;
	emit sig_previous();
}

void PlayManager::next(){
	//if(playlist_dead()) return;
	emit sig_next();
}

void PlayManager::stop(){

	_md = MetaData();
	_cur_idx = -1;
	_playstate = PlayManager::PlayState_Stopped;
	emit sig_playstate_changed(_playstate);
}

void PlayManager::record(bool b){
    emit sig_record(b);
}

void PlayManager::wait(){

	//if(playlist_dead()) return;
	_playstate = PlayManager::PlayState_Wait;
	emit sig_playstate_changed(PlayManager::PlayState_Paused);
}

void PlayManager::seek_rel(double percent){
	emit sig_seeked_rel(percent);
}

void PlayManager::seek_rel_ms(quint64 ms){
	emit sig_seeked_rel_ms(ms);
}

void PlayManager::seek_abs_ms(quint64 ms){
	emit sig_seeked_abs_ms(ms);
}

void PlayManager::set_position_ms(quint64 ms){
	_position = ms;
	emit sig_position_changed_ms(ms);
}

void PlayManager::duration_changed(quint64 duration_ms){
	_duration = duration_ms;
	emit sig_duration_changed(_duration);
}


void PlayManager::change_track(const MetaData& md){
	_md = md;
	_duration = md.length_ms;
	emit sig_track_changed(md);
}

void PlayManager::change_track_idx(int idx){

	_cur_idx = idx;
	_settings->set(Set::PL_LastTrack, _cur_idx);

	if(_cur_idx >= 0){

		switch(_playstate){

			case PlayManager::PlayState_Wait:
				pause();
				break;

			default:
				play();
		}
	}

	else {
		qDebug() << "Playlist finished";
		emit sig_playlist_finished();
		stop();
	}

	emit sig_track_idx_changed(_cur_idx);
}



bool PlayManager::playlist_dead(){

	return (_playstate == PlayManager::PlayState_Stopped);
}

