/* TagEdit.cpp */

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



#include "TagEdit/TagEdit.h"
#include "HelperStructs/Tagging/id3.h"

TagEdit::TagEdit(QObject *parent, bool is_extern) :
	SayonaraClass()
{
	_db = CDatabaseConnector::getInstance();
	_is_extern = is_extern;
}

void TagEdit::update_track(int idx, const MetaData& md){

	_changed_md[idx] = !( md.is_equal_deep(_v_md_orig[idx]) );
	_v_md[idx] = md;
}

void TagEdit::undo(int idx){
	_v_md[idx] = _v_md_orig[idx];
}


void TagEdit::undo_all(){
	_v_md = _v_md_orig;
}

MetaData TagEdit::get_metadata(int idx){
	return _v_md[idx];
}

MetaDataList TagEdit::get_all_metadata(){
	return _v_md;
}

int TagEdit::get_n_tracks(){
	return _v_md.size();
}

void TagEdit::set_metadata(const MetaDataList& v_md){
	_v_md = v_md;
	_v_md_orig = v_md;

	_changed_md.clear();
	for(int i=0; i<_v_md.size(); i++){
		_changed_md << false;
	}

	emit sig_metadata_received(_v_md);
}

void TagEdit::check_for_new_artists_and_albums(QStringList& new_artists, QStringList& new_albums){

	QStringList artists;
	QStringList albums;

	// first gather all artists and albums
	foreach(MetaData md, _v_md){
		if(!artists.contains(md.artist)){
			artists << md.artist;
		}

		if(!albums.contains(md.album)){
			albums << md.album;
		}
	}

	foreach(QString album_name, albums){
		int id = _db->getAlbumID(album_name);
		qDebug() << "Album: " << album_name << ": " << id;
		if(id < 0) new_albums << album_name;
	}

	foreach(QString artist_name, artists){
		int id = _db->getArtistID(artist_name);
		if(id < 0) new_artists << artist_name;
	}
}


void TagEdit::insert_new_artists(const QStringList& artists){

	foreach(QString a, artists){
		_db->insertArtistIntoDatabase(a);
	}
}

void TagEdit::insert_new_albums(const QStringList& albums){

	foreach(QString a, albums){

		_db->insertAlbumIntoDatabase(a);
	}
}

void TagEdit::apply_artists_and_albums_to_md(){


	for(int i=0; i<_v_md.size(); i++){

		if( _changed_md[i] == false ) continue;

		int artist_id, album_id;
		artist_id = _db->getArtistID(_v_md[i].artist);
		album_id = _db->getAlbumID(_v_md[i].album);

		if(_v_md[i].artist_id == artist_id && _v_md[i].album_id == album_id) continue;

		_v_md[i].album_id = album_id;
		_v_md[i].artist_id = artist_id;

	}
}

void TagEdit::write_tracks_to_db(){

	MetaDataList v_md;
	MetaDataList v_md_orig;

	if(!_is_extern){
		QStringList new_artists, new_albums;
		check_for_new_artists_and_albums(new_artists, new_albums);

		insert_new_albums(new_albums);
		insert_new_artists(new_artists);

		apply_artists_and_albums_to_md();
	}

	for(int i=0; i<_v_md.size(); i++){

		emit sig_progress( (i * 100) / _v_md.size());

		if( _changed_md[i] == false ) continue;

		if( !_is_extern ){

			bool success = ID3::setMetaDataOfFile(_v_md[i]);
			qDebug() << "Write track "<< _v_md[i].title
					 << " (" << _v_md[i].album << ") by " << _v_md[i].artist
					 << ": " << success;

			if( success ){

				if( _v_md[i].id >= 0 ){
					success = _db->updateTrack(_v_md[i]);
				}

				if(success){

					v_md.push_back(_v_md[i]);
					v_md_orig.push_back(_v_md_orig[i]);
				}
			}
		}

		else{
			v_md.push_back(_v_md[i]);
			v_md_orig.push_back(_v_md_orig[i]);
		}
	}

	emit sig_progress(-1);
	emit sig_metadata_changed(v_md_orig, v_md);
}

