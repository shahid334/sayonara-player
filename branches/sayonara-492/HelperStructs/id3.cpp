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


#include "HelperStructs/MetaData.h"
#include "HelperStructs/id3.h"
#include "HelperStructs/Helper.h"

#include <QString>
#include <QObject>
#include <QDebug>
#include <QDir>
#include <QFile>

#include <string>
	#include <taglib/tag.h>
	#include <taglib/taglib.h>
	#include <taglib/fileref.h>

using namespace std;
using namespace Helper;

TagLib::ByteVector get_tag_end(TagLib::FileRef& f){

    TagLib::ByteVector vec_len;
    int taglen = 0;
    int multiplier = 1;
    f.file()->seek(6);

    vec_len = f.file()->readBlock(4);
    for(int i=3; i>=0; i--){
            taglen += ((uchar)vec_len[i] * multiplier);
            multiplier *= 128;
    }

    // max 256 Kbyte
    if(taglen > 262143) taglen = 262143;

    f.file()->seek(taglen +10);
    TagLib::ByteVector end_of_tag = f.file()->readBlock(8);

    return end_of_tag;
}


bool find_discnumber(TagLib::ByteVector vec, int* discnumber, int* n_discs){
    *discnumber = -1;
    *n_discs = -1;
    if(vec.size() == 0) return "";
    QString ret;

    for(uint i=10; i<vec.size(); i++){
        char c = vec.at(i);
        if(c >= 47 && c <= 57)
            ret += c;
        if( (c >= 65 && c <= 90) ||
            (c >=97 && c <= 122) ) break;
    }


    while(ret.startsWith("/")){
        ret.remove(0, 1);
    }

    while(ret.endsWith("/")){
        ret.remove(ret.size() - 1, 1);
    }

    QStringList lst = ret.split("/");
    if(lst.size() > 1) {
        *discnumber = lst[0].toInt();
        *n_discs = lst[1].toInt();
    }

    else if(lst.size() == 1){
        *discnumber = lst[0].toInt();
    }

    return !ret.isEmpty();
}

TagLib::ByteVector find_attr(TagLib::FileRef& f, const char* attr){


    TagLib::ByteVector v(attr);
    TagLib::ByteVector result;

    TagLib::ByteVector tag_end = get_tag_end(f);

    long offset = f.file()->find(v, 0, tag_end);

    if(offset < 0) return result;

    f.file()->seek(offset);

    result = f.file()->readBlock(32);
    return result;
}


bool ID3::getMetaDataOfFile(MetaData& md){

    md.filepath = QDir(md.filepath).absolutePath();
    QFile qf(md.filepath);
    md.filesize = qf.size();
    qf.close();

    TagLib::FileRef f(TagLib::FileName(md.filepath.toUtf8()));


    int idx = md.filepath.lastIndexOf('/');
    md.title = md.filepath.right(md.filepath.length() - idx -1);
	md.title = md.title.left(md.title.length() - 4);

    if(f.isNull() || !f.tag() || !f.file()->isValid() || !f.file()->isReadable(md.filepath.toUtf8()) ) return false;
	string artist = f.tag()->artist().to8Bit(true);
	string album = f.tag()->album().to8Bit(true);
	string title = f.tag()->title().to8Bit(true);
	string genre = f.tag()->genre().to8Bit(true);
    string comment = f.tag()->comment().to8Bit(true);


    int discnumber = -1;
    int n_discs = -1;
    TagLib::ByteVector vec = find_attr(f, "TPOS");
    find_discnumber(vec, &discnumber, &n_discs);

	uint year = f.tag()->year();
	uint track = f.tag()->track();
	int bitrate = f.audioProperties()->bitrate() * 1000;

	int length = f.audioProperties()->length();

    QStringList genres;
    QString genre_str = cvtQString2FirstUpper(QString::fromLocal8Bit(genre.c_str()));
    genres = genre_str.split(QRegExp(",|/|;|\\."));
    for(int i=0; i<genres.size(); i++){
        genres[i] = genres[i].trimmed();
    }

    //md.album = cvtQString2FirstUpper(QString::fromLocal8Bit(album.c_str()));
    md.album = cvtQString2FirstUpper(QString::fromLocal8Bit(album.c_str()));
	md.artist = cvtQString2FirstUpper(QString::fromLocal8Bit(artist.c_str()));
	md.title = cvtQString2FirstUpper(QString::fromLocal8Bit(title.c_str()));
    md.length_ms = length * 1000;
	md.year = year;
	md.track_num = track;
	md.bitrate = bitrate;
    md.genres = genres;
    md.discnumber = discnumber;
    md.n_discs = n_discs;
    md.comment = cvtQString2FirstUpper(QString::fromLocal8Bit(comment.c_str()));


	if(md.title.length() == 0){
        idx = md.filepath.lastIndexOf('/');
        md.title = md.filepath.right(md.filepath.length() - idx -1);
		md.title = md.title.left(md.title.length() - 4);
	}
	return true;
}


void ID3::setMetaDataOfFile(MetaData& md){

	md.filepath = QDir(md.filepath).absolutePath();
	TagLib::FileRef f(TagLib::FileName(md.filepath.toUtf8()));
    if(f.isNull() || !f.tag() || !f.file()->isValid() || !f.file()->isWritable(md.filepath.toUtf8()) ){
        qDebug() << "ID3 cannot save";
		return;
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
	return;
}
