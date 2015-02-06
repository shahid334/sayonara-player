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


SoundcloudLibrary::SoundcloudLibrary(QObject *parent) :
	AbstractLibrary(parent)
{
	_scd = new SoundcloudData(this);
}

void SoundcloudLibrary::load(){
	_scd->load();
	AbstractLibrary::load();
}


void SoundcloudLibrary::get_all_artists(ArtistList& artists, LibSortOrder so){
	_scd->get_all_artists(artists, so);
}

void SoundcloudLibrary::get_all_artists_by_searchstring(Filter filter, ArtistList& artists, LibSortOrder so){

	ArtistList tmp_artists;
	bool* found_artists;
	_scd->get_all_artists(tmp_artists, so);
	found_artists = new bool[tmp_artists.size()];


	QMap<int, int> artist_map;

	int i=0;
	for(const Artist& a : tmp_artists){
		if(a.name.contains(filter.filtertext)){
			artists << a;
			found_artists[i] = true;
		}

		else{
			found_artists[i] = false;
		}

		artist_map.insert(a.id, i);

		i++;
	}
}

void SoundcloudLibrary::get_all_albums(AlbumList& albums, LibSortOrder so){
	_scd->get_all_albums(albums, so);
}

void SoundcloudLibrary::get_all_albums_by_artist(QList<int> artist_ids, AlbumList& albums, Filter filter, LibSortOrder so){


}

void SoundcloudLibrary::get_all_albums_by_searchstring(Filter filter, AlbumList& albums, LibSortOrder so){


	AlbumList tmp_albums;
	_scd->get_all_albums(tmp_albums, so);

	for(const Album& a : tmp_albums){
		if(a.name.contains(filter.filtertext)){
			albums << a;
		}
	}
}

void SoundcloudLibrary::get_all_tracks(MetaDataList& v_md, LibSortOrder so){
	_scd->get_all_tracks(v_md, so);
}

void SoundcloudLibrary::get_all_tracks_by_artist(QList<int> artist_ids, MetaDataList& v_md, Filter filter, LibSortOrder so){

	for(int artist_id : artist_ids){

		MetaDataList v_md_tmp;
		_scd->get_all_tracks_by_artist(v_md_tmp, artist_id, so);
		for(const MetaData& md : v_md_tmp){
			v_md << md;
		}
	}

	_scd->sort_tracks(v_md, so);

}

void SoundcloudLibrary::get_all_tracks_by_album(QList<int> album_ids, MetaDataList& v_md, Filter filter, LibSortOrder so){

	for(int album_id : album_ids){

		MetaDataList v_md_tmp;
		_scd->get_all_tracks_by_album(v_md_tmp, album_id, so);
		for(const MetaData& md : v_md_tmp){
			v_md << md;
		}
	}

	_scd->sort_tracks(v_md, so);
}

void SoundcloudLibrary::get_all_tracks_by_searchstring(Filter filter, MetaDataList& v_md, LibSortOrder so){
	_scd->get_all_tracks(v_md, so);
}

void SoundcloudLibrary::get_album_by_id(int album_id, Album& album){

	AlbumList albums;
	_scd->get_all_albums(albums, LibSortOrder());

	for(const Album& a : albums){
		if(a.id == album_id){
			album = a;
			return;
		}
	}
}

void SoundcloudLibrary::get_artist_by_id(int artist_id, Artist& artist){

	ArtistList artists;
	_scd->get_all_artists(artists, LibSortOrder());

	for(const Artist& a : artists){
		if(a.id == artist_id){
			artist = a;
			return;
		}
	}
}


void SoundcloudLibrary::update_track(const MetaData& md){

}

void SoundcloudLibrary::update_album(const Album& album){

}

void SoundcloudLibrary::delete_tracks(const MetaDataList& v_md, TrackDeletionMode mode){

}

void SoundcloudLibrary::psl_reload_library(bool b){

}


