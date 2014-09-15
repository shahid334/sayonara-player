/* SoundcloudLibrary.cpp */

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



#include "Soundcloud/SoundcloudLibrary.h"
#include "Soundcloud/SoundcloudHelper.h"
#include "HelperStructs/WebAccess.h"


SoundcloudLibrary::SoundcloudLibrary(QWidget* main_window, QObject *parent) :
	QObject(parent)
{

	_main_window = main_window;
	_settings = CSettingsStorage::getInstance();
	_db = CDatabaseConnector::getInstance();

	QList<int> sortings = _settings->getLibSorting();


	_artist_sortorder = (SortOrder) sortings[0];
	_album_sortorder = (SortOrder) sortings[1];
	_track_sortorder = (SortOrder) sortings[2];
}




bool SoundcloudLibrary::dl_all_artist_info(QString name, Artist& artist){
	bool success;
	QString content;

	QString dl = SoundcloudHelper::create_dl_get_artist(name);
	if(dl.size() == 0) return false;

	success = WebAccess::read_http_into_str(dl, &content);
	if(!success || content.size() == 0) return false;

	success = SoundcloudHelper::parse_artist_xml(content, artist);
	return success;
}


bool SoundcloudLibrary::dl_all_playlists_by_artist(qint64 artist_id, QString& content){

	bool success;

	QString dl = SoundcloudHelper::create_dl_get_playlists(artist_id);
	if(dl.size() == 0) return false;

	success = WebAccess::read_http_into_str(dl, &content);
	if(!success || content.size() == 0) {
		qDebug() << "Cannot get any data from " << dl;
		return false;
	}

	return success;
}


bool SoundcloudLibrary::dl_all_tracks_by_artist(qint64 artist_id, QString& content){

	bool success;

	QString dl = SoundcloudHelper::create_dl_get_tracks(artist_id);
	if(dl.size() == 0) return false;

	success = WebAccess::read_http_into_str(dl, &content);
	if(!success || content.size() == 0) {
		qDebug() << "Cannot get any data from " << dl;
		return false;
	}

	return success;
}


void SoundcloudLibrary::loadData(){

	return;

	bool success;
	Artist artist;
	QString content;

	success = dl_all_artist_info("Alloinyx", artist);
	if(!success){
		qDebug() << "Cannot get info from Shoutcast Artist Alloinyx";
		return;
	}

	success = dl_all_playlists_by_artist(artist.id, content);
	if(!success){
		return;
	}

	success = SoundcloudHelper::parse_playlist_xml(content, _vec_md, _vec_artists, _vec_albums);
	if(!success){
		qDebug() << "Could not parse Playlists xml file";
		return;
	}

	qDebug() << "Got " << _vec_md.size() << " tracks "
			 << ", " << _vec_artists.size() << " artists "
			 << ", " << _vec_albums.size() << " albums";

	content.clear();

	MetaDataList v_md;
	success = dl_all_tracks_by_artist(artist.id, content);
	if(!success){
		return;
	}

	success = SoundcloudHelper::parse_tracks_xml(content, v_md);
	qDebug() << "Got " << v_md.size() << " tracks ";

	if( v_md.size() == _vec_md.size() ) return;

	foreach(MetaData md_wo_pl, v_md){

		bool track_found_in_pl = false;
		foreach(MetaData md_in_pl, _vec_md){

			if(md_wo_pl.id == md_in_pl.id){
				track_found_in_pl = true;
				break;
			}
		}

		if(!track_found_in_pl){
			qDebug() << "Add one track " << md_wo_pl.title;
			md_wo_pl.album_id = 0;
			md_wo_pl.album = tr("Misc");
			_vec_md.push_back(md_wo_pl);
		}
	}
}


void SoundcloudLibrary::reloadLibrary(bool clear){

}

void SoundcloudLibrary::clearLibrary(){

}

void SoundcloudLibrary::refresh(bool emit_changed){

}

void SoundcloudLibrary::psl_selected_artists_changed(const QList<int>& lst){

}

void SoundcloudLibrary::psl_selected_albums_changed(const QList<int>& lst){

}

void SoundcloudLibrary::psl_selected_tracks_changed(const QList<int>& lst){

}

void SoundcloudLibrary::psl_prepare_album_for_playlist(int idx){

}

void SoundcloudLibrary::psl_prepare_artist_for_playlist(int idx){

}

void SoundcloudLibrary::psl_prepare_track_for_playlist(int idx){

}

void SoundcloudLibrary::psl_prepare_tracks_for_playlist(QList<int> lst){

}


void SoundcloudLibrary::
psl_sortorder_changed(Sort::SortOrder artist_sort, Sort::SortOrder album_sort, Sort::SortOrder track_sort){

}

void SoundcloudLibrary::psl_delete_tracks(int idx){

}

void SoundcloudLibrary::psl_delete_certain_tracks(const QList<int>&,int lst){

}

void SoundcloudLibrary::psl_play_next_all_tracks(){

}

void SoundcloudLibrary::psl_play_next_tracks(const QList<int>& lst){

}

void SoundcloudLibrary::psl_append_all_tracks(){

}

void SoundcloudLibrary::psl_append_tracks(const QList<int>& lst){

}

void SoundcloudLibrary::psl_track_rating_changed(int idx, int rating){

}

void SoundcloudLibrary::psl_album_rating_changed(int idx, int rating){

}

void SoundcloudLibrary::library_reloading_state_slot(QString str){

}

void SoundcloudLibrary::library_reloading_state_new_block(){

}

void SoundcloudLibrary::reload_thread_finished(){

}
