/*
 * PlaylistParser.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: luke
 */

#include "HelperStructs/PlaylistParser.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/id3.h"
#include <QStringList>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <qdom.h>


#include <vector>

using namespace std;


static int parse_m3u(QString file_content, vector<MetaData>& v_md);
static int parse_asx(QString file_content, vector<MetaData>& v_md);
static int parse_pls(QString file_content, vector<MetaData>& v_md);


int parse_m3u(QString file_content, vector<MetaData>& v_md){
	QStringList list = file_content.split('\n');

	foreach(QString line, list){
		line = line.trimmed();
		// remove comments
		int comment_idx=line.indexOf('#');
		if(comment_idx >= 0)
			line = line.mid(comment_idx, line.size() - comment_idx);

		if(line.size() <= 0 ||
			line.startsWith("#")) continue;

		MetaData md;
		if( !line.startsWith("http")){
			md = ID3::getMetaDataOfFile(line);
		}

		else {
			md.is_extern = true;
			md.artist = line;
			md.filepath = line;
			md.album = "";
		}

		v_md.push_back(md);
	}

	qDebug() << "extracted " << v_md.size() << " tracks";
	return v_md.size();
}


int parse_asx(QString file_content, vector<MetaData>& v_md){

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
			md.is_extern = true;
			md.bitrate = 128000;
			QDomNode content = entry.childNodes().at(entry_child);
			QString nodename = content.nodeName().toLower();
			qDebug() << "Nodename = " << nodename;
			QDomElement e = content.toElement();
			if(!nodename.compare("ref")){
				QString path = e.attribute("href");
				md.filepath = path.trimmed();
				md.artist = path.trimmed();
			}

			else if(!nodename.compare("title")){
				md.title = e.text();
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


int parse_pls(QString file_content, vector<MetaData>& v_md){

	v_md.clear();

	QStringList lines = file_content.split("\n");

	int n_titles = 0;

	// get number of titles
	int idx = file_content.toLower().indexOf("numberofentries=") + 16;
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
		val = line_splitted[1].trimmed();


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
			(track_idx -1 ) >= v_md.size() ) continue;

		key = tmp_key.left(f_track_idx);

		if(key.toLower().startsWith("file")){
			v_md.at(track_idx - 1).artist = val;
			v_md.at(track_idx - 1).filepath = val;
		}

		else if(line.toLower().startsWith("title")){
			v_md.at(track_idx - 1).title = val;
		}

		else if(line.toLower().startsWith("length")){
			v_md.at(track_idx - 1).length_ms = val.toInt() * 1000;
		}
	}

	return v_md.size();

}


int PlaylistParser::parse_playlist(QString playlist_file, vector<MetaData>& v_md){

	vector<MetaData> v_md_tmp;
	bool success = false;
	QString content;
	if(playlist_file.startsWith("http"))
		success = Helper::read_http_into_str(playlist_file, content);
	else
		success = Helper::read_file_into_str(playlist_file, content);

	if(!success) return 0;

	qDebug() << "Got content: ";
	qDebug() << content;

	if(playlist_file.toLower().endsWith("m3u")){
		parse_m3u(content, v_md_tmp);
	}

	else if(playlist_file.toLower().endsWith("ram")){
		parse_m3u(content, v_md_tmp);
	}

	else if(playlist_file.toLower().endsWith("pls")){
		parse_pls(content, v_md_tmp);
	}

	else if(playlist_file.toLower().endsWith("asx")){
		parse_asx(content, v_md_tmp);
	}

	for(uint i=0; i<v_md_tmp.size(); i++){

		if(Helper::checkTrack(v_md_tmp[i])){
			MetaData md = v_md_tmp[i];
			v_md.push_back(md);
		}

	}

	return v_md.size();
}



bool  PlaylistParser::is_supported_playlist(QString filename){
	return (filename.toLower().endsWith("m3u") ||
			filename.toLower().endsWith("pls") ||
			filename.toLower().endsWith("ram") ||
			filename.toLower().endsWith("asx") );
}



