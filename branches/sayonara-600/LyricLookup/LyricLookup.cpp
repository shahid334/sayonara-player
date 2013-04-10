/* LyricLookupThread.cpp */

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
 * LyricLookupThread.cpp
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
#include <QThread>

using namespace std;

size_t first_appearance = -1;
size_t last_appearance = -1;
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


/****************************************************************************************/
/* THREAD
 ****************************************************************************************/
LyricLookupThread::LyricLookupThread() {
	init_server_list();
	_cur_server = LYRIC_SRV_WIKIA;
	_final_wp.clear();

}

LyricLookupThread::~LyricLookupThread() {
	// TODO Auto-generated destructor stub
}

QString get_artist_wo_the(QString artist){
	QString tmp_artist = artist.toLower();
	tmp_artist.replace("the ", "");
	return artist.right(tmp_artist.size());


}

QString LyricLookupThread::calc_url(QString artist, QString song, bool without_the){

	QString tmp_artist;

	if(!without_the)
		tmp_artist = artist;
	else tmp_artist = get_artist_wo_the(artist);

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
	url.replace("<FIRST_ARTIST_LETTER>", QString(tmp_artist[0]));
	url.replace("<ARTIST>", tmp_artist);
	url.replace("<TITLE>", tmp_song);

	if(_server_list[_cur_server].to_lower)
		return url.toLower();

	else return url;
}

bool LyricLookupThread::parse_webpage(QString& dst){

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

	dst.replace("\n", "<br />");

	return true;
}

void LyricLookupThread::run(){

	if(_artist.size() == 0 && _title.size() == 0) {
		_final_wp = "No track selected";
		return;
	}

	int max_tries = 1;
	if(_artist.toLower().contains("the ")) max_tries = 2;
	int tries = 0;
	while(tries < max_tries){

		QString url = this->calc_url(_artist, _title, (bool) tries);

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

		_final_wp.clear();

		if ( !parse_webpage(_final_wp) ){
			_final_wp = QString("Sorry, no lyrics found<br />" + url);
			tries++;
		}

		else break;

	}
	_final_wp.push_front(_server_list[_cur_server].display_str + "<br /><br />");
	_final_wp.push_front(QString("<font size=\"5\" color=\"#F3841A\"><b>") +
			_artist + QString(" - ") +
			_title +
			"</b></font><br /><br />");
}


void LyricLookupThread::prepare_thread(QString artist, QString song, int srv){

		_artist = artist;
		_title = song;
		_cur_server = srv;
}

void LyricLookupThread::init_server_list(){

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

	ServerTemplate asklyrics;
	asklyrics.display_str = "AskLyrics";
	asklyrics.server_adress = QString("http://www.asklyrics.com/display");
	asklyrics.addReplacement("&", "and");
	asklyrics.addReplacement(" ", "-");
	asklyrics.addReplacement("'", "");
	asklyrics.addReplacement("--", "-");
	asklyrics.addReplacement("(", "-");
	asklyrics.addReplacement(")", "-");
	asklyrics.call_policy = QString("<SERVER>/<ARTIST>/<TITLE>-lyrics.htm");
	asklyrics.start_tag = QString("<pre class=\"lc\">");
	asklyrics.end_tag = QString("</pre>");
	asklyrics.include_start_tag = false;
	asklyrics.include_end_tag = false;
	asklyrics.is_numeric = false;
	asklyrics.to_lower = true;
	asklyrics.error = QString("Error 404");


	ServerTemplate elyrics;
	elyrics.display_str = "eLyrics";
	elyrics.server_adress= QString("http://www.elyrics.net/read");
	elyrics.addReplacement(" ", "-");
	elyrics.addReplacement("the ", "");
	elyrics.addReplacement("The ", "");
	elyrics.addReplacement("'", "_");
	elyrics.call_policy = QString("<SERVER>/<FIRST_ARTIST_LETTER>/<ARTIST>-lyrics/<TITLE>-lyrics.html");
	elyrics.start_tag= QString("lyrics</strong><br>");
	elyrics.include_start_tag = false;
	elyrics.end_tag = QString("</div>");
	elyrics.include_end_tag = false;
	elyrics.is_numeric = false;
	elyrics.to_lower = true;
	elyrics.error = QString("Error 404");


	_server_list.push_back(wikia);
	_server_list.push_back(oldieLyrics);
	_server_list.push_back(lyricskeeper);
	_server_list.push_back(metrolyrics);
	_server_list.push_back(asklyrics);
	_server_list.push_back(elyrics);

}

QStringList LyricLookupThread::getServers(){
	QStringList lst;
	foreach(ServerTemplate t, _server_list){
		lst.push_back(t.display_str);
	}

	return lst;
}

QString LyricLookupThread::getFinalLyrics(){
	return _final_wp;
}

