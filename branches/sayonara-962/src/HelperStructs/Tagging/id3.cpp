/* id3.cpp */

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


#include "HelperStructs/Tagging/ID3_Fileheader.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Tagging/id3access.h"
#include "HelperStructs/Helper.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QDir>
#include <QFile>
#include <fstream>
#include <vector>

#include <string>
#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/fileref.h>



using namespace std;
using namespace Helper;



bool ID3::getMetaDataOfFile(MetaData& md) {

	md.filepath = QDir(md.filepath).absolutePath();
    QFile qf(md.filepath);
    md.filesize = qf.size();
    qf.close();

	TagLib::FileRef f(TagLib::FileName(md.filepath.toUtf8()), true, TagLib::AudioProperties::Accurate);

	if(f.isNull() || !f.tag() || !f.file()->isValid() || !f.file()->isReadable(md.filepath.toUtf8()) ) {
		qDebug() << md.filepath << ": Something's wrong with this file";
		return false;
	}

	TagLib::Tag* tag = f.tag();

	string artist = tag->artist().to8Bit(true);
	string album = tag->album().to8Bit(true);
	string title = tag->title().to8Bit(true);
	string genre = tag->genre().to8Bit(true);
	string comment = tag->comment().to8Bit(true);

    int discnumber = 0;
    int n_discs = 0;

    taglib_id3_extract_discnumber(f, &discnumber);

	uint year = tag->year();
	uint track = tag->track();

	MetaData md_tmp = CDatabaseConnector::getInstance()->getTrackByPath(md.filepath);

	int bitrate = f.audioProperties()->bitrate() * 1000;
	int length = f.audioProperties()->length() * 1000;

	if( md_tmp.id >= 0 ){
		bitrate = md.bitrate;
		length = md.length_ms;
	}

    QStringList genres;
    QString genre_str = cvtQString2FirstUpper(QString::fromLocal8Bit(genre.c_str()));
    genres = genre_str.split(QRegExp(",|/|;|\\."));
    for(int i=0; i<genres.size(); i++) {
        genres[i] = genres[i].trimmed();
    }

    md.album = QString::fromLocal8Bit(album.c_str());
    md.artist = QString::fromLocal8Bit(artist.c_str());
    md.title = QString::fromLocal8Bit(title.c_str());
	md.length_ms = length;
	md.year = year;
	md.track_num = track;
	md.bitrate = bitrate;
    md.genres = genres;
    md.discnumber = discnumber;
    md.n_discs = n_discs;
    md.comment = QString::fromLocal8Bit(comment.c_str());

	if(md.title.length() == 0) {
		int idx = md.filepath.lastIndexOf('/');
		md.title = md.filepath.right(md.filepath.length() - idx -1);
		md.title = md.title.left(md.title.length() - 4);
	}

	return true;
}


bool ID3::setMetaDataOfFile(MetaData& md) {

	md.filepath = QDir(md.filepath).absolutePath();
    TagLib::FileRef f(TagLib::FileName(md.filepath.toUtf8()));
    if(f.isNull() || !f.tag() || !f.file()->isValid() || !f.file()->isWritable(md.filepath.toUtf8()) ) {
        qDebug() << "ID3 cannot save";
		return false;
	}

	TagLib::String album(md.album.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String artist(md.artist.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String title(md.title.toUtf8().data(), TagLib::String::UTF8);
    TagLib::String genre(md.genres.join(",").toUtf8().data(), TagLib::String::UTF8);

	f.tag()->setAlbum(album);
	f.tag()->setArtist(artist);
	f.tag()->setTitle(title);
	f.tag()->setGenre(genre);
	f.tag()->setYear(md.year);
	f.tag()->setTrack(md.track_num);
    f.save();

    TagLib::File* file = f.file();
    TagLib::MPEG::File* f_mp3;

    f_mp3 = dynamic_cast<TagLib::MPEG::File*>(file);

    if(!f_mp3) {
        qDebug() << "Tagging: no mp3 header";
		return true;
    }

    TagLib::ID3v2::Tag* id3_tag = f_mp3->ID3v2Tag();
    if(!id3_tag) {
        qDebug() << "Tagging: no valid id3 tag";
		return true;
    }

    ID3_FileHeader fh(md.filepath);

	if(fh.is_valid()){
		id3_write_discnumber(fh, md.discnumber, md.n_discs);
	}

	return true;
}



void ID3::checkForBrokenFiles(MetaDataList v_md, MetaDataList& v_md_broken) {
	return;
	int i=0;
	foreach(MetaData md, v_md) {
		ID3_FileHeader header(md.filepath);
		if(header.is_broken()) {
			qDebug() << md.title << "by (" << md.artist << ") on " << md.album << " is broken";
			v_md_broken.push_back(md);
			i++;
		}
	}

	qDebug() << "Found " << i << " broken files";
}
