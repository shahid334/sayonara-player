
#ifndef ID3_ACCESS_H_
#define ID3_ACCESS_H_

#include "HelperStructs/Tagging/ID3_Fileheader.h"

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



bool taglib_id3_extract_discnumber(TagLib::FileRef& fh, int* discnumber);


#endif
