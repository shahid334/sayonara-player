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
#include "HelperStructs/globals.h"
#include "HelperStructs/WebAccess.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "Settings/Settings.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>

#include <QDir>
#include <QUrl>
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QFontMetrics>
#include <QFileInfo>
#include <QMap>

#include "StreamPlugins/LastFM/LFMGlobals.h"

using namespace std;

static QString _install_path = "";

QString Helper::cvtQString2FirstUpper(const QString& str) {

	QString ret_str = "";
	QStringList lst = str.split(" ");

    foreach(QString word, lst) {
        QString first = word.left(1);
		word.remove(0,1);
        word = first.toUpper() + word + " ";

        ret_str += word;
	}

    return ret_str.left(ret_str.size() - 1);
}
template<typename T>
QString cvtNum2String(T num, int digits) {
	QString str = QString::number(num);
	while(str.size() < digits) {
		str.prepend("0");
	}

	return str;
}

QString Helper::cvt_ms_to_string(quint64 msec, bool empty_zero, bool colon, bool show_days) {

		if(msec == 0 && empty_zero){
			return "";
		}

		bool show_hrs = false;

		int sec = msec / 1000;
		int min = sec / 60;

		int secs = sec % 60;
		int hrs = min / 60;
		int days = hrs / 24;

		QString final_str;

        if(days > 0 && show_days) {
			final_str += QString::number(days) + "d ";
			hrs = hrs % 24;
			show_hrs = true;
		}

        if(!show_days) {
            hrs += (days * 24);
        }

		if(hrs > 0 || show_hrs) {
			final_str += QString::number(hrs) + "h ";
			min = min % 60;
		}

		if(colon)
			final_str +=  cvtNum2String(min, 2) + ":" + cvtNum2String(secs, 2);
		else
			final_str +=  cvtNum2String(min, 2) + "m " + cvtNum2String(secs, 2);

		return final_str;

	}



QString Helper::getSharePath() {

    QString path;
#ifndef Q_OS_WIN
    if(QFile::exists(_install_path + "/share/sayonara")) path = _install_path + "/share/sayonara/";
    else if(QFile::exists("/usr/share/sayonara")) path = "/usr/share/sayonara/";
    else path = "";
#else
    path = QDir::homePath() + QString("\\.Sayonara\\images\\");
    if(QFile::exists(path)) {
        return path;
    }
    else path = "";
#endif

    return path;

}

QString Helper::getLibPath() {
    QString path;
#ifndef Q_OS_WIN
    qDebug() << "Check for " << _install_path + "/lib/sayonara";
    if(QFile::exists(_install_path + "/lib/sayonara")) path = _install_path + "/lib/sayonara/";
    else if(QFile::exists("/usr/lib/sayonara")) path = "/usr/lib/sayonara/";
    else path = "";
#else
    path = QDir::homePath() + QString("\\.Sayonara\\images\\");
    if(QFile::exists(path)) {
        return path;
    }
    else path = "";
#endif

    qDebug() << "libpath = " << path;
    return path;
}

QPixmap Helper::getPixmap(const QString& icon_name, QSize sz, bool keep_aspect){

	QString path = QString(":/icons/") + icon_name;
	QPixmap pixmap(path);

    if(sz.width() == 0){
        return pixmap;
    }

    else{
        if(keep_aspect){
            return pixmap.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        else{
            return pixmap.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
    }

    return pixmap;
}


QIcon Helper::getIcon(const QString& icon_name){
	return QIcon(QString(":/icons/") + icon_name);
}


QString Helper::getErrorFile(){
	return getSayonaraPath() + "error_log";
}

QString Helper::getSayonaraPath() {
	return QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator();
}


QString Helper::createLink(const QString& name, const QString& target, bool underline) {
	
	bool dark = (Settings::getInstance()->get(Set::Player_Style) == 1);

	QString new_target;
	QString content;
	QString style = "";
	QString ret;


	if(target.size() == 0){
		new_target = name;
	}

	else {
		new_target = target;
	}

	if(!underline) style = " style: \"text-decoration=none;\" ";

	if(dark) {
		content = LIGHT_BLUE(name);
	}
	else {
		content = DARK_BLUE(name);
	}
	
	if(new_target.contains("://") || new_target.contains("mailto:")){
		ret = QString("<a href=\"") + new_target + "\"" + style + ">" + content + "</a>";
	}

	else {
		ret = QString("<a href=\"file://") + new_target + "\"" + style + ">" + content + "</a>";
	}

	return ret;
}

QString Helper::calc_filesize_str(quint64 filesize) {
	quint64 kb = 1024;
	quint64 mb = kb * 1024;
	quint64 gb = mb * 1024;

    QString size;
    if(filesize > gb) {
        size = QString::number(filesize / gb) + "." + QString::number((filesize / mb) % gb).left(2)  + " GB";
    }

    else if (filesize > mb) {
        size = QString::number(filesize / mb) + "." + QString::number((filesize / kb) % mb).left(2)  + " MB";
    }

    else {
        size = QString::number( filesize / kb) + " KB";
    }

    return size;
}

QString Helper::calc_lfm_artist_adress(const QString& artist) {
	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=artist.getinfo&api_key=");
	QString api_key = LFM_API_KEY;
	url += api_key + "&";
	url += "artist=" + QUrl::toPercentEncoding(artist);
	return url;
}


QString Helper::calc_lfm_album_adress(const QString& artist, const QString& album) {

	QString url = QString ("http://ws.audioscrobbler.com/2.0/?method=album.getinfo&api_key=");
	QString api_key = LFM_API_KEY;

	if(album == "")
		url = QString ("http://ws.audioscrobbler.com/2.0/?method=artist.getinfo&api_key=");

	url += api_key + "&";
	url += "artist=" + QUrl::toPercentEncoding(artist);
	if(album != "") url += "&album=" + QUrl::toPercentEncoding(album);
	return url;

}


QString Helper::calc_google_image_search_adress(const QString& str) {

	QString searchstring = str;
	searchstring.replace(" ", "%20");
	searchstring.replace("/", "%2F");
	searchstring.replace("&", "%26");
	searchstring.replace("$", "%24");

	QString url = QString("https://www.google.de/search?num=20&hl=de&site=imghp&tbm=isch&source=hp");

    url += QString("&q=") + searchstring;
    url += QString("&oq=") + searchstring;

	return url;
}


QString Helper::calc_google_artist_adress(const QString& artist) {

	return calc_google_image_search_adress(QUrl::toPercentEncoding(artist));
}


QString Helper::calc_google_album_adress(const QString& artist, const QString& album) {

	QString new_album, searchstring;
	QRegExp regex;

	searchstring = QUrl::toPercentEncoding(artist);
	new_album = album;

	regex = QRegExp(QString("(\\s)?-?(\\s)?((cd)|(CD)|((d|D)((is)|(IS))(c|C|k|K)))(\\d|(\\s\\d))"));

	new_album = new_album.toLower();
	new_album = new_album.remove(regex);
	new_album = new_album.replace("()", "");
	new_album = new_album.replace("( )", "");
	new_album = new_album.trimmed();
	new_album = QUrl::toPercentEncoding(album);

	if(searchstring.size() > 0) {
		searchstring += "+";
	}

	searchstring += new_album;

	return calc_google_image_search_adress(searchstring);
}



QString Helper::calc_cover_token(const QString& artist, const QString& album) {

	QByteArray str = QString(artist.trimmed() + album.trimmed()).toLower().toUtf8();

	return calc_hash(str);
}

QStringList Helper::get_soundfile_extensions() {

	QStringList filters;
	filters << "*.mp3"
			<< "*.ogg"
            << "*.oga"
			<< "*.m4a"
			<< "*.wav"
			<< "*.flac"
			<< "*.aac"
			<< "*.wma";

	foreach(QString filter, filters) {
		filters.push_back(filter.toUpper());
	}

	return filters;
}

bool Helper::is_soundfile(const QString& filename) {

	QStringList extensions = get_soundfile_extensions();
	foreach(QString extension, extensions) {
		if(filename.toLower().endsWith(extension.right(4))) {
			return true;
		}
	}

	return false;
}


QStringList Helper::get_playlistfile_extensions() {

	QStringList filters;

	filters << "*.pls"
			<< "*.m3u"
			<< "*.ram"
			<< "*.asx";


	foreach(QString filter, filters) {
		filters.push_back(filter.toUpper());
	}

	return filters;
}



bool Helper::is_playlistfile(const QString& filename) {
	QStringList extensions = get_playlistfile_extensions();
	foreach(QString extension, extensions) {
		if(filename.toLower().endsWith(extension.right(4).toLower())) {
			return true;
		}
	}

	return false;
}



QStringList Helper::get_podcast_extensions() {

    QStringList filters;

    filters << "*.xml"
            << "*.rss";

    foreach(QString filter, filters) {
        filters.push_back(filter.toUpper());
    }

    return filters;
}


bool Helper::is_podcastfile(const QString& filename, QString* content) {
    QStringList extensions = get_podcast_extensions();

    bool extension_ok = false;
    foreach(QString extension, extensions) {

        if(filename.toLower().endsWith(extension.right(4).toLower())) {
            extension_ok = true;
            break;

        }
    }

    qDebug() << "extension ok? " << extension_ok;
    if(!extension_ok) return false;

    if( Helper::is_www(filename) ) {
		qDebug() << "read http into str " << filename;
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

QString Helper::calc_file_extension(const QString& filename) {

    int last_point = filename.lastIndexOf(".") + 1;
    return filename.right(filename.size() - last_point);

}

void Helper::remove_files_in_directory(const QString& dir_name, const QStringList& filters) {

	QStringList file_list;
	QDir dir(dir_name);
	dir.setFilter(QDir::Files);

	if(filters.size() == 0) {
		QStringList tmp_list;
		tmp_list << "*";
		dir.setNameFilters(tmp_list);
	}

	else {
		dir.setNameFilters(filters);
	}

	file_list = dir.entryList();

	foreach(QString filename, file_list) {

		QFile file(dir.absoluteFilePath(filename));
		file.remove();
	}
}

QString Helper::get_parent_folder(const QString& filename) {
    
	QString ret;
	int last_idx;

	ret = filename.left(filename.lastIndexOf(QDir::separator()) + 1);
	last_idx = ret.lastIndexOf(QDir::separator());

	while(last_idx == ret.size() - 1 && ret.size() > 0) {
		ret = ret.left(ret.size() - 1);
    	last_idx = ret.lastIndexOf(QDir::separator());
    }

	return ret;
}

QString Helper::get_filename_of_path(const QString& path) {

	QString ret = path;

	while(ret.endsWith(QDir::separator())){
		ret.remove(path.size() - 1, 1);
	}

	ret.remove(Helper::get_parent_folder(path));
	ret.remove(QDir::separator());

	return ret;
}

void Helper::split_filename(const QString& src, QString& path, QString& filename) {

	path = Helper::get_parent_folder(src);
	filename = Helper::get_filename_of_path(src);	
}



QStringList Helper::extract_folders_of_files(const QStringList& files) {
    QStringList folders;
    foreach(QString file, files) {
        QString folder = get_parent_folder(file);
		if(!folders.contains(folder)){
			folders << folder;
		}
    }

    return folders;

}

bool Helper::checkTrack(const MetaData& md) {

    if( is_www(md.filepath)) return true;

    return QFile::exists(md.filepath);
}

bool Helper::read_file_into_byte_arr(const QString& filename, QByteArray& content){
	QFile file(filename);
	content.clear();


	if(!file.open(QIODevice::ReadOnly)){
		return false;
	}

	while(!file.atEnd()){
		QByteArray arr = file.read(4096);
		content.append(arr);
	}

	file.close();

	return (content.size() > 0);
}


bool Helper::read_file_into_str(const QString& filename, QString* content) {

	QFile file(filename);
    content->clear();
    if(!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	while (!file.atEnd()) {
        QByteArray arr = file.readLine();
        QString str = QString::fromLocal8Bit(arr);

        content->append(str);
	}

	file.close();

    if(content->size() > 0 ) {
		return true;
	}

	return false;

}

bool Helper::read_http_into_str(const QString& url, QString* content) {
    return WebAccess::read_http_into_str(url, content);
}

bool Helper::read_http_into_img(const QString& url, QImage* image) {
    return WebAccess::read_http_into_img(url, image);
}


QString Helper::easy_tag_finder(const QString& tag, const QString& xml_doc) {

	int p = tag.indexOf('.');
	QString ret = tag;
	QString new_tag = tag;
	QString t_rev;
	QString new_xml_doc = xml_doc;

	while(p > 0) {

		ret = new_tag.left(p);
		t_rev = tag.right(new_tag.length() - p -1);

		new_xml_doc = easy_tag_finder(ret, new_xml_doc);
		p = t_rev.indexOf('.');
		new_tag = t_rev;
	}

	ret = new_tag;

	QString str2search_start = QString("<") + ret + QString(".*>");
	QString str2search_end = QString("</") + ret + QString(">");
	QString str2search = str2search_start + "(.+)" + str2search_end;
	QRegExp rx(str2search);
	rx.setMinimal(true);


	int pos = 0;
	if(rx.indexIn(new_xml_doc, pos) != -1) {
		return rx.cap(1);
	}

	return "";
}

QString Helper::calc_hash(const QString& data) {
	return QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Md5).toHex();
}


QString Helper::split_string_to_widget(QString str, QWidget* w, QChar sep) {

    QFontMetrics fm(w->font());

    int width = w->width();


    QString subtext = str;
    QStringList lst;

    while(fm.width(subtext) > width) {
        int textsize = fm.width(subtext);
        double scale = (width * 1.0) / textsize;
        int idx = subtext.size() * scale - 2;
        if(idx < 0) idx = 0;

        while(idx < subtext.size() && idx >= 0 && subtext.at(idx) != sep) {
            idx --;
        }

        if(idx >= 0) {

            lst << subtext.left(idx+1);
            subtext = subtext.right(subtext.size() - idx);
        }

        else
            break;

    }

    lst << subtext;
    return lst.join("<br />");
}

bool Helper::is_url(const QString& str) {
    if(is_www(str)) return true;
    if(str.startsWith("file"), Qt::CaseInsensitive) return true;
    return false;
}

bool Helper::is_www(const QString& str) {

    if(str.startsWith("http")) return true;
    else if(str.startsWith("ftp")) return true;
    return false;
}

bool Helper::is_dir(const QString& filename) {
	if(!QFile::exists(filename)) return false;
	QFileInfo fileinfo(filename);
	return fileinfo.isDir();
}

bool Helper::is_file(const QString& filename) {
	if(!QFile::exists(filename)) return false;
	QFileInfo fileinfo(filename);
	return fileinfo.isFile();
}

QString Helper::get_album_w_disc(const MetaData& md) {

	if(md.album_id < 0){
        return md.album.trimmed();
    }

    QString ret;
    Album album;
    bool success;
    CDatabaseConnector* db = CDatabaseConnector::getInstance();

    QRegExp re(QString("(\\s)?-?(\\s)?((cd)|(CD)|((d|D)((is)|(IS))(c|C|k|K)))(\\d|(\\s\\d))"));

    success = db->getAlbumByID(md.album_id, album);

    if(!success){
        ret =  md.album.trimmed();
    }

    else if(album.discnumbers.size() > 1 && !album.name.contains(re)){
        ret = album.name.trimmed() + " (Disc " + QString::number(md.discnumber) + ")";
    }

    else {
        ret = album.name.trimmed();
    }

    return ret;
}


QString Helper::get_major_artist(const ArtistList& artists) {

    QStringList lst;

    foreach(Artist artist, artists) {
        lst << artist.name;
    }

    return get_major_artist(lst);
}

QString Helper::get_major_artist(const QStringList& artists) {

    QMap<QString, int> map;
    if(artists.size() == 0) return "";
	if(artists.size() == 1) return artists[0].toLower().trimmed();

    int n_artists = artists.size();

    foreach(QString artist, artists) {

        QString alower = artist.toLower().trimmed();

        // count appearance of artist
        if( !map.keys().contains(alower) ) {
            map.insert(alower, 1);
        }
        else {
            map[alower] = map.value(alower) + 1;
        };
    }

    // n_appearances have to be at least 2/3 of all apperances
    foreach(QString artist, map.keys()) {

        int n_appearances = map.value(artist);
        if(n_appearances * 3 > n_artists * 2) return artist;

    }

    return QString("Various");
}

QString Helper::get_album_major_artist(int album_id) {

	if(album_id == -1) return "";

    QStringList artists;
    MetaDataList v_md(0);
    QList<int> ids;

    CDatabaseConnector* db = CDatabaseConnector::getInstance();

    ids << album_id;
    db->getAllTracksByAlbum(ids, v_md);

    if(v_md.size() == 0) return "";
    if(v_md.size() == 1) return v_md[0].artist;

    foreach(MetaData md, v_md) {
        artists << md.artist;
    }

    return get_major_artist(artists);
}


QString Helper::get_newest_version() {

    QString str;
    WebAccess::read_http_into_str("http://sayonara.luciocarreras.de/newest", &str);
    return str;

}


void Helper::set_deja_vu_font(QWidget* w, int font_size) {
    QFont f = w->font();
    f.setFamily("DejaVu Sans");
    if(font_size > 0) {
        f.setPixelSize(font_size);
    }
    f.setStyleStrategy(QFont::PreferAntialias);
    f.setHintingPreference(QFont::PreferNoHinting);
    w->setFont(f);
}

void Helper::set_bin_path(const QString& str) {

    QDir d(str);
    d.cdUp();
    _install_path = d.absolutePath();
    qDebug() << "Install path in " << _install_path;

}


QString Helper::get_location_from_ip(const QString& ip){

	bool success;
	QString content;
	QString url = QString("http://freegeoip.net/xml/") + ip;

	success = WebAccess::read_http_into_str(url, &content);

	if(success){
		QString country = Helper::easy_tag_finder("Response.CountryName", content);

		QString city = Helper::easy_tag_finder("Response.City", content);

		QString ret = "";

		if(!city.isEmpty()){
			ret = city;
		}

		if(!country.isEmpty()){
			ret += ", " + country;
		}

		if(ret.startsWith(",")){
			ret.remove(0, 2);
		}

		qDebug() << "Return " << ret;
		return ret;
	}

	return "";
}


void Helper::sleep_ms(unsigned long ms){
	_Sleeper::sleep_ms(ms);
}
