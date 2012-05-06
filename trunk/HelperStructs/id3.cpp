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

#include <string>
	#include <taglib/tag.h>
	#include <taglib/taglib.h>
	#include <taglib/fileref.h>

using namespace Helper;

MetaData ID3::getMetaDataOfFile(QString file){
	MetaData md;
	TagLib::FileRef f(TagLib::FileName(file.toUtf8()));

		md.filepath = file;

		int idx = md.filepath.lastIndexOf('/');
		md.title = md.filepath.right(md.filepath.length() - idx -1);
		md.title = md.title.left(md.title.length() - 4);

		if(f.isNull() || !f.tag() || f.tag()->isEmpty() ) return md;
		string artist = f.tag()->artist().to8Bit(true);
		string album = f.tag()->album().to8Bit(true);
		string title = f.tag()->title().to8Bit(true);
		uint year = f.tag()->year();
		uint track = f.tag()->track();
		int bitrate = f.audioProperties()->bitrate() * 1000;



		int length = f.audioProperties()->length();


		md.album = cvtQString2FirstUpper(QString::fromLocal8Bit(album.c_str()));
		md.artist = cvtQString2FirstUpper(QString::fromLocal8Bit(artist.c_str()));
		md.title = cvtQString2FirstUpper(QString::fromLocal8Bit(title.c_str()));
		md.filepath = file;
		md.length_ms = length * 1000;
		md.year = year;
		md.track_num = track;
		md.bitrate = bitrate;


		if(md.title.length() == 0){
			idx = md.filepath.lastIndexOf('/');
			md.title = md.filepath.right(md.filepath.length() - idx -1);
			md.title = md.title.left(md.title.length() - 4);
		}

		return md;

}




void ID3::getMetaDataOfFile(TagLib::FileRef& f, QString file, MetaData& md){



		md.filepath = file;
		int idx = md.filepath.lastIndexOf('/');
		md.title = md.filepath.right(md.filepath.length() - idx -1);
		md.title = md.title.left(md.title.length() - 4);

		if(f.isNull()) return;

		string artist = f.tag()->artist().to8Bit(true);
		string album = f.tag()->album().to8Bit(true);
		string title = f.tag()->title().to8Bit(true);
		uint year = f.tag()->year();
		uint track = f.tag()->track();
		int bitrate = f.audioProperties()->bitrate() * 1000;



		int length = f.audioProperties()->length();


		md.album = cvtQString2FirstUpper(QString::fromLocal8Bit(album.c_str()));
		md.artist = cvtQString2FirstUpper(QString::fromLocal8Bit(artist.c_str()));
		md.title = cvtQString2FirstUpper(QString::fromLocal8Bit(title.c_str()));
		md.filepath = file;
		md.length_ms = length * 1000;
		md.year = year;
		md.track_num = track;
		md.bitrate = bitrate;


		if(md.title.length() == 0){
			idx = md.filepath.lastIndexOf('/');
			md.title = md.filepath.right(md.filepath.length() - idx -1);
			md.title = md.title.left(md.title.length() - 4);
		}

}


void ID3::setMetaDataOfFile(MetaData& md){


	TagLib::FileRef f(TagLib::FileName(md.filepath.toUtf8()));
	if(f.isNull()) {
		qDebug() << Q_FUNC_INFO << " f is null!";
		return;
	}

	TagLib::String album(md.album.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String artist(md.artist.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String title(md.title.toUtf8().data(), TagLib::String::UTF8);

	f.tag()->setAlbum(album);
	f.tag()->setArtist(artist);
	f.tag()->setTitle(title);
	f.tag()->setYear(md.year);
	f.tag()->setTrack(md.track_num);
	f.save();
	return;
}
