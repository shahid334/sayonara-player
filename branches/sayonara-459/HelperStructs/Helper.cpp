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

#include "StreamPlugins/LastFM/LFMGlobals.h"

using namespace std;


QString Helper::cvtQString2FirstUpper(QString str){

	QString ret_str = "";
	QStringList lst = str.split(" ");

    foreach(QString word, lst){
        QString first = word.left(1);
		word.remove(0,1);
        word = first.toUpper() + word + " ";

        ret_str += word;
	}

    return ret_str.left(ret_str.size() - 1);
}

QString cvtNum2String(int num, int digits){
	QString str = QString::number(num);
	while(str.size() < digits){
		str.prepend("0");
	}

	return str;
}

QString Helper::cvtMsecs2TitleLengthString(long int msec, bool colon, bool show_days){

		bool show_hrs = false;

		int sec = msec / 1000;
		int min = sec / 60;

		int secs = sec % 60;
		int hrs = min / 60;
		int days = hrs / 24;

		QString final_str;

        if(days > 0 && show_days){
			final_str += QString::number(days) + "d ";
			hrs = hrs % 24;
			show_hrs = true;
		}

        if(!show_days){
            hrs += (days * 24);
        }

		if(hrs > 0 || show_hrs){
			final_str += QString::number(hrs) + "h ";
			min = min % 60;
		}

		if(colon)
			final_str +=  cvtNum2String(min, 2) + ":" + cvtNum2String(secs, 2);
		else
			final_str +=  cvtNum2String(min, 2) + "m " + cvtNum2String(secs, 2);

		return final_str;

	}



QString Helper::getSharePath(){

    QString path;
#ifndef Q_OS_WIN
        if(QFile::exists("/usr/share/sayonara")) path = "/usr/share/sayonara/";
    else path = "";
#else
    path = QDir::homePath() + QString("\\.Sayonara\\images\\");
    if(QFile::exists(path)){
        return path;
    }
    else path = "";
#endif

    return path;

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

QString Helper::getSayonaraPath(){
	return QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator();
}

QString Helper::get_artist_image_path(QString artist){
	QString token = QString("artist_") + calc_cover_token(artist, "");
	QString image_path = QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + token + ".jpg";


	if(!QFile::exists(QDir::homePath() + QDir::separator() +".Sayonara" + QDir::separator() + "covers")){
		QDir().mkdir(QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers");
	}

	return image_path;
}

QString Helper::get_cover_path(QString artist, QString album){
	QString cover_token = calc_cover_token(artist, album);
	QString cover_path =  QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + cover_token + ".jpg";

	if(!QFile::exists(QDir::homePath() + QDir::separator() +".Sayonara" + QDir::separator() + "covers")){
		QDir().mkdir(QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers");
	}

	return cover_path;


}

QString Helper::calc_filesize_str(qint64 filesize){
    qint64 kb = 1024;
    qint64 mb = kb * 1024;
    qint64 gb = mb * 1024;

    QString size;
    if(filesize > gb){

        size = QString::number(filesize / gb) + "." + QString::number((filesize / mb) % gb).left(2)  + " GB";
    }

    else if (filesize > mb){
        size = QString::number(filesize / mb) + "." + QString::number((filesize / kb) % mb).left(2)  + " MB";
    }

    else {
        size = QString::number( filesize / kb) + " KB";
    }

    return size;
}

QString Helper::calc_lfm_artist_adress(QString artist){
	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=artist.getinfo&api_key=");
	QString api_key = LFM_API_KEY;
	url += api_key + "&";
	url += "artist=" + QUrl::toPercentEncoding(artist);
	return url;
}


QString Helper::calc_lfm_album_adress(QString artist, QString album){

	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=album.getinfo&api_key=");
	QString api_key = LFM_API_KEY;

	if(album == "")
		url = QString ("http://ws.audioscrobbler.com/2.0/?method=artist.getinfo&api_key=");

	url += api_key + "&";
	url += "artist=" + QUrl::toPercentEncoding(artist);
	if(album != "") url += "&album=" + QUrl::toPercentEncoding(album);
	return url;

}


QString Helper::calc_google_image_search_adress(QString searchstring, QString size, QString filetype){

	searchstring.replace(" ", "%20");
	searchstring.replace("/", "%2F");
	searchstring.replace("&", "%26");
	searchstring.replace("$", "%24");

    QString url = QString("https://www.google.de/search?num=20&hl=de&site=imghp&tbm=isch&source=hp");
    url += QString("&q=") + searchstring;
    url += QString("&oq=") + searchstring;

	return url;
}


QString Helper::calc_google_artist_adress(QString artist){

	return calc_google_image_search_adress(QUrl::toPercentEncoding(artist), GOOGLE_IMG_SMALL, GOOGLE_FT_JPG);
}


QString Helper::calc_google_album_adress(QString artist, QString album){

	artist = QUrl::toPercentEncoding(artist);

	album = album.toLower();

	album = album.remove(QRegExp(QString("(\\s)?-?(\\s)?((cd)|(CD)|((d|D)((is)|(IS))(c|C|k|K)))(\\d|(\\s\\d))")));
	album = album.replace("()", "");
	album = album.replace("( )", "");
	album = album.trimmed();
	album = QUrl::toPercentEncoding(album);

	QString searchstring = artist;
	if(searchstring.size() > 0) searchstring += "+";
	searchstring += album;

	return calc_google_image_search_adress(searchstring, GOOGLE_IMG_SMALL, GOOGLE_FT_JPG);
}



QString Helper::calc_cover_token(QString artist, QString album){
	QString ret = QCryptographicHash::hash(artist.toUtf8() + album.toUtf8(), QCryptographicHash::Md5).toHex();
	return ret;
}



QStringList Helper::get_soundfile_extensions(){

	QStringList filters;
	filters << "*.mp3"
			<< "*.ogg"
			<< "*.m4a"
			<< "*.wav"
			<< "*.flac"
			<< "*.aac"
			<< "*.wma";

	QString bla;

	foreach(QString filter, filters){
		filters.push_back(filter.toUpper());
	}


	/*
	filters << "*.avi"
			<< "*.flv"
			<< "*.mpg"
			<< "*.mpeg"
			<< "*.mkv"
			<< "*.wmv"
			<< "*.vob";*/

	return filters;
}

bool Helper::is_soundfile(QString filename){
	QStringList extensions = get_soundfile_extensions();
	foreach(QString extension, extensions){
		if(filename.toLower().endsWith(extension.right(4).toLower())){
			return true;
		}
	}

	return false;
}


QStringList Helper::get_playlistfile_extensions(){
	QStringList filters;

	filters << "*.pls"
			<< "*.m3u"
			<< "*.ram"
			<< "*.asx";


	foreach(QString filter, filters){
		filters.push_back(filter.toUpper());
	}

	return filters;
}

bool Helper::is_playlistfile(QString filename){
	QStringList extensions = get_playlistfile_extensions();
	foreach(QString extension, extensions){
		if(filename.toLower().endsWith(extension.right(4).toLower())){
			return true;
		}
	}

	return false;
}



bool Helper::checkTrack(const MetaData& md){

    if( md.filepath.startsWith("http", Qt::CaseInsensitive)) return true;
    if( md.filepath.startsWith("ftp", Qt::CaseInsensitive)) return true;
    if( md.filepath.startsWith("mms", Qt::CaseInsensitive)) return true;

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


QString Helper::easy_tag_finder(QString tag, QString& xml_doc){

	int p = tag.indexOf('.');
	QString t = tag;
	QString t_rev;
	QString new_xml_doc = xml_doc;

	while(p > 0){

		t = tag.left(p);
		t_rev = tag.right(tag.length() - p -1);

		new_xml_doc = easy_tag_finder(t, new_xml_doc);
		p = t_rev.indexOf('.');
		tag = t_rev;
	}

	t = tag;

	QString str2search_start = QString("<") + t + QString(".*>");
	QString str2search_end = QString("</") + t + QString(">");
	QString str2search = str2search_start + "(.+)" + str2search_end;
	QRegExp rx(str2search);
	rx.setMinimal(true);


	int pos = 0;
	if(rx.indexIn(new_xml_doc, pos) != -1) {
		return rx.cap(1);
	}

	return "";
}

QString Helper::calc_hash(QString data){
	return QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Md5).toHex();
}


