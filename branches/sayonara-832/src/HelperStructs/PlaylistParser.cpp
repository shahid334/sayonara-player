/* PlaylistParser.cpp */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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


#include "HelperStructs/PlaylistParser.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/WebAccess.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Tagging/id3.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QStringList>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <qdom.h>


static int parse_m3u(QString file_content, MetaDataList& v_md, QString abs_path="");
static int parse_asx(QString file_content, MetaDataList& v_md, QString abs_path="");
static int parse_pls(QString file_content, MetaDataList& v_md, QString abs_path="");
static CDatabaseConnector* db = CDatabaseConnector::getInstance();

QString _correct_filepath(QString filepath, QString abs_path){

    if(Helper::is_www(filepath)) return filepath;


    bool is_absolute = QDir(filepath).isAbsolute();
    QString tmp_filepath;
    if(!is_absolute){
        tmp_filepath = abs_path + filepath.trimmed();
        if(!QFile::exists(tmp_filepath)){
            tmp_filepath = abs_path + filepath;
            if(!QFile::exists(tmp_filepath)) {
                qDebug() << tmp_filepath << " does not exist";
                return "";
            }
            else return QDir(tmp_filepath).absolutePath();
        }

        else return QDir(tmp_filepath).absolutePath();
    }

    else {
        tmp_filepath = filepath.trimmed();
        if(!QFile::exists(tmp_filepath)){
            tmp_filepath = filepath;
            if(!QFile::exists(tmp_filepath)) {
                qDebug() << tmp_filepath << " does not exist (2)";
                return "";
            }
            else return QDir(tmp_filepath).absolutePath();
        }

        else return QDir(tmp_filepath).absolutePath();
    }

    return QDir(filepath).absolutePath();
}


int parse_m3u(QString file_content, MetaDataList& v_md, QString abs_path){
	QStringList list = file_content.split('\n');

    MetaData ext_md;
	foreach(QString line, list){
        qDebug() << "line = " << line;

		// remove comments
		int comment_idx=line.indexOf('#');
		if(comment_idx >= 0)
			line = line.mid(comment_idx, line.size() - comment_idx);

        if(line.trimmed().size() <= 0) continue;

        if(line.toUpper().startsWith("#EXTINF:")){
            int first_comma = line.indexOf(",");
            int space_after = line.indexOf(" - ", first_comma);
            ext_md.artist = line.mid(first_comma + 1, space_after - first_comma);
            ext_md.title = line.right(line.size() - (space_after + 3));
            continue;
        }

        MetaData md;
       /* if(ext_md.artist.size() > 0 || ext_md.title.size() > 0) md = ext_md;*/

        if( !Helper::is_www(line)){
            md.filepath = _correct_filepath(line, abs_path);

            MetaData md_tmp = db->getTrackByPath(md.filepath);

            if( md_tmp.id >= 0) v_md.push_back(md_tmp);
            else if( md.filepath.size() > 0 && ID3::getMetaDataOfFile(md) ){
				v_md.push_back(md);
			}
		}

		else {

            if(md.artist.size() == 0)
                md.artist = line;

            md.filepath = _correct_filepath(line, abs_path);

			md.album = "";
            v_md.push_back(md);
		}

        ext_md.artist = "";
        ext_md.title = "";
	}


	return v_md.size();
}


int parse_asx(QString file_content, MetaDataList& v_md, QString abs_path){


	v_md.clear();
	QDomDocument doc("AsxFile");
	doc.setContent(file_content.toLower());

	QDomElement docElement = doc.documentElement();
	QDomNode entry = docElement.firstChildElement("entry");

	if(!entry.hasChildNodes()) return 0;

	do{

		MetaData md;
		md.album = "";

		for(int entry_child=0; entry_child<entry.childNodes().size(); entry_child++)
		{
			md.bitrate = 128000;

			QDomNode content = entry.childNodes().at(entry_child);
			QString nodename = content.nodeName().toLower();
			QDomElement e = content.toElement();

			if(!nodename.compare("ref")){
				QString path = e.attribute("href");

				// filepath, convert to absolute path if relative
                md.filepath = _correct_filepath(path, abs_path);
				md.artist = path.trimmed();
			}

			else if(!nodename.compare("title")){
				md.title = e.text();
			}

            else if(!nodename.compare("album")){
                md.album = e.text();
            }

			else if(!nodename.compare("author")){
				md.artist = e.text();
			}
		}

		v_md.push_back(md);
		entry = entry.nextSibling();
	} while (!entry.isNull());

	return v_md.size();
}


int parse_pls(QString file_content, MetaDataList& v_md, QString abs_path){

	// abs_path = "", if file is not local

	v_md.clear();

	QStringList lines = file_content.split("\n");

	int n_titles = 0;

	// get number of titles
	int idx = file_content.toLower().indexOf("numberofentries=") +
			QString("numberofentries=").size();

	if(idx < 0) return 0;

	QString n_titles_str = "";
	while( file_content.at(idx).isDigit() ){
		n_titles_str += file_content.at(idx++);
	}


	n_titles = n_titles_str.toInt();
	if(n_titles == 0) return 0;
	MetaData md;
	md.album = "";
	for(int i=0; i<n_titles; i++) {
		v_md.push_back(md);
	}

	foreach(QString line, lines){

		if(line.trimmed().size() == 0 ||
			line.trimmed().startsWith("#")) continue;

		// remove comments
		int comment_idx=line.indexOf('#');
		if(comment_idx >= 0){
			line = line.mid(comment_idx, line.size() - comment_idx);
		}

		// split key, value
		QString key, val, tmp_key;
		QStringList line_splitted = line.split('=');
		if( line_splitted.size() <= 1 ) continue;

		tmp_key = line_splitted[0].trimmed();
        val = line_splitted[1];


		int track_idx = -1;
		int f_track_idx = 0;
		QString track_idx_str = "";

		for(int i=0; i<tmp_key.size(); i++){
			QChar c = tmp_key[i];
			if(c.isDigit()) {
				if(f_track_idx == 0) f_track_idx = i;
				track_idx_str += c;
			}
		}

		track_idx = track_idx_str.toInt();
		if( track_idx <= 0 ||
			track_idx_str.size() == 0 ||
			(uint)(track_idx - 1 ) >= v_md.size() ) continue;

		key = tmp_key.left(f_track_idx);

		if(key.toLower().startsWith("file")){
			v_md[track_idx - 1].artist = val;

			// calc absolute filepath
            v_md[track_idx - 1].filepath = _correct_filepath(val, abs_path);
		}

        else if(line.toLower().startsWith("title")){
			v_md[track_idx - 1].title = val;
		}

		else if(line.toLower().startsWith("length")){
			v_md[track_idx - 1].length_ms = val.toInt() * 1000;
		}
	}

	return v_md.size();

}


int PlaylistParser::parse_playlist(QString playlist_file, MetaDataList& v_md){

	// is only changed, if container file is local
	QString abs_path = "";

	MetaDataList v_md_tmp;
	MetaDataList v_md_to_delete;

	bool success = false;
	bool is_local_file = true;

	QString content;
    if(Helper::is_www(playlist_file)){
        success = WebAccess::read_http_into_str(playlist_file, &content);
		is_local_file = false;
	}

	else
        success = Helper::read_file_into_str(playlist_file, &content);

	if(!success) return 0;


	if(is_local_file){
		int last_slash = playlist_file.lastIndexOf(QDir::separator());
		abs_path = playlist_file.left(last_slash+1);
	}

	if(playlist_file.toLower().endsWith("m3u")){

		parse_m3u(content, v_md_tmp, abs_path);
	}

	else if(playlist_file.toLower().endsWith("ram")){
		parse_m3u(content, v_md_tmp, abs_path);
	}

	else if(playlist_file.toLower().endsWith("pls")){

		parse_pls(content, v_md_tmp, abs_path);
	}

	else if(playlist_file.toLower().endsWith("asx")){
		parse_asx(content, v_md_tmp, abs_path);
	}

	for(uint i=0; i<v_md_tmp.size(); i++){

		MetaData md = v_md_tmp[i];

		if( Helper::checkTrack(md) )
			v_md.push_back(md);

		else
			v_md_to_delete.push_back(md);

	}

	CDatabaseConnector::getInstance()->deleteTracks(v_md_to_delete);

	return v_md.size();
}


void PlaylistParser::save_playlist(QString filename, const MetaDataList& v_md, bool relative){

    if(!filename.endsWith("m3u", Qt::CaseInsensitive)) filename.append(".m3u");

    QString dir_str = filename.left(filename.lastIndexOf(QDir::separator()));
    QDir dir(dir_str);
    dir.cd(dir_str);

    FILE* file = fopen(filename.toStdString().c_str(), "w");
    if(!file) return;
    fputs("#EXTM3U\n", file);
    qint64 lines = 0;
    foreach(MetaData md, v_md){

        QString str;
        if(relative){
            str = dir.relativeFilePath(md.filepath);
        }

        else{
            str = md.filepath;
        }

        QString ext_data = "#EXTINF: " + QString::number(md.length_ms / 1000)  + ", " + md.artist + " - " + md.title + "\n";
        lines += fputs(ext_data.toLocal8Bit().data(), file);
        lines += fputs(str.toLocal8Bit().data(), file);
        lines += fputs("\n", file);
    }

    fclose(file);
}




