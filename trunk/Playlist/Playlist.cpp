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

#include <QFile>
#include <QObject>

#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>

#include <iostream>

using namespace std;

Playlist::Playlist(QObject * parent) : QObject (parent){

}

Playlist::~Playlist() {


	// TODO Auto-generated destructor stub
}



void Playlist::createPlaylist(QStringList& pathlist){

	qDebug() << "Create playlist called";

    if(!_playlist_mode.append){
		_v_meta_data.clear();
	}
    uint files2fill = pathlist.size();
	for(uint i=0; i<files2fill; i++){

		MetaData md = ID3::getMetaDataOfFile(pathlist[i]);
		_v_meta_data.push_back(md);

		double percent = i * 1.0 / files2fill;
		emit mp3s_loaded_signal((int) (percent * 100));
	}

	emit mp3s_loaded_signal(100);
	emit playlist_created(_v_meta_data);
}


void Playlist::createPlaylist(vector<MetaData>& v_meta_data){
	//_pathlist = pathlist;
	_v_meta_data.clear();

	_v_meta_data = v_meta_data;
	emit playlist_created(_v_meta_data);


}






void Playlist::forward(){

	if(_playlist_mode.shuffle){

		int track_num = rand() % _v_meta_data.size();
		_cur_play_idx = track_num;
		emit selected_file_changed(track_num);
		emit selected_file_changed_md(_v_meta_data[track_num]);

	}

	else if(this->_cur_play_idx < _v_meta_data.size() - 1 && _cur_play_idx >= 0){
		_cur_play_idx++;
		emit selected_file_changed(_cur_play_idx);
		emit selected_file_changed_md(_v_meta_data[_cur_play_idx]);

	}
}


void Playlist::backward(){


	if(this->_cur_play_idx > 0){
		_cur_play_idx--;
		emit selected_file_changed(_cur_play_idx);
		emit selected_file_changed_md(_v_meta_data[_cur_play_idx]);

	}

}



// --> GUI
void Playlist::next_track(){
	if(_cur_play_idx < 0) return;

	if(_playlist_mode.rep1){
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

		if(_cur_play_idx >= _v_meta_data.size() -1){

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

		if(_cur_play_idx >= _v_meta_data.size() -1){
			emit no_track_to_play();
		}

		else {
			_cur_play_idx++;
			emit selected_file_changed(_cur_play_idx);
			emit selected_file_changed_md(_v_meta_data[_cur_play_idx]);
		}
	}
}



// GUI -->
void Playlist::change_track(int new_row){
	if( (uint) new_row >= _v_meta_data.size())return;


	_cur_play_idx = new_row;
	emit selected_file_changed_md(_v_meta_data.at(new_row));
}


// GUI -->
void Playlist::clear_playlist(){

	//_pathlist.clear();
	_v_meta_data.clear();
}

// GUI -->
void Playlist::save_playlist(const QString& filename){


	FILE* file = fopen(filename.toStdString().c_str(), "w");

	if(file){
		qint64 lines = 0;
		for(int i=0; i<_v_meta_data.size(); i++){
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




