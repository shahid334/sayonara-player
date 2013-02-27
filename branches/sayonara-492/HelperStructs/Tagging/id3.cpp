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
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Tagging/id3access.h"
#include "HelperStructs/Helper.h"

#include <QString>
#include <QObject>
#include <QDebug>
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

    fstream file;
    file.open(md.filepath.toLocal8Bit(), ios_base::in);
    id3_extract_discnumber(file, &discnumber, &n_discs);
    file.close();

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


    fstream file;
    file.open(md.filepath.toLocal8Bit(), ios_base::out | ios_base::in | ios_base::binary);
    id3_write_discnumber(file, md.discnumber, md.n_discs);
    file.close();

	return;
}
