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



#include "Soundcloud/SoundcloudHelper.h"
#include "HelperStructs/MetaData.h"
#include <QtXml>

#define CLIENT_ID QString("ab7d663fc58d0257c64443358292f6ce")
#define CLIENT_ID_STR QString("client_id=") + CLIENT_ID

QString	SoundcloudHelper::create_dl_get_artist(QString name){
	QString ret = "";
	if(name.size() == 0) return ret;

	ret = QString("http://api.soundcloud.com/users?") +
			CLIENT_ID_STR +
			"&q=" +
			name;

	qDebug() << "Get Artist info from " << ret;

	return ret;
}


QString	SoundcloudHelper::create_dl_get_playlists(qint64 artist_id){
	QString ret = "";

	ret = QString("http://api.soundcloud.com/users/") +
			QString::number(artist_id) + "/"
			"playlists?" +
			CLIENT_ID_STR;

	qDebug() << "Get Artist playlists from " << ret;

	return ret;
}


QString	SoundcloudHelper::create_dl_get_tracks(qint64 artist_id){
	QString ret = "";

	ret = QString("http://api.soundcloud.com/users/") +
			QString::number(artist_id) + "/"
			"tracks?" +
			CLIENT_ID_STR;

	qDebug() << "Get Artist playlists from " << ret;

	return ret;
}


bool SoundcloudHelper::parse_artist_xml(const QString& content, Artist& artist){

	artist.id = -1;

	QDomDocument doc("Artist");
	doc.setContent(content);

	QDomElement docElement = doc.documentElement();
	QDomNode entry = docElement.firstChildElement("user");

	if(!entry.hasChildNodes()) return false;

	for(int entry_child=0; entry_child<entry.childNodes().size(); entry_child++)
	{
		QDomNode content = entry.childNodes().at(entry_child);
		QString nodename = content.nodeName().toLower();
		QDomElement e = content.toElement();

		if(!nodename.compare("id", Qt::CaseInsensitive)){
			artist.id = e.text().toInt();
		}

		else if(!nodename.compare("username")){
			artist.name = e.text();
		}

		else if(!nodename.compare("track-count")){
			artist.num_songs = e.text().toInt();
		}

		else if(!nodename.compare("playlist-count")){
			artist.num_albums = e.text().toInt();
		}
	}

	return (artist.id > 0);
}

bool SoundcloudHelper::parse_playlist_xml(
		const QString& content,
		MetaDataList& v_md,
		ArtistList& v_artists,
		AlbumList& v_albums){

	v_md.clear();
	v_artists.clear();
	v_albums.clear();

	if(content.size() == 0) return false;

	QDomDocument doc("Playlists");
	doc.setContent(content);

	QDomElement docElement = doc.documentElement();
	QDomNode entry = docElement.firstChildElement("playlist");

	if(!entry.hasChildNodes()) return false;



	do{

		Album album;

		QDomNodeList child_nodes = entry.childNodes();
		int n_child_nodes = child_nodes.size();

		for(int entry_child=0; entry_child<n_child_nodes; entry_child++)
		{
			QDomNode content = child_nodes.at(entry_child);
			QString nodename = content.nodeName().toLower();
			QDomElement e = content.toElement();

			if(!nodename.compare("id", Qt::CaseInsensitive)){

				album.id = e.text().toInt();

				// maybe album was not set yet
				for(int i=0; i<(int) v_md.size(); i++){
					v_md[i].album_id = album.id;
				}
			}

			else if(!nodename.compare("title", Qt::CaseInsensitive)){
				album.name = e.text();

				// maybe album was not set yet
				for(int i=0; i<(int) v_md.size(); i++){
					v_md[i].album_id = album.id;
				}
			}

			else if(!nodename.compare("track-count", Qt::CaseInsensitive)){
				album.num_songs = e.text().toInt();
			}

			else if(!nodename.compare("duration", Qt::CaseInsensitive)){
				album.length_sec = e.text().toInt() / 1000;
			}


			else if(!nodename.compare("tracks", Qt::CaseInsensitive)){

				qDebug() << "Found tracks for playlist " << album.name << ", " << album.id;
				QDomNode first_track = e.firstChildElement("track");

				while(true){

					bool success;
					MetaData md;
					Artist artist;
					artist.id = -1;

					if(first_track.isNull()) break;

					if(!first_track.hasChildNodes()){
						first_track = first_track.nextSibling();
						continue;
					}

					md.album_id = album.id;
					md.album = album.name;

					success = parse_track_dom(first_track, md, artist);

					if(success){
						v_md.push_back(md);

						if(album.artists.size() == 0 && artist.id > 0){
							album.artists << artist.name;

						}

						if(v_artists.size() == 0 && artist.id > 0){
							v_artists.push_back(artist);
						}
					}

					first_track = first_track.nextSibling();
				}
			}
		}

		if(album.id > 0) {
			v_albums.push_back(album);
		}

		entry = entry.nextSibling();

	} while( !entry.isNull() );

	if(v_artists.size() > 0){
		v_artists[0].num_albums = v_albums.size();
		v_artists[0].num_songs = v_md.size();
	}

	return (v_md.size() > 0);
}


bool SoundcloudHelper::parse_tracks_xml(const QString& content,
					 MetaDataList& v_md){

	bool success;
	QDomDocument doc("Tracks");
	doc.setContent(content);

	QDomElement docElement = doc.documentElement();
	QDomNode entry = docElement.firstChildElement("track");

	v_md.clear();

	if( !entry.hasChildNodes() ) return 0;

	do {

		MetaData md;
		Artist artist;

		success = parse_track_dom(entry, md, artist);
		if(success){
			v_md.push_back(md);
		}

		entry = entry.nextSibling();

	} while( !entry.isNull() );

	return (v_md.size() > 0);

}




bool SoundcloudHelper::parse_artist_dom(QDomNode node, Artist& artist){

	artist.id = -1;

	if(!node.hasChildNodes()) return false;

	QDomNodeList child_nodes = node.childNodes();
	int n_child_nodes = child_nodes.size();

	for(int entry_child=0; entry_child<n_child_nodes; entry_child++)
	{
		QDomNode content = child_nodes.at(entry_child);
		QString nodename = content.nodeName().toLower();
		QDomElement e = content.toElement();

		if(!nodename.compare("id", Qt::CaseInsensitive)){
			artist.id = e.text().toInt();
		}

		else if(!nodename.compare("username", Qt::CaseInsensitive)){
			artist.name = e.text();
		}

		else if(!nodename.compare("track-count", Qt::CaseInsensitive)){
			artist.num_songs = e.text().toInt();
		}

		else if(!nodename.compare("playlist-count", Qt::CaseInsensitive)){
			artist.num_albums = e.text().toInt();
		}
	}

	return (artist.id > 0);
}



bool SoundcloudHelper::parse_track_dom(QDomNode node, MetaData& md, Artist& artist){

	if(!node.hasChildNodes()) return false;

	bool streamable = false;
	QDomNodeList child_nodes = node.childNodes();
	int n_child_nodes = child_nodes.size();

	md.id = -1;
	md.filepath = "";


	for(int entry_child=0; entry_child<n_child_nodes; entry_child++)
	{
		QDomNode content = child_nodes.at(entry_child);
		QString nodename = content.nodeName().toLower();
		QDomElement e = content.toElement();

		md.artist_id = -1;

		if(!nodename.compare("id", Qt::CaseInsensitive)){
			md.id = e.text().toInt();
		}

		else if(!nodename.compare("title", Qt::CaseInsensitive)){
			md.title = e.text();
		}

		else if(!nodename.compare("user-id", Qt::CaseInsensitive)){
			md.artist_id = e.text().toLong();
		}

		else if(!nodename.compare("duration", Qt::CaseInsensitive)){
			md.length_ms = e.text().toLong();
		}

		else if(!nodename.compare("genre", Qt::CaseInsensitive)){
			md.genres << e.text();
		}

		else if(!nodename.compare("release-year", Qt::CaseInsensitive)){
			QString t = e.text();
			if(t.size() > 0) md.year = t.toInt();
		}

		else if(!nodename.compare("stream-url", Qt::CaseInsensitive)){
			md.filepath = e.text();
		}

		else if(!nodename.compare("description", Qt::CaseInsensitive)){
			md.comment = e.text();
		}

		else if(!nodename.compare("streamable", Qt::CaseInsensitive)){
			streamable = (e.text().compare("true", Qt::CaseInsensitive) == 0);
		}

		else if(!nodename.compare("user", Qt::CaseInsensitive) && md.artist_id < 0){

			bool success;
			success = parse_artist_dom(content, artist);
			if(!success) continue;

			md.artist = artist.name;
			md.artist_id = artist.id;
		}
	}

	if(!streamable) return false;
	if(md.id < 0) return false;
	if(md.filepath.size() == 0) return false;

	md.radio_mode = RADIO_SOUNDCLOUD;

	return true;
}
