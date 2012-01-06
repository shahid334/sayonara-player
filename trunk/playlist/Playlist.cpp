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
 */


/*
 * Playlist.cpp
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */

#include "Playlist.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/id3.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/CDirectoryReader.h"

#include <QFile>
#include <QList>
#include <QObject>
#include <QDebug>

#include <iostream>
#include <ctime>

using namespace std;

Playlist::Playlist(QObject * parent) : QObject (parent){



	_playlist_mode = CSettingsStorage::getInstance()->getPlaylistMode();
}

Playlist::~Playlist() {


	// TODO Auto-generated destructor stub
}



void Playlist::ui_loaded(){

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

		emit playlist_created(_v_meta_data, _cur_play_idx);
	}
}



void Playlist::save_playlist_to_storage(){

	QString playlist_str;
	for(uint i=0; i<_v_meta_data.size(); i++){

		playlist_str += QString::number(_v_meta_data[i].id);
		if(i != _v_meta_data.size() - 1) playlist_str += ",";
	}

	CSettingsStorage::getInstance()->setPlaylist(playlist_str);
}






void Playlist::createPlaylist(QStringList& pathlist){

	if(!_playlist_mode.append){
		_v_meta_data.clear();
		_cur_play_idx = -1;
	}

    uint files2fill = pathlist.size();
	for(uint i=0; i<files2fill; i++){

		if(QFile::exists(pathlist[i])){
			MetaData md = ID3::getMetaDataOfFile(pathlist[i]);
			md.is_extern = true;

			_v_extern_tracks.push_back(md);
			_v_meta_data.push_back(md);
		}

		double percent = i * 1.0 / files2fill;
		emit mp3s_loaded_signal((int) (percent * 100));
	}

	emit mp3s_loaded_signal(100);

	save_playlist_to_storage();
	emit playlist_created(_v_meta_data, _cur_play_idx);
}


void Playlist::createPlaylist(vector<MetaData>& v_meta_data){

	vector<MetaData> v_meta_data_tmp;


	for(int i=0; i<v_meta_data.size(); i++){

		MetaData md = v_meta_data.at(i);
		if(Helper::checkTrack(md)){
			v_meta_data_tmp.push_back(md);
		}
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

	save_playlist_to_storage();
	emit playlist_created(_v_meta_data, _cur_play_idx);
}

void Playlist::createPlaylist(CustomPlaylist& pl){
	createPlaylist(pl.tracks);
}




void Playlist::remove_row(int row){


	if(row < _cur_play_idx) _cur_play_idx --;
	else if(row == _cur_play_idx) _cur_play_idx = -1;

	QString filepath = (_v_meta_data.begin() + row)->filepath;
	_v_meta_data.erase(this->_v_meta_data.begin() + row);

	vector<MetaData> v_tmp_extern;

	for(int i=0; i<_v_extern_tracks.size(); i++){
		if(filepath != _v_extern_tracks.at(i).filepath){
			v_tmp_extern.push_back(_v_meta_data.at(i));
		}
	}

	_v_extern_tracks = v_tmp_extern;

	save_playlist_to_storage();
	emit playlist_created(_v_meta_data, _cur_play_idx);
}

void Playlist::directoryDropped(const QString& dir, int row){


	CDirectoryReader reader;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	QStringList fileList;
    int num_files = 0;
    reader.getFilesInsiderDirRecursive(QDir(dir), fileList, num_files);

    vector<MetaData> vec_md;
    foreach(QString filepath, fileList){
    	qDebug() << filepath;
    	MetaData md = ID3::getMetaDataOfFile(filepath);
    	if(db->getTrackByPath(filepath) <= 0){

    		md.is_extern = true;
    		_v_extern_tracks.push_back(md);
    	}

    	else md.is_extern = false;

    	vec_md.push_back(md);
    }

    insert_tracks(vec_md, row);

}

void Playlist::insert_tracks(const vector<MetaData>& v_metadata, int row){

	vector<MetaData> new_vec;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	if(row <= _cur_play_idx && _cur_play_idx != -1)
		_cur_play_idx += v_metadata.size();


	for(int i=0; i<row; i++)
		new_vec.push_back(_v_meta_data.at(i));


	for(uint i=0; i<v_metadata.size(); i++){
		MetaData md = v_metadata.at(i);
		if(db->getTrackByPath(md.filepath) > 0){
			md.is_extern = false;
		}

		else {
			md.is_extern = true;
			_v_extern_tracks.push_back(md);
		}
		new_vec.push_back(md);
	}



	for(uint i=row; i<_v_meta_data.size(); i++)
		new_vec.push_back(_v_meta_data.at(i));

	_v_meta_data.clear();
	_v_meta_data = new_vec;


	save_playlist_to_storage();
	emit playlist_created(_v_meta_data, _cur_play_idx);
}


// not used, but maybe later
void Playlist::insert_albums(const vector<Album>& v_albums, int idx){

	int tmp_idx = idx;
	for(uint i=0; i<v_albums.size(); i++){
		vector<MetaData> vec;
		CDatabaseConnector::getInstance()->getAllTracksByAlbum(v_albums.at(i).id, vec);
		insert_tracks(vec, tmp_idx);
		tmp_idx += vec.size();
	}
}

// not used, but maybe late
void Playlist::insert_artists(const vector<Artist>& v_artists, int idx){
	int tmp_idx = idx;
	for(uint i=0; i<v_artists.size(); i++){
		vector<MetaData> vec;
		CDatabaseConnector::getInstance()->getAllTracksByArtist(v_artists.at(i).id, vec);
		insert_tracks(vec, tmp_idx);
		tmp_idx += vec.size();
	}
}


void Playlist::play(){

	if(_v_meta_data.size() <= 0) return;

	if(_cur_play_idx <= -1){
		_cur_play_idx = 0;
		emit selected_file_changed(_cur_play_idx);
		emit selected_file_changed_md(_v_meta_data[_cur_play_idx]);
	}

	else{
		emit goon_playing();
	}

}

void Playlist::stop(){

	_cur_play_idx = -1;
	emit no_track_to_play();
	emit playlist_created(_v_meta_data, _cur_play_idx);
}

void Playlist::forward(){

	if(_playlist_mode.shuffle){

		int track_num = rand() % _v_meta_data.size();
		_cur_play_idx = track_num;
		emit selected_file_changed(track_num);
		emit selected_file_changed_md(_v_meta_data[track_num]);

	}

	else if(_cur_play_idx < (int) _v_meta_data.size() - 1 && _cur_play_idx >= 0){
		_cur_play_idx++;
		emit selected_file_changed(_cur_play_idx);
		emit selected_file_changed_md(_v_meta_data[_cur_play_idx]);

	}
}

// GUI -->
void Playlist::backward(){


	if(this->_cur_play_idx > 0){
		_cur_play_idx--;
		emit selected_file_changed(_cur_play_idx);
		emit selected_file_changed_md(_v_meta_data[_cur_play_idx]);

	}

}



// --> GUI
void Playlist::next_track(){


	if(_v_meta_data.size() == 0){
		emit no_track_to_play();
		return;
	}

	int track_num;

	if(_playlist_mode.shuffle)
		track_num = rand() % _v_meta_data.size();


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


	if(track_num >= 0){
		MetaData md = _v_meta_data[track_num];
		if(Helper::checkTrack(md)){
			emit selected_file_changed(track_num);
			emit selected_file_changed_md(_v_meta_data[track_num]);
			_cur_play_idx = track_num;
			if(_playlist_mode.dynamic)
				emit search_similar_artists(_v_meta_data[_cur_play_idx].artist);
		}

		else{
			remove_row(track_num);
			next_track();
			emit sig_library_changed();
		}
	}

	else{
		emit no_track_to_play();
	}
}



// GUI -->
void Playlist::change_track(int new_row){
	if( (uint) new_row >= _v_meta_data.size())return;

	MetaData md = _v_meta_data[new_row];
	if(Helper::checkTrack(md)){

		_cur_play_idx = new_row;

		emit selected_file_changed_md(md);

		if(_playlist_mode.dynamic)
			emit search_similar_artists(md.artist);
	}

	else{
		_cur_play_idx = -1;
		remove_row(new_row);
		emit sig_library_changed();
		emit no_track_to_play();
	}
}


// GUI -->
void Playlist::clear_playlist(){

	_v_meta_data.clear();
	_v_extern_tracks.clear();
	_cur_play_idx = -1;

	save_playlist_to_storage();
	emit playlist_created(_v_meta_data, _cur_play_idx);

}

// --> custom playlists
void Playlist::prepare_playlist_for_save(int id){
	emit sig_playlist_prepared(id, _v_meta_data);
}

void Playlist::prepare_playlist_for_save(QString name){
	emit sig_playlist_prepared(name, _v_meta_data);
}


// GUI -->
void Playlist::save_playlist(const QString& filename){


	FILE* file = fopen(filename.toStdString().c_str(), "w");

	if(file){
		qint64 lines = 0;
		for(uint i=0; i<_v_meta_data.size(); i++){
			string str = _v_meta_data.at(i).filepath.toStdString();

			lines += fputs(str.c_str(), file);
			lines += fputs("\n", file);
		}

		fclose(file);
	}

	else{
		qDebug() <<  Q_FUNC_INFO << " File could not be openend; " << filename;
	}
}


// GUI -->
void Playlist::playlist_mode_changed(const Playlist_Mode& playlist_mode){

	CSettingsStorage::getInstance()->setPlaylistMode(playlist_mode);
	_playlist_mode = playlist_mode;
	_playlist_mode.print();

}





void Playlist::edit_id3_request(){
	emit data_for_id3_change(_v_meta_data);
}

void Playlist::id3_tags_changed(vector<MetaData>& new_meta_data){
	_v_meta_data = new_meta_data;
	emit playlist_created(_v_meta_data, _cur_play_idx);
	if(_cur_play_idx >= 0 && _cur_play_idx < (int) _v_meta_data.size())
		emit cur_played_info_changed(_v_meta_data[_cur_play_idx]);
}


void Playlist::similar_artists_available(QList<int>& artists){



	if(artists.size() == 0) return;

	Helper::randomize_list(artists);

	srand ( time(NULL) );


	int cur_artist_idx = 0;
	bool is_track_already_in = false;
	MetaData md;

	do {
		int artist_id = artists.at(cur_artist_idx);
		vector<MetaData> vec_tracks;
		CDatabaseConnector::getInstance()->getAllTracksByArtist(artist_id, vec_tracks);
		qDebug() << "Try artist " << artist_id;

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
	}

	while(is_track_already_in && cur_artist_idx < artists.size());

	qDebug() << "Found artist " << md.artist;

	_v_meta_data.push_back(md);

	save_playlist_to_storage();
	emit playlist_created(_v_meta_data, _cur_play_idx);

}


void Playlist::import_new_tracks_to_library(){

}
