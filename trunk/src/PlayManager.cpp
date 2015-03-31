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

}

PlayManager::~PlayManager(){
}

PlayManager::PlayState PlayManager::get_play_state(){
	return _playstate;
}


void PlayManager::play(){

	if(playlist_dead()) return;

	if(_playstate == PlayManager::PlayState_Stopped && _cur_idx == -1){
		_playstate = PlayManager::PlayState_Playing;
		next();
		return;
	}

	_playstate = PlayManager::PlayState_Playing;

	emit sig_playstate_changed(_playstate);
}

void PlayManager::play_pause(){

	if(playlist_dead()) return;

	if(_playstate == PlayManager::PlayState_Playing){
		pause();
	}

	else{
		play();
	}
}

void PlayManager::pause(){

	if(playlist_dead()) return;

	if(_playstate == PlayManager::PlayState_Stopped){
		_playstate = PlayManager::PlayState_Paused;
		next();
		return;
	}

	_playstate = PlayManager::PlayState_Paused;

	emit sig_playstate_changed(_playstate);
}

void PlayManager::previous(){
	if(playlist_dead()) return;
	emit sig_previous();
}

void PlayManager::next(){
	if(playlist_dead()) return;
	emit sig_next();
}

void PlayManager::stop(){

	_cur_idx = -1;
	_playstate = PlayManager::PlayState_Stopped;
	emit sig_playstate_changed(_playstate);
}

void PlayManager::wait(){

	if(playlist_dead()) return;
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
	emit sig_position_changed_ms(ms);
}


void PlayManager::change_track(const MetaData& md){
	emit sig_track_changed(md);
}

void PlayManager::change_track_idx(int idx){

	_cur_idx = idx;
	_settings->set(Set::PL_LastTrack, _cur_idx);

	if( _cur_idx >= 0 && _cur_idx < _playlist_len){

		switch(_playstate){

			case PlayManager::PlayState_Wait:
				pause();
				break;

			default:
				play();
		}
	}

	else {
		stop();
	}

	emit sig_track_idx_changed(_cur_idx);
}


void PlayManager::playlist_changed(const MetaDataList& v_md){

	int playlist_len = v_md.size();

	if( playlist_dead() &&
		playlist_len > 0)
	{
		change_track_idx(0);
		change_track(v_md[0]);

		play();
	}

	_playlist_len = playlist_len;

	emit sig_playlist_changed(playlist_len);
}


bool PlayManager::playlist_dead(){

	if( _playstate == PlayManager::PlayState_Stopped &&
		_playlist_len == 0 )
	{
		return true;
	}

	return false;
}
