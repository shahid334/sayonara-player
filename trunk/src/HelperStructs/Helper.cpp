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
#include "HelperStructs/WebAccess.h"
#include "DatabaseAccess/CDatabaseConnector.h"

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
#include <QFontMetrics>
#include <QFileInfo>
#include <QMap>
#include <QString>

#include "StreamPlugins/LastFM/LFMGlobals.h"

using namespace std;

static QString _install_path = "";

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
    if(QFile::exists(_install_path + "/share/sayonara")) path = _install_path + "/share/sayonara/";
    else if(QFile::exists("/usr/share/sayonara")) path = "/usr/share/sayonara/";
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

QString Helper::getLibPath(){
    QString path;
#ifndef Q_OS_WIN
    qDebug() << "Check for " << _install_path + "/lib/sayonara";
    if(QFile::exists(_install_path + "/lib/sayonara")) path = _install_path + "/lib/sayonara/";
    else if(QFile::exists("/usr/lib/sayonara")) path = "/usr/lib/sayonara/";
    else path = "";
#else
    path = QDir::homePath() + QString("\\.Sayonara\\images\\");
    if(QFile::exists(path)){
        return path;
    }
    else path = "";
#endif

    qDebug() << "libpath = " << path;
    return path;

}


QString Helper::getIconPath(){

	QString path;

#ifndef Q_OS_WIN
    if(QFile::exists(_install_path + "/share/sayonara")) path = _install_path + "/share/sayonara/";
    else if(QFile::exists("/usr/share/sayonara")) path = "/usr/share/sayonara/";
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

QString Helper::get_artist_image_path(QString artist, QString extension){
	QString token = QString("artist_") + calc_cover_token(artist, "");
    QString image_path = QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + token + "." + extension;


	if(!QFile::exists(QDir::homePath() + QDir::separator() +".Sayonara" + QDir::separator() + "covers")){
		QDir().mkdir(QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers");
	}

	return image_path;
}

QString Helper::get_cover_path(QString artist, QString album, QString extension){
	QString cover_token = calc_cover_token(artist, album);
    QString cover_path =  QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + cover_token + "." + extension;

	if(!QFile::exists(QDir::homePath() + QDir::separator() +".Sayonara" + QDir::separator() + "covers")){
		QDir().mkdir(QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers");
	}

	return cover_path;
}

QString Helper::get_cover_path(int album_id){

    if(album_id == -1) return "";

    Album album;
    bool success = CDatabaseConnector::getInstance()->getAlbumByID(album_id, album);
    if(!success) return "";

    if(album.artists.size() == 0){
        return get_cover_path("", album.name);
    }

    else if(album.artists.size() == 1){
        return get_cover_path(album.artists[0], album.name);
    }

    else if(album.artists.size() == 2){
        return get_cover_path("Various", album.name);
    }

    else return "";
}

QString Helper::createLink(QString name, QString target, bool underline){
	
	int dark = CSettingsStorage::getInstance()->getPlayerStyle();
	if(target.size() == 0) target = name;

	QString content;
	QString style = "";
	
	if(!underline) style = " style: \"text-decoration=none;\" ";

	if(dark){
		content = LIGHT_BLUE(name);
	}
	else content = DARK_BLUE(name);
	

	return QString("<a href=\"") + target + "\"" + style + ">" + content + "</a>";
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


QString Helper::calc_google_image_search_adress(QString searchstring){

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

    return calc_google_image_search_adress(QUrl::toPercentEncoding(artist));
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

    return calc_google_image_search_adress(searchstring);
}



QString Helper::calc_cover_token(QString artist, QString album){
    QString ret = QCryptographicHash::hash(artist.trimmed().toLower().toUtf8() + album.trimmed().toLower().toUtf8(), QCryptographicHash::Md5).toHex();
	return ret;
}



QStringList Helper::get_soundfile_extensions(){

	QStringList filters;
	filters << "*.mp3"
			<< "*.ogg"
            << "*.oga"
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



QStringList Helper::get_podcast_extensions(){

    QStringList filters;

    filters << "*.xml"
            << "*.rss";

    foreach(QString filter, filters){
        filters.push_back(filter.toUpper());
    }

    return filters;
}


bool Helper::is_podcastfile(QString filename, QString* content){
    QStringList extensions = get_podcast_extensions();

    bool extension_ok = false;
    foreach(QString extension, extensions){

        if(filename.toLower().endsWith(extension.right(4).toLower())){
            extension_ok = true;
            break;

        }
    }

    qDebug() << "extension ok? " << extension_ok;
    if(!extension_ok) return false;

    if( Helper::is_www(filename) ){
        qDebug() << "read http into str";
        read_http_into_str(filename, content);
    }

    else{
        read_file_into_str(filename, content);
    }

    QString header = content->left(content->size());
    if(content->size() > 1024) header = content->left(1024);


    if(header.contains("<rss")) return true;
    return false;
}

QString Helper::calc_file_extension(QString filename){

    int last_point = filename.lastIndexOf(".") + 1;
    return filename.right(filename.size() - last_point);

}

void Helper::remove_files_in_directory(QString dir_name, QStringList filters){
	if(filters.size() == 0) filters << "*";

	QDir dir(dir_name);
	dir.setFilter(QDir::Files);
	dir.setNameFilters(filters);
	QStringList file_list = dir.entryList();

	foreach(QString filename, file_list){

		QFile file(dir.absoluteFilePath(filename));
		file.remove();
	}
}

QString Helper::get_parent_folder(QString filename){
    
    QString ret= filename.left(filename.lastIndexOf(QDir::separator()) + 1);
    int last_idx = ret.lastIndexOf(QDir::separator());
    while(last_idx == ret.size() - 1){
	ret = ret.left(ret.size() - 1);
    	last_idx = ret.lastIndexOf(QDir::separator());
    }
    return ret;
}

QString Helper::get_filename_of_path(QString path){
    while(path.endsWith(QDir::separator())) path.remove(path.size() - 1, 1);
    path.remove(Helper::get_parent_folder(path));
    path.remove(QDir::separator());
    return path;
}

void Helper::split_filename(QString src, QString& path, QString& filename){

	path = Helper::get_parent_folder(src);
	filename = Helper::get_filename_of_path(src);	
}



QStringList Helper::extract_folders_of_files(QStringList files){
    QStringList folders;
    foreach(QString file, files){
        QString folder = get_parent_folder(file);
        if(!folders.contains(folder))
            folders << folder;
    }

    return folders;

}

bool Helper::checkTrack(const MetaData& md){


    if( is_www(md.filepath)) return true;

	if( !QFile::exists(md.filepath) && md.id >= 0 ){
		return false;
	}

	return true;
}


bool Helper::read_file_into_str(QString filename, QString* content){

	QFile file(filename);
    content->clear();
    if(!file.open(QIODevice::ReadOnly)){
		return false;
	}

	while (!file.atEnd()) {
        QByteArray arr = file.readLine();
        QString str = QString::fromLocal8Bit(arr);

        content->append(str);
	}

	file.close();

    if(content->size() > 0 ){
		return true;
	}

	return false;

}

bool Helper::read_http_into_str(QString url, QString* content){
    return WebAccess::read_http_into_str(url, content);
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


QString Helper::split_string_to_widget(QString str, QWidget* w, QChar sep){

    QFontMetrics fm(w->font());

    int width = w->width();


    QString subtext = str;
    QStringList lst;

    while(fm.width(subtext) > width){
        int textsize = fm.width(subtext);
        double scale = (width * 1.0) / textsize;
        int idx = subtext.size() * scale - 2;
        if(idx < 0) idx = 0;

        while(idx < subtext.size() && idx >= 0 && subtext.at(idx) != sep){
            idx --;
        }

        if(idx >= 0){

            lst << subtext.left(idx+1);
            subtext = subtext.right(subtext.size() - idx);
        }

        else
            break;

    }

    lst << subtext;
    return lst.join("<br />");
}

bool Helper::is_url(QString str){
    if(is_www(str)) return true;
    if(str.startsWith("file"), Qt::CaseInsensitive) return true;
    return false;
}

bool Helper::is_www(QString str){


    if(str.startsWith("http")) return true;
    else if(str.startsWith("ftp")) return true;
    return false;
}

bool Helper::is_dir(QString filename){
        if(!QFile::exists(filename)) return false;
	QFileInfo fileinfo(filename);
	return fileinfo.isDir();
}

bool Helper::is_file(QString filename){
        if(!QFile::exists(filename)) return false;
	QFileInfo fileinfo(filename);
	return fileinfo.isFile();
}

QString Helper::get_album_w_disc(const MetaData& md){

    if(md.album_id <= 0) return md.album.trimmed();

    QRegExp re(QString("(\\s)?-?(\\s)?((cd)|(CD)|((d|D)((is)|(IS))(c|C|k|K)))(\\d|(\\s\\d))"));
    CDatabaseConnector* db = CDatabaseConnector::getInstance();
    Album album;
    bool success = db->getAlbumByID(md.album_id, album);

    if(!success) return md.album.trimmed();

    if(album.discnumbers.size() > 1 && !album.name.contains(re))
        return album.name.trimmed() + " (Disc " + QString::number(md.discnumber) + ")";

    else return album.name.trimmed();

}


QString Helper::get_album_major_artist(int albumid){

    if(albumid == -1) return "";

    MetaDataList v_md;
    QList<int> idlist;
    idlist << albumid;
    CDatabaseConnector::getInstance()->getAllTracksByAlbum(idlist, v_md);

    if(v_md.size() == 0) return "";
    if(v_md.size() == 1) return v_md[0].artist;

    QMap<QString, int> map;


    foreach(MetaData md, v_md){

        QString alower = md.artist.toLower().trimmed();
        if(!map.keys().contains(alower)) map.insert(alower, 1);
        else map[alower] = map.value(alower) + 1;

    }

    if(map.keys().size() == 0) return "";

    foreach(QString artist, map.keys()){
        if( (map.value(artist) * 100) >= (((int)v_md.size() * 200) / 3)) return artist;
    }

    return QString("Various");

}



Album Helper::get_album_from_metadata(const MetaData& md) {

    Album album;
    CDatabaseConnector* db = CDatabaseConnector::getInstance();
    bool success = false;

    // perfect metadata
    if (md.album_id >= 0){

        success = db->getAlbumByID(md.album_id, album);
    }

    if(success) return album;

    // guess
    int albumID = db->getAlbumID(md.album);
    if(albumID >= 0) success = db->getAlbumByID(albumID, album);

    if(success) return album;

    // assemble album
    album.name = md.album;
    album.artists.clear();
    album.artists.push_back(md.artist);

    return album;
}



QString Helper::get_newest_version(){

    QString str;
    WebAccess::read_http_into_str("http://sayonara.luciocarreras.de/newest", &str);
    return str;

}


void Helper::set_deja_vu_font(QWidget* w, int font_size){
    QFont f = w->font();
    f.setFamily("DejaVu Sans");
    if(font_size > 0){
        f.setPixelSize(font_size);
    }
    f.setStyleStrategy(QFont::PreferAntialias);
    f.setHintingPreference(QFont::PreferNoHinting);
    w->setFont(f);
}

void Helper::set_bin_path(QString str){

    QDir d(str);
    d.cdUp();
    _install_path = d.absolutePath();
    qDebug() << "Install path in " << _install_path;

}
