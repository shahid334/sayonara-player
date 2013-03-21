/* id3access.cpp */

/* Copyright (C) 2013  Lucio Carreras
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



#include "HelperStructs/Tagging/id3access.h"
#include "HelperStructs/Tagging/ID3_Fileheader.h"

#include <QByteArray>
#include <QStringList>
#include <QDebug>

#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include "taglib/id3v2tag.h"
#include "taglib/id3v2frame.h"
#include "taglib/id3v2header.h"
#include "taglib/mpegfile.h"
#include "taglib/mpegheader.h"
#include "taglib/tmap.h"



QString id3_extract_artist(ID3_FileHeader& fh){
 QByteArray raw_data;
 QByteArray four("TPE1", 4);
 QString str = fh.read(four, raw_data);
 if(str.size() > 0) return str;
 four = QByteArray("TPE2", 4);
 str = fh.read(four, raw_data);
 return str;

}

QString id3_extract_album(ID3_FileHeader& fh){
 QByteArray raw_data;
 QByteArray four("TALB", 4);
 return fh.read(four, raw_data);

}

QString id3_extract_title(ID3_FileHeader& fh){
 QByteArray raw_data;
 QByteArray four("TIT2", 4);
 return fh.read(four, raw_data);

}

int id3_extract_tracknumber(ID3_FileHeader& fh){
  QByteArray raw_data;
  QByteArray four("TRCK", 4);
  bool b = false;
  int i = fh.read(four, raw_data).toInt(&b);
  if(!b) return 0;
  return i;
}

int id3_extract_year(ID3_FileHeader& fh){
  QByteArray raw_data;
  QByteArray four("TDRC", 4);
  bool b = false;
  int i = fh.read(four, raw_data).toInt(&b);
  if(b) return i;

  four = QByteArray("TYER", 4);
  i = fh.read(four, raw_data).toInt(&b);
  if(b) return i;
 
  four = QByteArray("TORY", 4);
  i = fh.read(four, raw_data).toInt(&b);
  if(!b) return 0;
 
  return i;
}

QString id3_extract_genres(ID3_FileHeader& fh){
 QByteArray raw_data;
 QByteArray four("TCON", 4);
 return fh.read(four, raw_data);
}

QString id3_extract_comment(ID3_FileHeader& fh){
 QByteArray raw_data;
 QByteArray four("COMM", 4);
 return fh.read(four, raw_data);
}


bool id3_extract_discnumber(ID3_FileHeader& fh, int* discnumber, int* n_discs){

    *discnumber = 0;
    *n_discs = 0;

	QByteArray four("TPOS", 4);
	QByteArray raw_data;

	QString text = fh.read(four, raw_data);

    if(!text.size()) return false;

	qDebug() << "TPOS" << text;

    return true;
}


bool id3_write_discnumber(ID3_FileHeader& fh, int discnumber, int n_discs){
   
   if(discnumber <= 0) return false;
   if(n_discs == -1) n_discs = 1;

   char c_new_frame[] = { 0x03, discnumber + '0', '/', n_discs + '0', 0x00 };

   QByteArray arr(c_new_frame, 5);
   fh.fh_set_frame_content("TPOS", arr);
   fh.commit();

   return true;
}








bool taglib_id3_extract_discnumber(TagLib::FileRef& fh, int* discnumber){

    TagLib::File* f = fh.file();
    TagLib::MPEG::File* f_mp3;

    f_mp3 = dynamic_cast<TagLib::MPEG::File*>(f);

    if(!f_mp3) {
        qDebug() << "Tagging: no mp3 header";
        return false;
    }

    TagLib::ID3v2::Tag* id3_tag = f_mp3->ID3v2Tag();
    if(!id3_tag){
        qDebug() << "Tagging: no valid id3 tag";
        return false;
    }

    QByteArray vec;
    TagLib::ID3v2::FrameListMap map = id3_tag->frameListMap();
     TagLib::ID3v2::FrameList l = map["TPOS"];
     if(!l.isEmpty()){
         qDebug() << "Tagging: list is not empty";
         vec = QByteArray(l.front()->toString().toCString(false));
     }

     else {
         for(TagLib::ID3v2::FrameListMap::ConstIterator it=map.begin(); it!=map.end(); it++){
             qDebug() << QString(it->first.data());
         }
         qDebug() << "Tagging: list is empty";
         return false;
     }

    bool slash_found = false;

    for(int i=0; i<vec.size(); i++){

        char c = vec[i];
        qDebug() << (unsigned int) c;

        if(c == '/') {
            slash_found = true;
            continue;
        }

        // Ascii: '/'=47, '0'=48, '1'... '9'=57
        if(c >= '0' && c <= '9'){

            if(!slash_found)
                *discnumber = 10*(*discnumber) + (c - '0');
        }
    }

    return true;

}
