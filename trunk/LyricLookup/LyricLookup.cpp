/* LyricLookup.cpp */

/* Copyright (C) 2011  Lucio Carreras
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


/*
 * LyricLookup.cpp
 *
 *  Created on: May 21, 2011
 *      Author: luke
 */

#include "LyricLookup/LyricLookup.h"
#include "HelperStructs/Helper.h"
#include <QString>
#include <QDebug>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <QRegExp>
#include <QStringList>

using namespace std;

size_t first_appearance = -1;
size_t last_appearance = -1;
bool b_save;
QStringList lst;
static QString webpage;

size_t get_content_ll( void *ptr, size_t size, size_t nmemb, FILE *userdata){

	(void) userdata;
	char* cptr = (char*) ptr;
	for(uint i=0; i<size * nmemb; i++){

		webpage.append(cptr[i]);
	}

	return size * nmemb;
}



LyricLookup::LyricLookup() {
	init_server_list();
	_cur_server = WIKIA;

}

LyricLookup::~LyricLookup() {
	// TODO Auto-generated destructor stub
}

QString LyricLookup::calc_url(QString artist, QString song){
	QString tmp_artist = artist;
	QString tmp_song = song;

	// apply replacements
	for(int i=0; i<3; i++){
		QMap<QString, QString> replacements = _server_list[_cur_server].replacements;
		for(QMap<QString, QString>::iterator it = replacements.begin(); it != replacements.end(); it++){
			tmp_artist.replace(it.key(), it.value());
			tmp_song.replace(it.key(), it.value());
		}
	}

	QString url = _server_list[_cur_server].call_policy;
	url.replace("<SERVER>", _server_list[_cur_server].server_adress);
	url.replace("<ARTIST>", tmp_artist);
	url.replace("<TITLE>", tmp_song);


	//qDebug() << "url = " << url;

	if(_server_list[_cur_server].to_lower)
		return url.toLower();

	else return url;

}

bool LyricLookup::parse_webpage(QString& dst){

	dst = webpage;

	ServerTemplate t = _server_list[_cur_server];

	int start_idx = dst.indexOf(t.start_tag, 0);
	if(start_idx == -1){
		return false;
	}

	int end_idx = dst.indexOf(t.end_tag, start_idx);
	if(end_idx == -1){
		return false;
	}

	if(!t.include_start_tag) start_idx += t.start_tag.size();
	if(t.include_end_tag) end_idx += t.end_tag.size();



	dst.remove(0, start_idx);

	int num_chars = end_idx - start_idx;


	dst = dst.left(num_chars);

	if(dst.contains(t.error, Qt::CaseInsensitive)){
		return false;
	}

	QString word;
	if(t.is_numeric){
		QRegExp rx("&#(\\d+);|<br />|</span>|</p>");

		QStringList tmplist;;
		int pos = 0;
		while ((pos = rx.indexIn(dst, pos)) != -1) {
		    QString str = rx.cap(1);

		    pos += rx.matchedLength();
		    if(str.size() == 0){
		    	tmplist.push_back(word);
		    	word = "";
		    	tmplist.push_back("<br />");
		    }

		    else{
		    	word.append(QChar(str.toInt()));
		    }
		}

		dst = "";

		foreach(QString str, tmplist){
			dst.append(str);
		}
	}

	return true;

}

QString LyricLookup::find_lyrics(QString artist, QString song, int srv){

		_cur_server = srv;
		b_save = true;

		QString url = this->calc_url(artist, song);

		webpage = "";
		CURL *curl;
		curl = curl_easy_init();

		if(curl){
			curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_content_ll);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}

		QString final_wp;
		if ( !parse_webpage(final_wp) ){
			final_wp = QString("Sorry, no lyrics found");
		}

		final_wp.push_front(_server_list[_cur_server].display_str + "<br /><br />");
		return final_wp;


}

void LyricLookup::init_server_list(){

	ServerTemplate wikia;
	wikia.display_str = "Wikia.com";
	wikia.server_adress = QString("http://lyrics.wikia.com");
	wikia.addReplacement(" ", "_");
	wikia.addReplacement("&", "%26");
	wikia.call_policy = QString("<SERVER>/<ARTIST>:<TITLE>");
	wikia.start_tag = QString("<div class='lyricbox'>");
	wikia.end_tag = QString("<!--");
	wikia.include_start_tag = false;
	wikia.include_end_tag = false;
	wikia.is_numeric = true;
	wikia.to_lower = false;
	wikia.error = "this page needs content";


	ServerTemplate oldieLyrics;
	oldieLyrics.display_str = "OldieLyrics.com";
	oldieLyrics.server_adress = QString("http://www.oldielyrics.com/lyrics");
	oldieLyrics.addReplacement(" ", "_");
	oldieLyrics.addReplacement("(", "_");
	oldieLyrics.addReplacement(")", "_");
	oldieLyrics.addReplacement(".", "_");
	oldieLyrics.addReplacement("&", "_");
	oldieLyrics.addReplacement("'", "");
	oldieLyrics.addReplacement("__", "_");
	oldieLyrics.call_policy = QString("<SERVER>/<ARTIST>/<TITLE>.html");
	oldieLyrics.start_tag = QString("&#");
	oldieLyrics.end_tag = QString("</div>");
	oldieLyrics.include_start_tag = true;
	oldieLyrics.include_end_tag = false;
	oldieLyrics.is_numeric = true;
	oldieLyrics.to_lower = true;
	oldieLyrics.error = QString("error 404");


	ServerTemplate lyricskeeper;
	lyricskeeper.display_str = "Lyricskeeper.de";
	lyricskeeper.server_adress = QString("http://lyricskeeper.de/de");
	lyricskeeper.addReplacement("&", "%26");
	lyricskeeper.addReplacement(" ", "-");
	lyricskeeper.addReplacement("'", "");
	lyricskeeper.addReplacement("--", "-");
	lyricskeeper.call_policy = QString("<SERVER>/<ARTIST>/<TITLE>.html");
	lyricskeeper.start_tag = QString("<div id=\"lyrics\">");
	lyricskeeper.end_tag = QString("</div>");
	lyricskeeper.include_start_tag = false;
	lyricskeeper.include_end_tag = false;
	lyricskeeper.is_numeric = false;
	lyricskeeper.to_lower = true;
	lyricskeeper.error = QString("page cannot be found");



	ServerTemplate metrolyrics;
	metrolyrics.display_str = "MetroLyrics.com";
	metrolyrics.server_adress = QString("http://www.metrolyrics.com");
	metrolyrics.addReplacement("&", "and");
	metrolyrics.addReplacement(" ", "-");
	metrolyrics.call_policy = QString("<SERVER>/<TITLE>-lyrics-<ARTIST>.html");
	metrolyrics.start_tag = QString("<span class='line line-s' id='line_1'>");
	metrolyrics.end_tag = QString("</p>");
	metrolyrics.include_start_tag = false;
	metrolyrics.include_end_tag = false;
	metrolyrics.is_numeric = true;
	metrolyrics.to_lower = true;
	metrolyrics.error = QString("404 page not found");

	_server_list.push_back(wikia);
	_server_list.push_back(oldieLyrics);
	_server_list.push_back(lyricskeeper);
	_server_list.push_back(metrolyrics);

}

QStringList LyricLookup::getServers(){
	QStringList lst;
	foreach(ServerTemplate t, _server_list){
		lst.push_back(t.display_str);
	}

	return lst;

}


