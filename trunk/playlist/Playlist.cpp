/* Playlist.cpp */

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
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */

#include "Playlist.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/id3.h"
#include "HelperStructs/PlaylistParser.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/CDirectoryReader.h"

#include <QFile>
#include <QList>
#include <QObject>
#include <QDate>
#include <QTime>
#include <QDebug>

#include <iostream>
#include <ctime>

using namespace std;

Playlist::Playlist(QObject * parent) : QObject (parent){

	_radio_active = false;
	_playlist_mode = CSettingsStorage::getInstance()->getPlaylistMode();
	_db = CDatabaseConnector::getInstance();
	_v_meta_data.clear();
}

Playlist::~Playlist() {

}


// call this function if all extern signals/slots for this class are defined
void Playlist::ui_loaded(){

	_v_meta_data.clear();
	bool loadPlaylist = CSettingsStorage::getInstance()->getLoadPlaylist();

	if( loadPlaylist ){

		QString saved_playlist = CSettingsStorage::getInstance()->getPlaylist();
		QStringList list = saved_playlist.split(',');

		if(list.size() > 0){

			foreach(QString trackid, list){
				if(trackid == "-1" || trackid == "") continue;
				MetaData track = CDatabaseConnector::getInstance()->getTrackById(trackid.toInt());
				_v_meta_data.push_back(track);
			}
			_cur_play_idx = -1;
		}

		emit sig_playlist_created(_v_meta_data, _cur_play_idx);
	}
}



// save the playlist, for possibly reloading it on next startup
void Playlist::psl_save_playlist_to_storage(){

	if(_radio_active){
		return;
	}

	QString playlist_str;
	for(uint i=0; i<_v_meta_data.size(); i++){

		playlist_str += QString::number(_v_meta_data[i].id);
		if(i != _v_meta_data.size() - 1) playlist_str += ",";
	}

	CSettingsStorage::getInstance()->setPlaylist(playlist_str);
}



// create a playlist, where metadata is already available
void Playlist::psl_createPlaylist(vector<MetaData>& v_meta_data, int radio){

	// no tracks in new playlist
	if(v_meta_data.size() == 0) {
		_v_meta_data.clear();
		return;
	}




	bool played_radio = (_radio_active != RADIO_OFF);
	if(radio != _radio_active){
		psl_stop();
	}

	_radio_active = radio;
	emit sig_radio_active(radio);

	vector<MetaData> v_meta_data_tmp;

	for(uint i=0; i<v_meta_data.size(); i++){

		MetaData md = v_meta_data.at(i);
		if( checkTrack(md) )
			v_meta_data_tmp.push_back(md);
	}


	v_meta_data = v_meta_data_tmp;

	if(!_playlist_mode.append){
		_v_meta_data.clear();
		_v_meta_data = v_meta_data;
		_cur_play_idx = -1;
	}

	else{
		for(uint i=0; i<v_meta_data.size(); i++){
			_v_meta_data.push_back(v_meta_data.at(i));
		}
	}

	if(_radio_active == RADIO_OFF)
		psl_save_playlist_to_storage();


	emit sig_playlist_created(_v_meta_data, _cur_play_idx);

	if(v_meta_data.size() == 0)
			emit sig_no_track_to_play();

	// if radio currently plays or was playing until now
	else if(_radio_active == RADIO_LFM || _radio_active == RADIO_STATION || played_radio){

		emit sig_selected_file_changed(0);
		emit sig_selected_file_changed_md(_v_meta_data[0]);
	}
}



// create a new playlist, where only filepaths are given
// Load Folder, Load File...
void Playlist::psl_createPlaylist(QStringList& pathlist, int radio){

	if(radio != _radio_active)
		psl_stop();

	_radio_active = radio;
	emit sig_radio_active(radio);

	if(!_playlist_mode.append){
		_v_meta_data.clear();
		_cur_play_idx = -1;
	}

    uint files2fill = pathlist.size();


    vector<MetaData> v_md_from_playlist;

    // parse playlist sound files
    for(uint i=0; i<files2fill; i++){

    	if(!QFile::exists(pathlist[i])) continue;
		if(!PlaylistParser::is_supported_playlist(pathlist[i])) continue;

		PlaylistParser::parse_playlist(pathlist[i], v_md_from_playlist);

    }

    // push titles from playlist into playlist
    for(uint i=0; i<v_md_from_playlist.size(); i++){

    	MetaData md = v_md_from_playlist[i];
    	QString filepath = md.filepath.trimmed();

		if(_db->getTrackByPath(filepath) <= 0){
			md.is_extern = true;
			_v_extern_tracks.push_back(md);
		}

		else md.is_extern = false;

		_v_meta_data.push_back(md);

    }

    // parse pure sound files
    // throw those files away, that were already loaded by a m3u, pls or different playlist
    for(uint i=0; i<files2fill; i++){
    	MetaData md;
    	// only pure sound files are allowed
    	if(!Helper::is_soundfile(pathlist[i])) continue;
		if(!ID3::getMetaDataOfFile(pathlist[i], md)) continue;

		bool already_found_in_pl = false;

		// look in playlist files
		for(uint j=0; j<v_md_from_playlist.size(); j++){

			if(v_md_from_playlist[j].filepath == md.filepath){
				already_found_in_pl = true;
				break;
			}
		}

		if(!already_found_in_pl){
			QString filepath = md.filepath.trimmed();

			if(_db->getTrackByPath(filepath) <= 0){
				md.is_extern = true;
				_v_extern_tracks.push_back(md);
			}

			else md.is_extern = false;

			_v_meta_data.push_back(md);
		}

		double percent = i * 1.0 / files2fill;
		emit sig_mp3s_loaded_signal((int) (percent * 100));

    }


	emit sig_mp3s_loaded_signal(100);

	psl_save_playlist_to_storage();

	emit sig_playlist_created(_v_meta_data, _cur_play_idx);
}


// create playlist from saved custom playlist
void Playlist::psl_createPlaylist(CustomPlaylist& pl, int radio){
	psl_createPlaylist(pl.tracks, radio);
}


void Playlist::remove_row(int row){
	QList<int> remove_list;
	remove_list << row;
	psl_remove_rows(remove_list);
}

// remove one row
void Playlist::psl_remove_rows(const QList<int> & rows){

	vector<MetaData> v_tmp_md;
	vector<MetaData> v_tmp_extern;

	int n_tracks = _v_meta_data.size();
	int n_tracks_extern = _v_extern_tracks.size();

	bool* to_delete = new bool[n_tracks];
	bool* to_delete_extern = new bool[n_tracks_extern];

	for(int i=0; i<n_tracks_extern; i++){
		to_delete_extern[i] = false;
	}

	for(int i=0; i<n_tracks; i++){
		to_delete[i] = false;
		if(rows.contains(i)) {
			to_delete[i] = true;
			for(int j=0; j<n_tracks_extern; j++){
				MetaData e_md = _v_extern_tracks[j];
				if(e_md.filepath.toLower().trimmed() == _v_meta_data[i].filepath.toLower().trimmed()){
					to_delete_extern[j] = true;
				}
			}
		}
	}

	for(int i=0; i<n_tracks; i++){
		if(!to_delete[i]){
			v_tmp_md.push_back(_v_meta_data[i]);
			if(i < _cur_play_idx) _cur_play_idx--;
			else if(i == _cur_play_idx) _cur_play_idx = -1;
		}
	}

	if(_cur_play_idx < -1) _cur_play_idx = -1;

	for(int i=0; i<n_tracks_extern; i++){
		if(!to_delete_extern[i])
			v_tmp_extern.push_back(_v_extern_tracks[i]);
	}

	_v_meta_data = v_tmp_md;
	_v_extern_tracks = v_tmp_extern;

	psl_save_playlist_to_storage();
	emit sig_playlist_created(_v_meta_data, _cur_play_idx);

	delete to_delete;
	delete to_delete_extern;
}


// a directory was dropped from extern desktop
void Playlist::psl_directoryDropped(const QString& dir, int row){
/*
	if(_radio_active != RADIO_OFF){
		psl_stop();
		row = 0;
	}

	CDirectoryReader reader;
	QStringList fileList;
	vector<MetaData> vec_md;
	int num_files = 0;


	reader.getFilesInsiderDirRecursive(QDir(dir), fileList, num_files);

    foreach(QString filepath, fileList){
    	MetaData md;
    	if(!ID3::getMetaDataOfFile(filepath, md)) continue;

    	if( _db->getTrackByPath(filepath) <= 0 ){

    		md.is_extern = true;
    		_v_extern_tracks.push_back(md);
    	}

    	else md.is_extern = false;

    	vec_md.push_back(md);
    }

    psl_insert_tracks(vec_md, row);
*/

}

// insert tracks (also drag & drop)
void Playlist::psl_insert_tracks(const vector<MetaData>& v_metadata, int row){

	// turn off radio
	bool switched_from_radio = false;
	if(_radio_active != RADIO_OFF){
		switched_from_radio = true;
		psl_stop();
		row = 0;
	}

	vector<MetaData> new_vec;

	// possibly the current playing index has to be updated
	if(row <= _cur_play_idx && _cur_play_idx != -1)
		_cur_play_idx += v_metadata.size();

	// all titles before the current row stay as they are
	for(int i=0; i<row; i++){
		new_vec.push_back(_v_meta_data.at(i));
	}

	// insert new tracks
	for(uint i=0; i<v_metadata.size(); i++){
		MetaData md = v_metadata.at(i);
		if( _db->getTrackByPath(md.filepath) > 0 ){
			md.is_extern = false;
		}

		else {
			md.is_extern = true;
			_v_extern_tracks.push_back(md);
		}
		new_vec.push_back(md);
	}

	// append all old tracks with higher index as row
	for(uint i=row; i<_v_meta_data.size(); i++)
		new_vec.push_back(_v_meta_data.at(i));

	_v_meta_data.clear();
	_v_meta_data = new_vec;


	psl_save_playlist_to_storage();
	emit sig_playlist_created(_v_meta_data, _cur_play_idx);

	// radio was turned off, so we start at beginning of playlist
	if(switched_from_radio && _v_meta_data.size() > 0){
		_cur_play_idx = 0;
		emit sig_selected_file_changed(0);
		emit sig_selected_file_changed_md(_v_meta_data[0]);
	}
}


// not used, but maybe later
void Playlist::psl_insert_albums(const vector<Album>& v_albums, int idx){

	int tmp_idx = idx;
	for(uint i=0; i<v_albums.size(); i++){
		vector<MetaData> vec;
		_db->getAllTracksByAlbum(v_albums.at(i).id, vec);
		psl_insert_tracks(vec, tmp_idx);
		tmp_idx += vec.size();
	}
}

// not used, but maybe late
void Playlist::psl_insert_artists(const vector<Artist>& v_artists, int idx){
	int tmp_idx = idx;
	for(uint i=0; i<v_artists.size(); i++){
		vector<MetaData> vec;
		_db->getAllTracksByArtist(v_artists.at(i).id, vec);
		psl_insert_tracks(vec, tmp_idx);
		tmp_idx += vec.size();
	}
}


// play a track
void Playlist::psl_play(){

	if(_v_meta_data.size() <= 0) return;

	// state was stop until now
	if(_cur_play_idx <= -1){

		int track_num = 0;
		MetaData md = _v_meta_data[track_num];

		if( checkTrack(md) ){
			_cur_play_idx = track_num;

			emit sig_selected_file_changed(track_num);
			emit sig_selected_file_changed_md(md);

			if(_playlist_mode.dynamic)
				emit sig_search_similar_artists(md.artist);
		}
	}

	else{
		emit sig_goon_playing();
	}

}

void Playlist::psl_stop(){

	if(_radio_active != RADIO_OFF){

		save_stream_playlist();
		psl_clear_playlist();
	}

	_radio_active = RADIO_OFF;
	emit sig_radio_active(_radio_active);

	_cur_play_idx = -1;
	emit sig_no_track_to_play();
	emit sig_playlist_created(_v_meta_data, _cur_play_idx);
}

// fwd was pressed -> next track
void Playlist::psl_forward(){

	MetaData md;
	int track_num = 0;


	// lastfm removes current played file
	// next file is also on idx 0
	if(_radio_active == RADIO_LFM){

		remove_row(0);

		// any more files to play?
		if(_v_meta_data.size() == 0){
			emit sig_no_track_to_play();
			emit sig_need_more_radio();
			return;
		}

		_cur_play_idx = 0;
		emit sig_selected_file_changed(0);
		emit sig_selected_file_changed_md(_v_meta_data[0]);
		return;
	}

	// this shouldn't happen, because forward is disabled
	else if(_radio_active == RADIO_STATION){
		return;
	}


	// SHUFFLE
	if(_playlist_mode.shuffle){
		track_num = rand() % _v_meta_data.size();
	}

	// new track within normal playlist
	else if(_cur_play_idx < (int) _v_meta_data.size() - 1 && _cur_play_idx >= 0){

		if( _playlist_mode.repAll )
			track_num = (_cur_play_idx + 1) % _v_meta_data.size();
		else
			track_num = (_cur_play_idx + 1);
	}

	md = _v_meta_data[track_num];

	if( checkTrack(md) ){

		_cur_play_idx = track_num;

		emit sig_selected_file_changed(track_num);
		emit sig_selected_file_changed_md(md);

		if(_playlist_mode.dynamic)
			emit sig_search_similar_artists(md.artist);
	}
}

// GUI -->
void Playlist::psl_backward(){

	// this shouldn't happen, because backward is disabled
	if(_radio_active != RADIO_OFF) return;

	if(this->_cur_play_idx <= 0) return;

	int track_num = _cur_play_idx - 1;
	MetaData md = _v_meta_data[track_num];

	if( checkTrack(md) ){
		emit sig_selected_file_changed(track_num);
		emit sig_selected_file_changed_md(md);
		_cur_play_idx = track_num;
		if(_playlist_mode.dynamic)
			emit sig_search_similar_artists(md.artist);
	}

}



// --> GUI
void Playlist::psl_next_track(){

	// radio station is over?
	if(_radio_active == RADIO_STATION){
		emit sig_no_track_to_play();
		return;
	}

	// no track to play anymore
	// if LFM, fetch more tracks
	if(_v_meta_data.size() == 0){
		emit sig_no_track_to_play();
		if(_radio_active == RADIO_LFM){
			emit sig_need_more_radio();
		}
		return;
	}


	int track_num = -1;
	// LFM next track
	if(_radio_active == RADIO_LFM){

		// start screaming for new tracks
		if(_v_meta_data.size() == 1)
			emit sig_need_more_radio();




		remove_row(0);
		// we just played the last track
		if(_v_meta_data.size() == 0){
			emit sig_no_track_to_play();
			emit sig_need_more_radio();
		}

		else
			track_num = 0;
	}


	// shuffle mode
	else if(_playlist_mode.shuffle){
		track_num = rand() % _v_meta_data.size();
		if(track_num == _cur_play_idx){
			track_num = (_cur_play_idx + 1) % _v_meta_data.size();
		}
	}


	// standard next track, also valid for stream stations
	else {

		// last track
		if(_cur_play_idx >= (int) _v_meta_data.size() -1){
			track_num = -1;

			if(_playlist_mode.repAll)
				track_num = 0;
		}

		else
			track_num = _cur_play_idx + 1;
	}


	// valid next track
	if(track_num >= 0){
		MetaData md = _v_meta_data[track_num];

		// maybe track is deleted here
		if( checkTrack(md) ){
			emit sig_selected_file_changed(track_num);
			emit sig_selected_file_changed_md(_v_meta_data[track_num]);
			_cur_play_idx = track_num;
			if(_playlist_mode.dynamic)
				emit sig_search_similar_artists(md.artist);
		}

		else{
			remove_row(track_num);
			psl_next_track();
		}
	}

	// something weird happened, we have less than 1 track
	else{
		if(_radio_active == RADIO_LFM)
			sig_need_more_radio();
		emit sig_no_track_to_play();

	}
}



// GUI -->
void Playlist::psl_change_track(int new_row){

	if( (uint) new_row >= _v_meta_data.size())return;

	MetaData md = _v_meta_data[new_row];
	if( checkTrack(md) ){

		// never should gonna happen...
		// we cannot click into playlist
		if(_radio_active == RADIO_LFM){
			for(int i=0; i<new_row; i++){
				remove_row(0);
			}

			if(_v_meta_data.size() == 0){
				emit sig_need_more_radio();
				return;
			}

			md = _v_meta_data[0];

			_cur_play_idx = 0;
		}


		else{
			_cur_play_idx = new_row;
		}

		emit sig_selected_file_changed_md(md);

		if(_playlist_mode.dynamic && _radio_active == RADIO_OFF)
			emit sig_search_similar_artists(md.artist);
	}

	else{
		_db->deleteTrack(md);
		_cur_play_idx = -1;
		remove_row(new_row);
		emit sig_no_track_to_play();
	}
}


// GUI -->
void Playlist::psl_clear_playlist(){

	_v_stream_playlist.clear();
	_v_meta_data.clear();
	_v_extern_tracks.clear();
	_cur_play_idx = -1;

	if(_radio_active == RADIO_OFF)
		psl_save_playlist_to_storage();

	emit sig_playlist_created(_v_meta_data, _cur_play_idx);

}

// --> custom playlists
void Playlist::psl_prepare_playlist_for_save(int id){
	emit sig_playlist_prepared(id, _v_meta_data);
}

void Playlist::psl_prepare_playlist_for_save(QString name){
	emit sig_playlist_prepared(name, _v_meta_data);
}


// GUI -->
void Playlist::psl_save_playlist(const QString& filename, const vector<MetaData>& v_md){

	FILE* file = fopen(filename.toStdString().c_str(), "w");

	if(file){
		qint64 lines = 0;
		for(uint i=0; i<v_md.size(); i++){
			string str = v_md.at(i).filepath.toStdString();

			lines += fputs(str.c_str(), file);
			lines += fputs("\n", file);
		}

		fclose(file);
	}
}


// GUI -->
void Playlist::psl_playlist_mode_changed(const Playlist_Mode& playlist_mode){

	CSettingsStorage::getInstance()->setPlaylistMode(playlist_mode);
	_playlist_mode = playlist_mode;
	_playlist_mode.print();

}


void Playlist::psl_edit_id3_request(){
	emit sig_data_for_id3_change(_v_meta_data);
}

void Playlist::psl_id3_tags_changed(vector<MetaData>& new_meta_data){

	for(uint i=0; i<_v_meta_data.size(); i++){
		MetaData md_old = _v_meta_data[i];
		for(uint j=0; j<new_meta_data.size(); j++){
			MetaData md_new = new_meta_data[j];
			//qDebug() << "compare " << md_old.filepath.toLower().trimmed() << " with " << md_new.filepath.toLower().trimmed() << md_old.filepath.toLower().trimmed().compare(md_new.filepath.toLower().trimmed());
			if(md_old.filepath.toLower().trimmed().compare(md_new.filepath.toLower().trimmed()) == 0){
				_v_meta_data.at(i) = md_new;
			}
		}
	}

	emit sig_playlist_created(_v_meta_data, _cur_play_idx);

	if(_cur_play_idx >= 0 && _cur_play_idx < (int) _v_meta_data.size())
		emit sig_cur_played_info_changed(_v_meta_data[_cur_play_idx]);
}


void Playlist::psl_similar_artists_available(const QList<int>& artists){


	// the response came too late, we already switched to radio
	if(_radio_active != RADIO_OFF ){

		return;
	}

	if(artists.size() == 0) return;

	QList<int> artists_copy = Helper::randomize_list(artists);

	srand ( time(NULL) );


	int cur_artist_idx = 0;
	bool is_track_already_in = false;
	MetaData md;

	do {
		int artist_id = artists_copy.at(cur_artist_idx);
		vector<MetaData> vec_tracks;
		_db->getAllTracksByArtist(artist_id, vec_tracks);

		// give each artist several trys
		int max_rounds = vec_tracks.size();
		for(int rounds=0; rounds < max_rounds; rounds++){
			is_track_already_in = false;
			int rnd_track = (rand() % vec_tracks.size());
			md = vec_tracks.at(rnd_track);

			// search playlist
			for(uint i=0; i<_v_meta_data.size(); i++){
				if(_v_meta_data[i].id == md.id){
					is_track_already_in = true;
					break;
				}
			}

			if(!is_track_already_in) break;

		}

		cur_artist_idx++;
	} while(is_track_already_in && cur_artist_idx < artists_copy.size());

	if(!is_track_already_in)
		_v_meta_data.push_back(md);

	psl_save_playlist_to_storage();
	emit sig_playlist_created(_v_meta_data, _cur_play_idx);

}


void Playlist::psl_import_new_tracks_to_library(bool copy){

	Q_UNUSED(copy);

	if(_v_extern_tracks.size() == 0) return;
	emit sig_import_files(_v_extern_tracks);

}

void Playlist::psl_import_result(bool success){

	_v_extern_tracks.clear();
	if(success){

		foreach(MetaData md, _v_meta_data){
			md.is_extern = false;
		}

		psl_createPlaylist(_v_meta_data);
	}

}


void Playlist::psl_new_radio_playlist_available(const vector<MetaData>& playlist){

	_radio_active = RADIO_LFM;
	_cur_play_idx = 0;
	emit sig_radio_active(_radio_active);

	vector<MetaData> pl_copy = playlist;
	this->psl_clear_playlist();
	psl_createPlaylist(pl_copy, _radio_active);
}


void Playlist::psl_play_stream(const QString& url, const QString& name){

	// playlist radio
	if(PlaylistParser::is_supported_playlist(url)){
		vector<MetaData> v_md;
		if(PlaylistParser::parse_playlist(url, v_md) > 0){

			for(uint i=0; i<v_md.size(); i++){
				if(name.size() > 0)
					v_md.at(i).title = name;
				else v_md.at(i).title = "Radio Station";
			}

			psl_createPlaylist(v_md, RADIO_STATION);
		}

		return;
	}

	// real stream
	else{
		vector<MetaData> v_md;
		MetaData md;

		if(name.size() > 0) md.title = name;
		else md.title = "Radio Station";

		md.artist = url;
		md.filepath = url;
		v_md.push_back(md);
		psl_createPlaylist(v_md, RADIO_STATION);

		return;
	}
}

void Playlist::psl_valid_strrec_track(const MetaData& md){
	qDebug() << "push back " << md.title;
	_v_stream_playlist.push_back(md);
}

void Playlist::save_stream_playlist(){

	if(_v_stream_playlist.size() == 0) {
		qDebug() << "Stream playlist is empty";
		return;
	}



	QString title = "Radio_" + QDate::currentDate().toString("yyyy-mm-dd");
	title += QTime::currentTime().toString("hh.mm");
	QString dir = CSettingsStorage::getInstance()->getStreamRipperPath() + QDir::separator();

	psl_save_playlist(dir + title + ".m3u", _v_stream_playlist);

	/*qDebug() << "found " << _v_stream_playlist.size() << " items";
	sig_playlist_prepared(title, _v_stream_playlist);*/
}


bool  Playlist::checkTrack(const MetaData& md){
	if(Helper::checkTrack(md)) return true;
	else {
		MetaData md_cp = md;
		_db->deleteTrack(md_cp);
		emit sig_library_changed();
		return false;
	}
}

void Playlist::psl_play_next_tracks(const vector<MetaData>& v_md){
	psl_insert_tracks(v_md, _cur_play_idx + 1);
}
