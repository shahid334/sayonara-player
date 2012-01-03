/*
 * Playlists.cpp
 *
 *  Created on: Jan 2, 2012
 *      Author: luke
 */


#include "playlists/Playlists.h"
#include "HelperStructs/MetaData.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QMap>
#include <QDebug>

#include <vector>
using namespace std;

Playlists::Playlists() {

}

Playlists::~Playlists() {
	// TODO Auto-generated destructor stub
}

void Playlists::load_all_playlists(){

	_mapping.clear();
	bool success = CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);

	if(success){
		emit sig_all_playlists_loaded(_mapping);
	}

	else{
		qDebug() << "playlists could not be fetched";
	}
}

void Playlists::load_single_playlist(int id){
	CustomPlaylist pl;
	QString playlist_name = CDatabaseConnector::getInstance()->getPlaylistNameById(id);
	bool success = CDatabaseConnector::getInstance()->getPlaylistById(id, pl);

	if(success){
		qDebug() << "Playlist " << playlist_name << " loaded";
		emit sig_single_playlist_loaded(pl);
	}

	else{
		qDebug() << "Could not load playlist " << playlist_name;
	}
}



void Playlists::save_playlist_as_custom(int id, vector<MetaData>& vec_md){
	bool success = CDatabaseConnector::getInstance()->storePlaylist(vec_md, id);
		if(success){
			qDebug() << "Saved playlist as " << id;
			_mapping.clear();
			CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);
			emit sig_all_playlists_loaded(_mapping);

		}

		else{
			qDebug() << "Could not save playlist " << id;
		}
}

void Playlists::save_playlist_as_custom(QString name, vector<MetaData>& vec_md){
	bool success = CDatabaseConnector::getInstance()->storePlaylist(vec_md, name);
	if(success){
		qDebug() << "Saved playlist as " << name;
		_mapping.clear();
		CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);
		emit sig_all_playlists_loaded(_mapping);

	}

	else{
		qDebug() << "Could not save playlist " << name;
	}
}

void Playlists::delete_playlist(int id){

	QString playlist_name = CDatabaseConnector::getInstance()->getPlaylistNameById(id);
	bool success = CDatabaseConnector::getInstance()->deletePlaylist(id);

	if(success){
		qDebug() << "playlist " << playlist_name << " deleted";
	}

	else {
		qDebug() << "playlist " << playlist_name << " could not be deleted";
	}

	_mapping.clear();
	CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);
	emit sig_all_playlists_loaded(_mapping);
}

void Playlists::ui_loaded(){
	bool success = CDatabaseConnector::getInstance()->getAllPlaylists(_mapping);
	if(success){
		sig_all_playlists_loaded(_mapping);
	}
}
