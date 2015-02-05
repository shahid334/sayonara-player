/* SoundcloudParser.cpp */

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

#define sc_debug qDebug() << "Soundcloud: "

#include "Soundcloud/SoundcloudHelper.h"
#include "HelperStructs/MetaData.h"
#include <QtXml>

#define CLIENT_ID QString("ab7d663fc58d0257c64443358292f6ce")
#define CLIENT_ID_STR QString("client_id=") + CLIENT_ID



void remove_first_and_last(QString& str, QChar first, QChar last){
	if(str.startsWith(first)){
		str.remove(0, 1);
	}

	if(str.endsWith(last)){
		str.remove(str.size() - 1, 1);
	}
}



SoundcloudParser::SoundcloudParser(){

}


SoundcloudParser::~SoundcloudParser(){

}


ArtistList SoundcloudParser::search_artist(const QString& name){

	ArtistList artists;

	if(_artist_name_mapping.contains(name)){

		int idx = _artist_idx_mapping[ _artist_name_mapping[name] ];

		artists << _artist_cache[idx];

		return artists;
	}

	QString content;
	QString url = create_dl_get_artist(name);
	Helper::read_http_into_str(url, &content);

	JsonItem item = parse("Artists", content);

	for(const JsonItem& artist_item : item.values){
		Artist artist;
		extract_artist(artist_item, artist);

		if(_artist_available.contains(artist.id)){
			continue;
		}

		int idx = _artist_cache.size();
		_artist_cache << artist;
		_artist_available.insert(artist.id, true);
		_artist_idx_mapping.insert(artist.id, idx);
		_artist_name_mapping.insert(artist.name, idx);

		create_artist_cache(artist.id);

		artists << artist;
	}


	sc_debug << "Found " << artists.size() << " artists";
	sc_debug << "Found " << _album_cache.size() << " albums";
	sc_debug << "Found " << _track_cache.size() << " tracks";

	return artists;
}

bool SoundcloudParser::create_artist_cache(const QString &name){

	ArtistID id;
	if(!_artist_name_mapping.contains(name)){
		ArtistList artists = search_artist(name);
		if(artists.size() == 0){
			return false;
		}
	}

	id = _artist_name_mapping[name];

	return create_artist_cache(id);
}


bool SoundcloudParser::create_artist_cache(ArtistID artist_id){

	QString content;
	QString url = create_dl_get_playlists(artist_id);
	Helper::read_http_into_str(url, &content);

	JsonItem item = parse("Playlists", content);

	// iterate over playlists
	QList<AlbumCacheIdx> artist_album_idxs = _artist_album_mapping[artist_id];
	QList<TrackCacheIdx> artist_track_idxs = _artist_track_mapping[artist_id];


	for(const JsonItem& album_item : item.values){

		Album album;
		MetaDataList v_md_tmp;


		bool success = extract_playlist(album_item, album, v_md_tmp);

		if(!success){
			continue;
		}

		if(!_album_available.contains(album.id)){

			int idx = _album_cache.size();
			_album_cache << album;
			_album_available.insert(album.id, true);
			_album_idx_mapping.insert(album.id, idx);

			artist_album_idxs << idx;
		}

		else{
			qDebug() << "Check album " << album.name << "... Old";
		}


		QList<TrackCacheIdx> album_track_idxs = _album_track_mapping[album.id];

		for(const MetaData& md : v_md_tmp){

			qDebug() << "Found track " << md.title;
			if(_track_available.contains(md.id)){
				qDebug() << "Found track " << md.title << " ... old";
				continue;
			}

			qDebug() << "Found track " << md.title << " ... new";

			int idx = _track_cache.size();
			_track_cache << md;
			_track_available.insert(md.id, true);
			_track_idx_mapping.insert(md.id, idx);

			artist_track_idxs << idx;
			album_track_idxs << idx;
		}

		_album_track_mapping[album.id] = album_track_idxs;
	}

	_artist_album_mapping[artist_id] = artist_album_idxs;
	_artist_track_mapping[artist_id] = artist_track_idxs;

	return true;
}

ArtistList SoundcloudParser::get_all_artists(){
	return _artist_cache;
}

MetaDataList SoundcloudParser::get_all_tracks(){
	return _track_cache;
}

AlbumList SoundcloudParser::get_all_albums(){

	return _album_cache;
}

MetaDataList SoundcloudParser::get_all_tracks_by_artist(const QString& name){

	if(!_artist_name_mapping.contains(name)){
		create_artist_cache(name);
		if(!_artist_name_mapping.contains(name)){
			MetaDataList v_md;
			return v_md;
		}
	}

	int id = _artist_name_mapping[name];
	return get_all_tracks_by_artist(id);

}

AlbumList SoundcloudParser::get_all_albums_by_artist(const QString& name){

	AlbumList albums;
	if(!_artist_name_mapping.contains(name)){
		create_artist_cache(name);
		if(!_artist_name_mapping.contains(name)){
			return albums;
		}
	}

	int id = _artist_name_mapping[name];
	return get_all_albums_by_artist(id);

}

MetaDataList SoundcloudParser::get_all_tracks_by_artist(ArtistID id){

	MetaDataList v_md;
	if(!_artist_track_mapping.contains(id)){
		create_artist_cache(id);
		if(!_artist_track_mapping.contains(id)){
			return v_md;
		}
	}

	QList<int> idxs = _artist_track_mapping[id];

	for(int idx : idxs){
		v_md << _track_cache[idx];
	}

	return v_md;
}

AlbumList SoundcloudParser::get_all_albums_by_artist(ArtistID id){
	AlbumList albums;
	if(!_artist_album_mapping.contains(id)){
		create_artist_cache(id);
		if(!_artist_album_mapping.contains(id)){
			return albums;
		}
	}
	QList<int> idxs = _artist_album_mapping[id];

	for(int idx : idxs){
		albums << _album_cache[idx];
	}

	return albums;
}

MetaDataList SoundcloudParser::get_all_tracks_by_album(AlbumID album_id){

	MetaDataList v_md;

	if(!_album_track_mapping.contains(album_id)){
		return v_md;
	}

	QList<int> idxs = _album_track_mapping[album_id];

	for(int idx : idxs){
		v_md << _track_cache[idx];
	}

	return v_md;
}





/*** PRIVATE ***/

QString	SoundcloudParser::create_dl_get_artist(QString name){
	QString ret = "";
	if(name.size() == 0) return ret;

	ret = QString("http://api.soundcloud.com/users?") +
			CLIENT_ID_STR +
			"&q=" +
			name;

	sc_debug << "Get Artist info from " << ret;

	return ret;
}


QString	SoundcloudParser::create_dl_get_playlists(qint64 artist_id){
	QString ret = "";

	ret = QString("http://api.soundcloud.com/users/") +
			QString::number(artist_id) + "/"
			"playlists?" +
			CLIENT_ID_STR;

	sc_debug << "Get Artist playlists from " << ret;

	return ret;
}


QString	SoundcloudParser::create_dl_get_tracks(qint64 artist_id){
	QString ret = "";

	ret = QString("http://api.soundcloud.com/users/") +
			QString::number(artist_id) + "/"
			"tracks?" +
			CLIENT_ID_STR;

	sc_debug << "Get Artist playlists from " << ret;

	return ret;
}


int	SoundcloudParser::find_value_end(const QString& content, int start_at){

	int quote_counter = 0;

	for(int i=start_at; i<content.size(); i++){
		QChar c = content[i];
		if(c == '\"'){
			quote_counter ++;
		}

		if(quote_counter % 2 == 0 && c == ','){
			return i;
		}

		if(c == '}'){
			return i;
		}
	}

	return content.size() - 1;
}


int	SoundcloudParser::find_block_end(const QString& content, int start_at){

	int quote_counter = 0;

	for(int i=start_at; i<content.size(); i++){
		QChar c = content[i];
		if(c == '{'){
			quote_counter ++;
		}

		else if(c == '}'){
			quote_counter --;
		}

		if(quote_counter == 0){
			if(c == ',' || c == ']'){
				return i;
			}
		}
	}

	return content.size() - 1;
}

int	SoundcloudParser::find_array_end(const QString& content, int start_at){
	int quote_counter = 0;

	for(int i=start_at; i<content.size(); i++){
		QChar c = content[i];
		if(c == '['){
			quote_counter ++;
		}

		else if(c == ']'){
			quote_counter --;
		}

		if(quote_counter == 0 && c == ','){
			return i;
		}
	}

	return content.size() - 1;
}



JsonItem SoundcloudParser::parse(QString key, const QString& content){

	JsonItem ret;
	if(content.startsWith('[')){
		ret = parse_array(key, content);
	}

	else if(content.startsWith('{')){
		ret = parse_block(key, content);
	}

	else {
		ret = parse_standard(key, content);
	}

	return ret;
}

JsonItem SoundcloudParser::parse_array(QString key, QString content){

	JsonItem ret;
	ret.key = key;
	ret.type = JsonItem::TypeArray;

	remove_first_and_last(content, '[', ']');

	int i=0;
	forever{

		int end = find_block_end(content);
		if(end == 0) break;

		QString str = content.left(end + 1);
		if(str.size() == 0) break;

		ret.values << parse_block(QString::number(i), str);

		int letters_left = content.size() - str.size();
		if(letters_left <= 0) break;

		content = content.right(letters_left);
		i++;
	}

	return ret;
}

JsonItem SoundcloudParser::parse_block(QString key, QString content){

	JsonItem ret;
	ret.key = key;
	ret.type = JsonItem::TypeBlock;

	remove_first_and_last(content, '{', '}');

	forever{

		// find key

		QString item_key;

		int i;
		int quote_counter = 0;
		for(i=0; i<content.size(); i++){

			QChar c = content[i];

			if(c == '\"'){
				quote_counter ++;
			}

			else if(quote_counter == 1){
				item_key.push_back(c);
			}

			if(quote_counter == 2){
				if(c == ':'){
					i++;
					break;
				}
			}
		}

		if(content.size() == 0){
			break;
		}

		if(quote_counter == 0){
			break;
		}

		if(i >= content.size() - 2){
			break;
		}

		QChar c = content[i];
		QString substr;
		content = content.right(content.size() - i);

		int new_start;

		if(c == '['){
			new_start = find_array_end(content);
			substr = content.left(new_start);
			ret.values << parse_array(item_key, substr);
		}

		else if(c == '{'){
			new_start = find_block_end(content);
			substr = content.left(new_start);
			ret.values << parse_block(item_key, substr);
		}

		else{
			new_start = find_value_end(content);
			substr = content.left(new_start);
			ret.values << parse_standard(item_key, substr);
		}

		content = content.right(content.size() - new_start);
		if(content.size() == 0) break;
	}

	return ret;
}

JsonItem SoundcloudParser::parse_standard(QString key, QString content){

	JsonItem ret;
	ret.key = key;

	int end = find_value_end(content);
	content = content.left(end + 1);

	if(content.startsWith('\"')){
		remove_first_and_last(content, '\"', '\"');
		ret.type = JsonItem::TypeString;
	}

	else{
		ret.type = JsonItem::TypeNumber;
	}

	ret.pure_value = content;

	return ret;
}

bool SoundcloudParser::extract_track(const JsonItem& item, MetaData& md){

	if(item.type != JsonItem::TypeBlock){
		return false;
	}

	for(const JsonItem& track_info : item.values){

		if(track_info.key == "duration"){
			md.length_ms = (quint32) track_info.pure_value.toLong();
		}

		else if(track_info.key == "id"){
			md.id = track_info.pure_value.toInt();
		}

		else if(track_info.key == "user_id"){
			md.artist_id = track_info.pure_value.toInt();
		}

		else if(track_info.key == "title"){
			md.title = track_info.pure_value;
		}

		else if(track_info.key == "release_year"){
			md.year = track_info.pure_value.toInt();
		}

		else if(track_info.key == "stream_url"){
			md.set_filepath(track_info.pure_value + "?" + CLIENT_ID_STR);
		}

		else if(track_info.key == "username"){
			md.artist = track_info.pure_value;
		}

		else if(track_info.key == "genre"){
			md.genres << track_info.pure_value;
		}

		else if(track_info.key == "artwork_url"){
			md.cover_download_url = track_info.pure_value;
		}
	}

	return true;
}

bool SoundcloudParser::extract_artist(const JsonItem& item, Artist& artist){

	if(item.type != JsonItem::TypeBlock){
		return false;
	}

	for(const JsonItem& artist_info : item.values){
		if(artist_info.key == "id"){
			artist.id = artist_info.pure_value.toInt();
		}

		else if(artist_info.key == "username"){
			artist.name = artist_info.pure_value;
		}

		else if(artist_info.key == "avatar_url"){
			artist.cover_download_url = artist_info.pure_value;
		}
	}

	return true;
}

bool SoundcloudParser::extract_playlist(const JsonItem& item, Album& album, MetaDataList& v_md){

	if(item.type != JsonItem::TypeBlock){
		return false;
	}

	Artist artist;

	// iterate over elements in album
	for(const JsonItem& album_info : item.values){

		if(album_info.key == "uri"){


			QString uri = album_info.pure_value;

			int idx = uri.lastIndexOf("/") + 1;

			if(idx == 0) continue;

			album.id = uri.right(uri.size() - idx).trimmed().toInt();

		}
		else if(album_info.key == "title"){
			album.name = album_info.pure_value;
		}

		else if(album_info.key == "track_count"){
			album.num_songs = album_info.pure_value.toInt();
		}

		else if(album_info.key == "duration"){
			album.length_sec = (quint32) (album_info.pure_value.toLong() / 1000);
		}

		else if(album_info.key == "artwork_url"){
			album.cover_download_url = album_info.pure_value;
		}

		else if(album_info.key == "tracks"){
			int i=1;
			for(const JsonItem& track : album_info.values){

				MetaData md;
				md.track_num = i++;
				bool success = extract_track(track, md);

				if(success){
					v_md << md;
				}
			}
		}

		else if(album_info.key == "user"){
			extract_artist(album_info, artist);
		}
	}

	for(MetaData& md : v_md){

		md.album_id = album.id;
		md.album = album.name;
		md.artist = artist.name;
		md.artist_id = artist.id;

		if(!album.cover_download_url.isEmpty()){
			md.cover_download_url = album.cover_download_url;
		}
	}

	album.artists << artist.name;

	return true;
}

