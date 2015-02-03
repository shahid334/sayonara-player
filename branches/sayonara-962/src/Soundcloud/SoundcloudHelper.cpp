/* SoundcloudHelper.cpp */

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

QString	SoundcloudHelper::create_dl_get_artist(QString name){
	QString ret = "";
	if(name.size() == 0) return ret;

	ret = QString("http://api.soundcloud.com/users?") +
			CLIENT_ID_STR +
			"&q=" +
			name;

	sc_debug << "Get Artist info from " << ret;

	return ret;
}


QString	SoundcloudHelper::create_dl_get_playlists(qint64 artist_id){
	QString ret = "";

	ret = QString("http://api.soundcloud.com/users/") +
			QString::number(artist_id) + "/"
			"playlists?" +
			CLIENT_ID_STR;

	sc_debug << "Get Artist playlists from " << ret;

	return ret;
}


QString	SoundcloudHelper::create_dl_get_tracks(qint64 artist_id){
	QString ret = "";

	ret = QString("http://api.soundcloud.com/users/") +
			QString::number(artist_id) + "/"
			"tracks?" +
			CLIENT_ID_STR;

	sc_debug << "Get Artist playlists from " << ret;

	return ret;
}


int	SoundcloudHelper::Parser::find_value_end(const QString& content, int start_at){

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


int	SoundcloudHelper::Parser::find_block_end(const QString& content, int start_at){

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

int	SoundcloudHelper::Parser::find_array_end(const QString& content, int start_at){
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



JsonItem SoundcloudHelper::Parser::parse(QString key, const QString& content){

	JsonItem ret;
	if(content.startsWith('[')){
		ret = Parser::parse_array(key, content);
	}

	else if(content.startsWith('{')){
		ret = Parser::parse_block(key, content);
	}

	else {
		ret = parse_standard(key, content);
	}

	return ret;
}

JsonItem SoundcloudHelper::Parser::parse_array(QString key, QString content){

	JsonItem ret;
	ret.key = key;
	ret.type = JsonItem::TypeArray;

	remove_first_and_last(content, '[', ']');

	int i=0;
	forever{

		int end = Parser::find_block_end(content);
		if(end == 0) break;

		QString str = content.left(end + 1);
		if(str.size() == 0) break;

		ret.values << Parser::parse_block(QString::number(i), str);

		int letters_left = content.size() - str.size();
		if(letters_left <= 0) break;

		content = content.right(letters_left);
		i++;
	}

	return ret;
}

JsonItem SoundcloudHelper::Parser::parse_block(QString key, QString content){

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
			new_start = Parser::find_array_end(content);
			substr = content.left(new_start);
			ret.values << parse_array(item_key, substr);
		}

		else if(c == '{'){
			new_start = Parser::find_block_end(content);
			substr = content.left(new_start);
			ret.values << parse_block(item_key, substr);
		}

		else{
			new_start = find_value_end(content);
			substr = content.left(new_start);
			ret.values << Parser::parse_standard(item_key, substr);
		}

		content = content.right(content.size() - new_start);
		if(content.size() == 0) break;
	}

	return ret;
}

JsonItem SoundcloudHelper::Parser::parse_standard(QString key, QString content){

	JsonItem ret;
	ret.key = key;

	int end = Parser::find_value_end(content);
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

ArtistList SoundcloudHelper::search_artist(const QString& name){

	QString content;
	ArtistList artists;
	QString url = create_dl_get_artist(name);
	Helper::read_http_into_str(url, &content);

	JsonItem item = Parser::parse("Artists", content);

	for(const JsonItem& artist_item : item.values){
		Artist artist;
		Parser::extract_artist(artist_item, artist);
		artists << artist;
	}

	sc_debug << "Found " << artists.size() << " artists";
	return artists;
}


bool SoundcloudHelper::get_all_playlists(qint32 artist_id, MetaDataList& v_md, AlbumList& albums){

	QString content;
	QString url = create_dl_get_playlists(artist_id);
	Helper::read_http_into_str(url, &content);

	JsonItem item = Parser::parse("Playlists", content);

	// iterate over playlists
	for(const JsonItem& album_item : item.values){

		Album album;
		MetaDataList v_md_tmp;

		bool success = Parser::extract_playlist(album_item, album, v_md_tmp);

		if(!success){
			continue;
		}

		albums << album;
		album.print();

		for(const MetaData& md : v_md_tmp){
			v_md.push_back(md);
			md.print();
		}
	}

	sc_debug << "Found " << v_md.size() << " tracks in " << albums.size() << " playlists";

	return true;
}


bool SoundcloudHelper::Parser::extract_track(const JsonItem& item, MetaData& md){

	if(item.type != JsonItem::TypeBlock){
		return false;
	}

	for(const JsonItem& track_info : item.values){

		if(track_info.key == "duration"){
			md.length_ms = (quint32) track_info.pure_value.toLong();
		}

		/*else if(track_info.key == "id"){
			md.id = track_info.pure_value.toInt();
		}*/

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

bool SoundcloudHelper::Parser::extract_artist(const JsonItem& item, Artist& artist){

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

bool SoundcloudHelper::Parser::extract_playlist(const JsonItem& item, Album& album, MetaDataList& v_md){

	if(item.type != JsonItem::TypeBlock){
		return false;
	}

	Artist artist;

	// iterate over elements in album
	for(const JsonItem& album_info : item.values){

		/*if(album_info.key == "id"){
			album.id = album_info.pure_value.toInt();
		}
		else*/ if(album_info.key == "title"){
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
				bool success = Parser::extract_track(track, md);

				if(success){
					v_md << md;
				}
			}
		}

		else if(album_info.key == "user"){
			Parser::extract_artist(album_info, artist);
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

