/*
 * Helper.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#include "HelperStructs/Helper.h"
#include "LastFM/LastFM.h"

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

QString Helper::calc_album_lfm_adress(QString album){
	LastFM lfm;
	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=album.search&api_key=");
	QString api_key = lfm.get_api_key();

	url += api_key + "&album=" + QUrl::toPercentEncoding(album);
	return url;
}

QString Helper::calc_cover_lfm_adress(QString artist, QString album){
	LastFM lfm;
	QString api_key = lfm.get_api_key();
	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=album.getinfo&api_key=");

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

QString Helper::calc_search_album_adress(QString album){
	LastFM lfm;
	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=album.search&api_key=");
	QString api_key = lfm.get_api_key();

	url += api_key + "&album=" + QUrl::toPercentEncoding(album);
	return url;
}

QString Helper::calc_search_artist_adress(QString artist){

	LastFM lfm;
	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=artist.search&api_key=");
	QString api_key = lfm.get_api_key();

	url += api_key + "&album=" + QUrl::toPercentEncoding(artist);
	return url;
}

