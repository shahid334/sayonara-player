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
#include <fstream>
#include <vector>

#include <string>
	#include <taglib/tag.h>
	#include <taglib/taglib.h>
	#include <taglib/fileref.h>

typedef vector<uchar> ByteVector;

using namespace std;
using namespace Helper;

void stretch_file(fstream& f, long offset, int n_bytes);

long get_mp3_header_size(fstream& f){

    long headersize = 0;
    int multiplier = 1;

    char bytes[4];
    f.seekg(6, ios_base::beg);
    f.read(bytes, 4);
        cout << "headersize " << flush;
    for(int i=3; i>=0; i--){
            headersize += (bytes[i] * multiplier);
            cout << (int) bytes[i] << "." << flush;
            multiplier *= 128;
    }

    f.seekg(0, ios_base::beg);
    cout << endl;

    return headersize;
}


void set_mp3_header_size(fstream& f, int new_size){

    // header size (0x -X XX XX XX ) * 4
    char c_new_size[4];

    c_new_size[0] = new_size >> 21; c_new_size[0] &= 0x7F;
    c_new_size[1] = new_size >> 14; c_new_size[1] &= 0x7F;
    c_new_size[2] = new_size >> 7; c_new_size[2] &= 0x7F;
    c_new_size[3] = new_size; c_new_size[3] &= 0x7F;

    f.seekp(6, ios_base::beg);
    f.write(c_new_size, 4);
    f.flush();
    f.seekp(0, ios_base::beg);
}



// input: entire vector, with header, size, flags, content
// output: of size of content
int get_mp3_frame_content_size(const ByteVector& vec){

    if(vec.size() < 8) return 0;

    int size = 0;
    int multiplier = 1;
    for(int i=7; i>=4; i--){

            size += (vec[i] * multiplier);
            multiplier *= 256;
    }

    return size;
}

// input: entire vector, with header, size, flags, content
// ountput: same, but modified
ByteVector set_mp3_frame_content_size(const ByteVector& vec, uint new_size){

    if(vec.size() < 8) return vec;
    ByteVector result = vec;

    result[7] = new_size;
    result[6] = new_size >> 8;
    result[5] = new_size >> 16;
    result[4] = new_size >> 24;
    return result;
}


// input: entire vector, with header, size, flags, content
// output: content
ByteVector get_mp3_frame_content(const ByteVector& vec){

    ByteVector result;
    int len_content = get_mp3_frame_content_size(vec);
    if(len_content+10 > vec.size()) return result;

    for(int i=10; i<10+len_content; i++)
        result.push_back(vec[i]);

    return result;
}

// input: entire vector, with header, size, flags, content
// output: same, but modified
ByteVector set_mp3_frame_content(const ByteVector& vec_org, const ByteVector& data){

    ByteVector result = vec_org;
    result = set_mp3_frame_content_size(vec_org, data.size());

    while(result.size() > 10)
        result.pop_back();

    for(uint i=0; i<data.size(); i++){
        result.push_back(data[i]);
    }

    return result;
}


ByteVector get_mp3_header_end_str(fstream& f){

    ByteVector result;
    char end_tag[8];
    int taglen = get_mp3_header_size(f);

    // max 256 Kbyte
    if(taglen > 262143) taglen = 262143;

    f.seekg(taglen +10, ios_base::beg);
    f.read(end_tag, 8);
    for(int i=0; i<8; i++){
        result.push_back(end_tag[i]);
    }

    return result;
}

// input: file, the attribute searching for (e.g. TPOS), a reference to an output vector (EVERYTHING is included there)
// output: where did we finde the tag, negative if not available
long get_mp3_attr(fstream& f, char* tag, int tag_size, ByteVector& result){

    ByteVector tag_end = get_mp3_header_end_str(f);

    long offset = -1;
    const int block_size = 256;
    char block[block_size];

    bool found = false;
    int n_blocks = 0;
    f.seekg(0, ios_base::beg);

    while(true){

        f.read(block, block_size);

        uint found_cs = 0;
        int ptr_tag = 0;
        for(int i=0; i<block_size; i++, offset++){
                if(block[i] == tag[ptr_tag]){
                        found_cs++;
                        ptr_tag++;
                }

                else {
                        ptr_tag = 0;
                        found_cs = 0;
                }


            if(found_cs == tag_size) {
                found = true;
                offset -= (tag_size -2);
                break;
            }
        }

        n_blocks++;

        if(found || offset > 256000) break;
    }

    if(!found) return -1;

    char block_data[10];

    f.seekg(offset, ios_base::beg);
    f.read(block_data, 10);
    for(int i=0; i<10; i++){
        cout << (int) block_data[i] << "." << flush;
        result.push_back(block_data[i]);
    }


   int framesize = get_mp3_frame_content_size(result);
   char* entire_block = new char[10+framesize];

   f.seekg(offset, ios_base::beg);
   f.read(entire_block, 10 + framesize);

   result.clear();

   for(int i=0; i<10+framesize; i++){
       result.push_back(entire_block[i]);
   }

   f.seekg(0, ios_base::beg);
   f.seekp(0, ios_base::beg);

   return offset;
}


bool write_discnumber(fstream& f, int discnumber, int n_discs=1){
   long header_size = get_mp3_header_size(f);
   long new_header_size = header_size;
   if(discnumber <= 0) return false;
   n_discs = 1;

   char c_new_frame[] = {'T', 'P', 'O', 'S',
               0, 0, 0, 0x05,
               0, 0,
               0x03, discnumber + '0', '/', n_discs + '0', 0x00 };

   ByteVector bv_new_frame;
   ByteVector old_frame;
   long pos = get_mp3_attr(f, "TPOS", 4, bv_new_frame);

   int old_frame_size = 0;
   if (pos > 0){
      old_frame_size = get_mp3_frame_content_size(bv_new_frame) + 10;
   }

   else{
       old_frame_size = 0;
       pos = header_size;
   }

   new_header_size = header_size + (15 - old_frame_size);

   qDebug() << "old frame size = " << old_frame_size;
   qDebug() << "old header size = " << header_size;
   qDebug() << "new header size = " << new_header_size;

   set_mp3_header_size(f, new_header_size);



   stretch_file(f, pos, 15- old_frame_size);

   f.seekg(pos, ios_base::beg);
   qDebug() << "where are we? " << f.tellg();
   f.write(c_new_frame, 15);

   qDebug() << "insert at pos " << pos;

}


bool extract_discnumber(fstream& f, int* discnumber, int* n_discs){

    *discnumber = -1;
    *n_discs = -1;

    QString ret;

    ByteVector vec;
    long pos = get_mp3_attr(f, "TPOS", 4, vec);

    if(vec.size() == 0 || pos < 0) return false;


    const int frame_start = 10;
    int frame_size = get_mp3_frame_content_size(vec);


    for(uint i=0; i<frame_size; i++){
       char c = vec[i+frame_start];


       if(c >= 47 && c <= 57){
           ret += c;
       }
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

    return true;
}


void stretch_file(fstream& f, long offset, int n_bytes){
    if(n_bytes <= 0 ) return;

    const int buffersize = 50;

    char buffer[buffersize];
    f.seekg(0, ios_base::end);

    long filesize = f.tellg();
    f.seekg(-buffersize, ios_base::end);

    cout << "Filesize = " << filesize << endl;

    long i=filesize - buffersize;
    int round = 1;
    while(i>offset){
           f.read(buffer, buffersize);

           round ++;
           f.seekg(-buffersize + n_bytes,  ios_base::cur);
           f.write(buffer, buffersize);
           f.flush();

           i-=buffersize;
           if(i <= offset) break;
           f.seekg(-(buffersize + n_bytes + buffersize), ios_base::cur);

           round ++;

    }

    i += buffersize;
    int rest = (i-offset);
    char* restbuffer = new char[rest];

    f.seekg(offset, ios_base::beg);

    f.read(restbuffer, rest-1);
    f.seekg(-rest + n_bytes, ios_base::cur);
    f.write(restbuffer, rest);
    f.flush();

    delete restbuffer;
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

    fstream file;
    file.open(md.filepath.toLocal8Bit(), ios_base::in);
    extract_discnumber(file, &discnumber, &n_discs);
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

    qDebug() << "Write Discnumber";
    fstream file;
    file.open(md.filepath.toLocal8Bit(), ios_base::out | ios_base::in | ios_base::binary);
    write_discnumber(file, md.discnumber, md.n_discs);
    file.close();




	return;
}
