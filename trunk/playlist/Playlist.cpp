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

#include <QFile>
#include <QObject>

#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>

#include <iostream>
#include <ctime>

using namespace std;

Playlist::Playlist(QObject * parent) : QObject (parent){

}

Playlist::~Playlist() {


	// TODO Auto-generated destructor stub
}



void Playlist::createPlaylist(QStringList& pathlist){

	if(!_playlist_mode.append){
		_v_meta_data.clear();
		_cur_play_idx = -1;
	}
    uint files2fill = pathlist.size();
	for(uint i=0; i<files2fill; i++){

		MetaData md = ID3::getMetaDataOfFile(pathlist[i]);
		md.is_extern = true;
		_v_meta_data.push_back(md);

		double percent = i * 1.0 / files2fill;
		emit mp3s_loaded_signal((int) (percent * 100));
	}

	emit mp3s_loaded_signal(100);
	emit playlist_created(_v_meta_data, _cur_play_idx);
}


void Playlist::createPlaylist(vector<MetaData>& v_meta_data){

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


	emit playlist_created(_v_meta_data, _cur_play_idx);


}




void Playlist::remove_row(int row){


	if(row < _cur_play_idx) _cur_play_idx --;
	else if(row == _cur_play_idx) _cur_play_idx = -1;

	_v_meta_data.erase(this->_v_meta_data.begin() + row);

	emit playlist_created(_v_meta_data, _cur_play_idx);
}





void Playlist::insert_tracks(const vector<MetaData>& v_metadata, int row){

	vector<MetaData> new_vec;
	if(row < _cur_play_idx && _cur_play_idx != -1)
		_cur_play_idx += v_metadata.size();


	for(int i=0; i<row; i++)
		new_vec.push_back(_v_meta_data.at(i));


	for(uint i=0; i<v_metadata.size(); i++)
		new_vec.push_back(v_metadata.at(i));


	for(uint i=row; i<_v_meta_data.size(); i++)
		new_vec.push_back(_v_meta_data.at(i));

	_v_meta_data.clear();
	_v_meta_data = new_vec;

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


	if(_playlist_mode.rep1){
		if(_cur_play_idx < 0) return;
		emit selected_file_changed(_cur_play_idx);
		emit selected_file_changed_md(_v_meta_data[_cur_play_idx]);

	}

	else if(_playlist_mode.shuffle){

		int track_num = rand() % _v_meta_data.size();
		_cur_play_idx = track_num;
		emit selected_file_changed(track_num);
		emit selected_file_changed_md(_v_meta_data[track_num]);
	}


	else if(_playlist_mode.repAll){

		if(_cur_play_idx >= (int) _v_meta_data.size() -1){

			emit selected_file_changed(0);
			emit selected_file_changed_md(_v_meta_data[0]);
			_cur_play_idx = 0;
		}

		else {
			_cur_play_idx++;
			emit selected_file_changed(_cur_play_idx);
			emit selected_file_changed_md(_v_meta_data[_cur_play_idx]);
		}
	}


	else {

		if(_cur_play_idx >= (int) _v_meta_data.size() -1){
			emit no_track_to_play();
		}

		else {
			_cur_play_idx++;
			emit selected_file_changed(_cur_play_idx);
			emit selected_file_changed_md(_v_meta_data[_cur_play_idx]);
		}
	}

	emit search_similar_artists(_v_meta_data[_cur_play_idx].artist);
}



// GUI -->
void Playlist::change_track(int new_row){
	if( (uint) new_row >= _v_meta_data.size())return;


	_cur_play_idx = new_row;
	emit selected_file_changed_md(_v_meta_data.at(new_row));
	emit search_similar_artists(_v_meta_data[_cur_play_idx].artist);
}


// GUI -->
void Playlist::clear_playlist(){

	_v_meta_data.clear();
	_cur_play_idx = -1;
	emit playlist_created(_v_meta_data, _cur_play_idx);

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

		qDebug() << lines << " lines were written";
		fclose(file);
	}

	else{
		qDebug() << " File could not be openend";
	}
}


// GUI -->
void Playlist::playlist_mode_changed(const Playlist_Mode& playlist_mode){

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


void Playlist::similar_artists_available(const int& artist_id){

	srand ( time(NULL) );
	vector<MetaData> vec_tracks;
	CDatabaseConnector::getInstance()->getAllTracksByArtist(artist_id, vec_tracks);

	int rnd_track = (rand() % vec_tracks.size());
	MetaData md = vec_tracks.at(rnd_track);

	_v_meta_data.push_back(md);

	emit playlist_created(_v_meta_data, _cur_play_idx);

}
