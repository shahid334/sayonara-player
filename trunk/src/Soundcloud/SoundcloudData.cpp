#include "Soundcloud/SoundcloudData.h"

SoundcloudData::SoundcloudData(QObject *parent) :
	QObject(parent)
{
	_sc_parser = new SoundcloudParser();
}

SoundcloudData::~SoundcloudData()
{
	delete _sc_parser;
}


bool artist_less_than_name(const Artist& artist1, const Artist& artist2){
	return (artist1.name < artist2.name);
}

bool artist_less_than_trackcount(const Artist& artist1, const Artist& artist2){
	return (artist1.name < artist2.name);
}


bool artist_greater_than_name(const Artist& artist1, const Artist& artist2){
	return (artist1.name > artist2.name);
}

bool artist_greater_than_trackcount(const Artist& artist1, const Artist& artist2){
	return (artist1.name > artist2.name);
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



bool track_less_than_tracknum(const MetaData& md1, const MetaData& md2){
	return (md1.track_num < md2.track_num);
}

bool track_less_than_title(const MetaData& md1, const MetaData& md2){
	return (md1.title < md2.title);
}

bool track_less_than_album(const MetaData& md1, const MetaData& md2){
	if(md1.album == md2.album){
		if(md1.discnumber == md2.discnumber){
			return md1.track_num < md2.track_num;
		}

		else{
			return md1.discnumber < md2.discnumber;
		}
	}

	else{
		return md1.album < md2.album;
	}
}

bool track_less_than_artist(const MetaData& md1, const MetaData& md2){
	if(md1.artist == md2.artist){
		if(md1.album == md2.album){
			if(md1.discnumber == md2.discnumber){
				return md1.track_num < md2.track_num;
			}

			else{
				return md1.discnumber < md2.discnumber;
			}
		}

		else{
			return md1.album < md2.album;
		}
	}

	else{
		return (md1.artist < md2.artist);
	}
}

bool track_less_than_year(const MetaData& md1, const MetaData& md2){
	return (md1.year < md2.year);
}

bool track_less_than_length(const MetaData& md1, const MetaData& md2){
	return (md1.length_ms < md2.length_ms);
}

bool track_less_than_bitrate(const MetaData& md1, const MetaData& md2){
	return (md1.bitrate < md2.bitrate);
}

bool track_less_than_rating(const MetaData& md1, const MetaData& md2){
	return (md1.rating < md2.rating);
}

bool track_less_than_size(const MetaData& md1, const MetaData& md2){
	return (md1.filesize < md2.filesize);
}




bool track_greater_than_tracknum(const MetaData& md1, const MetaData& md2){
	return (md1.track_num > md2.track_num);
}

bool track_greater_than_title(const MetaData& md1, const MetaData& md2){
	return (md1.title > md2.title);
}

bool track_greater_than_album(const MetaData& md1, const MetaData& md2){
	if(md1.album == md2.album){
		if(md1.discnumber == md2.discnumber){
			return md1.track_num < md2.track_num;
		}

		else{
			return md1.discnumber < md2.discnumber;
		}
	}

	else{
		return md1.album > md2.album;
	}
}


bool track_greater_than_artist(const MetaData& md1, const MetaData& md2){
	if(md1.artist == md2.artist){
		if(md1.album_id == md2.album_id){
			if(md1.discnumber == md2.discnumber){
				return md1.track_num < md2.track_num;
			}

			else{
				return md1.discnumber < md2.discnumber;
			}
		}

		else{
			return md1.album < md2.album;
		}
	}

	else{
		return (md1.artist > md2.artist);
	}
}

bool track_greater_than_year(const MetaData& md1, const MetaData& md2){
	return (md1.year > md2.year);
}

bool track_greater_than_length(const MetaData& md1, const MetaData& md2){
	return (md1.length_ms > md2.length_ms);
}

bool track_greater_than_bitrate(const MetaData& md1, const MetaData& md2){
	return (md1.bitrate > md2.bitrate);
}

bool track_greater_than_rating(const MetaData& md1, const MetaData& md2){
	return (md1.rating > md2.rating);
}

bool track_greater_than_size(const MetaData& md1, const MetaData& md2){
	return (md1.filesize > md2.filesize);
}

void SoundcloudData::sort_tracks(MetaDataList& v_md, LibSortOrder so){

	switch(so.so_tracks){
		case Sort::TrackNumAsc:
			qSort(v_md.begin(), v_md.end(), track_less_than_tracknum);
			break;
		case Sort::TrackNumDesc:
			qSort(v_md.begin(), v_md.end(), track_greater_than_tracknum);
			break;
		case Sort::TrackTitleAsc:
			qSort(v_md.begin(), v_md.end(), track_less_than_title);
			break;
		case Sort::TrackTitleDesc:
			qSort(v_md.begin(), v_md.end(), track_greater_than_title);
			break;
		case Sort::TrackAlbumAsc:
			qSort(v_md.begin(), v_md.end(), track_less_than_album);
			break;
		case Sort::TrackAlbumDesc:
			qSort(v_md.begin(), v_md.end(), track_greater_than_album);
			break;
		case Sort::TrackArtistAsc:
			qSort(v_md.begin(), v_md.end(), track_less_than_artist);
			break;
		case Sort::TrackArtistDesc:
			qSort(v_md.begin(), v_md.end(), track_greater_than_artist);
			break;
		case Sort::TrackYearAsc:
			qSort(v_md.begin(), v_md.end(), track_less_than_year);
			break;
		case Sort::TrackYearDesc:
			qSort(v_md.begin(), v_md.end(), track_greater_than_year);
			break;
		case Sort::TrackLenghtAsc:
			qSort(v_md.begin(), v_md.end(), track_less_than_length);
			break;
		case Sort::TrackLengthDesc:
			qSort(v_md.begin(), v_md.end(), track_greater_than_length);
			break;
		case Sort::TrackBitrateAsc:
			qSort(v_md.begin(), v_md.end(), track_less_than_bitrate);
			break;
		case Sort::TrackBitrateDesc:
			qSort(v_md.begin(), v_md.end(), track_greater_than_bitrate);
			break;
		case Sort::TrackRatingAsc:
			qSort(v_md.begin(), v_md.end(), track_less_than_rating);
		case Sort::TrackRatingDesc:
			qSort(v_md.begin(), v_md.end(), track_greater_than_rating);
		case Sort::TrackSizeAsc:
			qSort(v_md.begin(), v_md.end(), track_less_than_size);
		case Sort::TrackSizeDesc:
			qSort(v_md.begin(), v_md.end(), track_greater_than_size);
		default:
			qSort(v_md.begin(), v_md.end(), track_less_than_tracknum);
	}
}

void SoundcloudData::sort_albums(AlbumList& albums, LibSortOrder so) {
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

void SoundcloudData::sort_artists(ArtistList& artists, LibSortOrder so){
	switch(so.so_artists){
		case Sort::ArtistNameAsc:
			qSort(artists.begin(), artists.end(), artist_less_than_name);
			break;
		case Sort::ArtistNameDesc:
			qSort(artists.begin(), artists.end(), artist_greater_than_name);
			break;

		default:
			break;
	}
}



bool SoundcloudData::load(){

	_sc_parser->search_artist("Alloinyx");
	return true;

}


void SoundcloudData::get_all_artists(ArtistList& artists, LibSortOrder so){

	artists = _sc_parser->get_all_artists();
	if(so.so_artists != _sortorder.so_artists) {
		sort_artists(artists, so);
		_sortorder = so;
	}
}


void SoundcloudData::get_all_albums(AlbumList& albums, LibSortOrder so){

	albums = _sc_parser->get_all_albums();

	if(so.so_albums != _sortorder.so_albums){

		sort_albums(albums, so);
		_sortorder = so;
	}
}


void SoundcloudData::get_all_tracks(MetaDataList& v_md, LibSortOrder so){

	v_md = _sc_parser->get_all_tracks();
	qDebug() << "Get all tracks...";

	if(so.so_tracks != _sortorder.so_tracks){
		sort_tracks(v_md, so);
		_sortorder = so;
	}

}

void SoundcloudData::get_all_albums_by_artist(AlbumList &albums, int artist_id, LibSortOrder so){
	albums = _sc_parser->get_all_albums_by_artist(artist_id);
}

void SoundcloudData::get_all_tracks_by_album(MetaDataList &v_md, int album_id, LibSortOrder so){
	v_md = _sc_parser->get_all_tracks_by_album(album_id);
}

void SoundcloudData::get_all_tracks_by_artist(MetaDataList &v_md, int artist_id, LibSortOrder so){
	v_md = _sc_parser->get_all_tracks_by_artist(artist_id);
}
