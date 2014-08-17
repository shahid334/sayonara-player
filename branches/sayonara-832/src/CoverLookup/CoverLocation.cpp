
#include "CoverLookup/CoverLocation.h"
#include "HelperStructs/Helper.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QDir>


CoverLocation::CoverLocation(){
}

CoverLocation CoverLocation::getInvalidLocation(){

	CoverLocation cl;
	cl.cover_path = Helper::getIconPath() + "logo.png";
	cl.google_url = "";
	return cl;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QString& artist_name){

	CoverLocation ret;
	QString cover_token = Helper::calc_cover_token(artist_name, album_name);
	QString cover_path =  QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + cover_token + ".jpg";

	if(!QFile::exists(QDir::homePath() + QDir::separator() +".Sayonara" + QDir::separator() + "covers")){
		QDir().mkdir(QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers");
	}

	ret.cover_path = cover_path;
	ret.google_url = Helper::calc_google_album_adress(artist_name, album_name);

	return ret;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QStringList& artists){

	QString major_artist = Helper::get_major_artist(artists);
	return get_cover_location(album_name, major_artist);
}

CoverLocation CoverLocation::get_cover_location(int album_id){

	if(album_id < 0) return CoverLocation::getInvalidLocation();

	Album album;
	bool success = CDatabaseConnector::getInstance()->getAlbumByID(album_id, album);

	if(!success) {
		return getInvalidLocation();
	}

	return get_cover_location(album);
}


CoverLocation CoverLocation::get_cover_location(const Album& album){
	int n_artists;

	n_artists = album.artists.size();
	if( n_artists > 1 ){
		return CoverLocation::get_cover_location(album.name, album.artists);
	}

	else if( n_artists == 1 ){
		return CoverLocation::get_cover_location(album.name, album.artists[0]);
	}

	else {
		return CoverLocation::get_cover_location(album.name, "");
	}
}

CoverLocation CoverLocation::get_cover_location(const Artist& artist){
	return CoverLocation::get_cover_location(artist.name);
}


CoverLocation CoverLocation::get_cover_location(const QString& artist){
	CoverLocation ret;

	QString token = QString("artist_") + Helper::calc_cover_token(artist, "");
	QString target_file = QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + token + ".jpg";

	if(!QFile::exists(QDir::homePath() + QDir::separator() +".Sayonara" + QDir::separator() + "covers")){
		QDir().mkdir(QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers");
	}

	ret.cover_path = target_file;
	ret.google_url = Helper::calc_google_artist_adress(artist);

	return ret;
}


CoverLocation CoverLocation::get_cover_location(const MetaData& md){

	return get_cover_location(md.album, md.artist);
}

