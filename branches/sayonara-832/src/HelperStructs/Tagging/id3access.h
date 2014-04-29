/* id3access.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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




#ifndef ID3_ACCESS_H_
#define ID3_ACCESS_H_

#include "HelperStructs/Tagging/ID3_Fileheader.h"
#include "HelperStructs/MetaData.h"

#include <QFile>

#include <QMap>
#include <QByteArray>
#include <QFile>
#include <QList>
#include <QMap>
#include <QDebug>


#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include "taglib/id3v2tag.h"
#include "taglib/mpegfile.h"
#include "taglib/mpegheader.h"



typedef unsigned char uchar;
typedef unsigned int uint;

#define DISCNUMBER "TPOS"
/*
if(!valid){

    char c[] = {'I', 'D', '3', 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    raw_data = QByteArray(c, 10);
    valid = true;
}

*/





void stretch_file(QFile* f, long offset, int n_bytes);


struct FileHeader {

private:

    bool    valid;
    qint64  header_size;
    qint64  org_size;
    QFile*  f;


    QByteArray raw_data;
    QList<QByteArray> things_to_write;
    QMap<QByteArray, QByteArray> all_frames;
    QMap<QString, int>        all_frames_indexes;


private:

    void        fh_update_size();
    bool        fh_open_and_read_file(QString filename);
    qint64      fh_read_header_size(const QByteArray& ten);



public:

    FileHeader(QString filename);


    bool      is_valid();


    QByteArray  fh_get_frame_content(const QByteArray& vec);


    QByteArray fh_calc_frame_content_size_int_to_byte(uint new_size);
    int        fh_calc_frame_content_size_byte_to_int(const QByteArray& ten);
    void       fh_set_frame_content(const QByteArray& four, const QByteArray& data_wo_header);
    QByteArray fh_extract_frame_content(const QByteArray& data_w_header);


    QByteArray read(const QByteArray& four);

    bool commit();
};

bool id3_write_discnumber(ID3_FileHeader& fh, int discnumber, int n_discs=1);
bool id3_extract_discnumber(ID3_FileHeader& fh, int* discnumber, int* n_discs);
QString id3_extract_artist(ID3_FileHeader& fh);
QString id3_extract_album(ID3_FileHeader& fh);
QString id3_extract_title(ID3_FileHeader& fh);
int id3_extract_tracknumber(ID3_FileHeader& fh);
int id3_extract_year(ID3_FileHeader& fh);
QString id3_extract_genres(ID3_FileHeader& fh);
QString id3_extract_comment(ID3_FileHeader& fh);
bool id3_extract_metadata(ID3_FileHeader& fh, MetaData& md);



bool taglib_id3_extract_discnumber(TagLib::FileRef& fh, int* discnumber);


#endif
