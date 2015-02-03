#include "Soundcloud/SoundcloudData.h"

SoundcloudData::SoundcloudData(QObject *parent) :
	QObject(parent)
{

}

SoundcloudData::~SoundcloudData(){

}



bool artist_less_than_name(const Artist& artist1, const Artist& artist2){
	return (artist1.name < artist2.name);
}

bool artist_less_than_trackcount(const Artist& artist1, const Artist& artist2){
	return (artist1.name < artist2.name);
}


bool album_less_than_name(const Album& album1, const Album& album2){
	return (album1.name < album2.name);
}

bool album_less_than_length(const Album& album1, const Album& album2){
	return (album1.length_sec < album2.length_sec);
}

bool album_less_than_num_songs(const Album& album1, const Album& album2){
	return (album1.num_songs < album2.num_songs);
}

bool album_less_than_year(const Album& album1, const Album& album2){
	return (album1.year < album2.year);
}



bool album_greater_than_name(const Album& album1, const Album& album2){
	return (album1.name > album2.name);
}

bool album_greater_than_length(const Album& album1, const Album& album2){
	return (album1.length_sec > album2.length_sec);
}

bool album_greater_than_num_songs(const Album& album1, const Album& album2){
	return (album1.num_songs > album2.num_songs);
}

bool album_greater_than_year(const Album& album1, const Album& album2){
	return (album1.year > album2.year);
}


bool SoundcloudData::load(){

	_artist_cache = SoundcloudHelper::search_artist("Alloinyx");
	if(_artist_cache.size() == 0){
		return false;
	}

	return SoundcloudHelper::get_all_playlists(_artist_cache[0].id, _track_cache, _album_cache);

}

void SoundcloudData::get_all_artists(ArtistList& artists, LibSortOrder so){

	artists = _artist_cache;

	if(so.so_artists != _sortorder.so_artists) {
		qSort(artists.begin(), artists.end(), artist_less_than_name);
	}
}

void SoundcloudData::get_all_albums(AlbumList& albums, LibSortOrder so){
	albums = _album_cache;

	if(so.so_albums != _sortorder.so_albums){

		switch(so.so_albums){
			case Sort::AlbumNameAsc:
				qSort(albums.begin(), albums.end(), album_less_than_name);
				break;
			case Sort::AlbumNameDesc:
				qSort(albums.begin(), albums.end(), album_greater_than_name);
				break;
			case Sort::AlbumYearAsc:
				qSort(albums.begin(), albums.end(), album_less_than_year);
				break;
			case Sort::AlbumYearDesc:
				qSort(albums.begin(), albums.end(), album_greater_than_year);
				break;
			case Sort::AlbumTracksAsc:
				qSort(albums.begin(), albums.end(), album_less_than_num_songs);
				break;
			case Sort::AlbumTracksDesc:
				qSort(albums.begin(), albums.end(), album_greater_than_num_songs);
				break;
			case Sort::AlbumDurationAsc:
				qSort(albums.begin(), albums.end(), album_less_than_length);
				break;
			case Sort::AlbumDurationDesc:
				qSort(albums.begin(), albums.end(), album_greater_than_length);
				break;

			default:
				break;
		}
	}
}

void SoundcloudData::get_all_tracks(MetaDataList& v_md, LibSortOrder so){
	v_md = _track_cache;
}
