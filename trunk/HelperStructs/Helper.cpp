/* Helper.cpp */

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
 * Helper.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/globals.h"

#include <string>
#include <iostream>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <ctime>

#include <QDir>
#include <QUrl>
#include <QString>
#include <QCryptographicHash>
#include <QDebug>
#include <QList>
#include <QFile>



using namespace std;


template<typename T>
string cvtNum2String(const T & num){
	stringstream sstr;
	sstr << num;
	return sstr.str();

}


template<typename T>
QString cvtSomething2QString(const T & sth){
	stringstream sstr;
	sstr << sth;
	return QString(sstr.str().c_str());
}


void cvtSecs2MinAndSecs(int secs, int* tgt_min, int* tgt_sec)
{

	*tgt_min = secs / 60;
	*tgt_sec = secs % 60;

}


string Helper::trim(const string & toTrim){

	if(toTrim.size() == 0) return toTrim;

	const char* arr_src = toTrim.c_str();

	int count_whitespaces = 0;
	for(int i=toTrim.size()-1; i>=0; i--){
		if(!isspace(arr_src[i])){
			break;
		}

		else count_whitespaces ++;
	}

	char* arr_dst = new char[toTrim.size() - count_whitespaces + 1];

	for(uint i=0; i<toTrim.size() - count_whitespaces; i++){
		arr_dst[i] = arr_src[i];
	}

	arr_dst[toTrim.size() - count_whitespaces] = '\0';

	string retStr = string(arr_dst);
	delete arr_dst;
	return retStr;


}


QString Helper::getIconPath(){


	QString path;
#ifndef Q_OS_WIN
		if(QFile::exists("/usr/share/sayonara")) path = "/usr/share/sayonara/";
	else path = "./GUI/icons/";
#else
	path = QDir::homePath() + QString("\\.Sayonara\\images\\");
	if(QFile::exists(path)){
		return path;
	}
	else return QString("");
#endif

	return path;

}


QString Helper::get_cover_path(QString artist, QString album){
	QString cover_token = calc_cover_token(artist, album);
	QString cover_path =  QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + cover_token + ".jpg";

	if(!QFile::exists(QDir::homePath() + QDir::separator() +".Sayonara" + QDir::separator() + "covers")){
		QDir().mkdir(QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers");
	}

	return cover_path;


}


QString Helper::calc_cover_lfm_adress(QString artist, QString album){

	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=album.getinfo&api_key=");
	QString api_key = QString("51d6f9eaef806f603f346844bef326ba");

	if(album == "")
		url = QString ("http://ws.audioscrobbler.com/2.0/?method=artist.getinfo&api_key=");

	url += api_key + "&";
	url += "artist=" + QUrl::toPercentEncoding(artist);
	if(album != "") url += "&album=" + QUrl::toPercentEncoding(album);
	return url;

}


QString Helper::calc_cover_google_adress(QString artist, QString album){

	QString url = QString("http://www.google.de/images?q=");

	artist = QUrl::toPercentEncoding(artist);

	album = album.toLower();

	album = album.remove(QRegExp(QString("(\\s)?-?(\\s)?((cd)|(CD)|((d|D)((is)|(IS))(c|C|k|K)))(\\d|(\\s\\d))")));
	album = album.replace("()", "");
	album = album.replace("( )", "");
	album = album.trimmed();
	album = QUrl::toPercentEncoding(album);



	url += artist + "+" + album;
	url +=  QString("&tbs=isz:s,ift:jpg");			// klein*/

	return url;
}

QString Helper::calc_cover_token(QString artist, QString album){
	QString ret = QCryptographicHash::hash(artist.toUtf8() + album.toUtf8(), QCryptographicHash::Md5).toHex();
	return ret;
}



QStringList Helper::get_soundfile_extensions(){

	QStringList filters;
	filters.push_back("*.mp3");
	filters.push_back("*.ogg");
	filters.push_back("*.m4a");
	filters.push_back("*.wma");
	filters.push_back("*.wav");
	filters.push_back("*.flac");
	filters.push_back("*.aac");

	// ok, these files contain sound :)
	filters.push_back("*.avi");
	filters.push_back("*.flv");
	filters.push_back("*.mpg");
	filters.push_back("*.mpeg");
	filters.push_back("*.mkv");
	filters.push_back("*.wmv");

	return filters;
}

bool Helper::is_soundfile(QString filename){
	QStringList extensions = get_soundfile_extensions();
	for(int i=0; i<=6; i++){
		if(filename.endsWith(extensions[i].right(4))){
			return true;
		}
	}

	return false;
}


bool Helper::checkTrack(const MetaData& md){

	if( md.filepath.startsWith("http")) return true;
	if( md.filepath.startsWith("ftp")) return true;
	if( md.filepath.startsWith("mms")) return true;

	if( !QFile::exists(md.filepath) && md.id >= 0 ){

		return false;
	}

	return true;
}




bool Helper::read_file_into_str(QString filename, QString& content){

	QFile file(filename);
	content.clear();
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		return false;
	}

	while (!file.atEnd()) {
		content += file.readLine();
	}

	file.close();

	if(content.size() > 0 ){
		return true;
	}

	return false;

}




QString Helper::calc_album_lfm_adress(QString album){

	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=album.search&api_key=");


	url += LFM_API_KEY + "&album=" + QUrl::toPercentEncoding(album);
	return url;
}



QString Helper::calc_search_album_adress(QString album){

	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=album.search&api_key=");

	url += LFM_API_KEY + "&album=" + QUrl::toPercentEncoding(album);
	return url;
}

QString Helper::calc_search_artist_adress(QString artist){

	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=artist.search&api_key=");

	url += LFM_API_KEY + "&album=" + QUrl::toPercentEncoding(artist);
	return url;
}



