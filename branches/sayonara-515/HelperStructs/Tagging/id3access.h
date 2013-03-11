
#ifndef ID3_ACCESS_H_
#define ID3_ACCESS_H_


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
    qint64  empty_space_begins;
    qint64  empty_space_len;
    QFile*  f;


    QByteArray raw_data;
    QList<QByteArray> things_to_write;


private:

    void        fh_update_size();

    qint64      fh_read_header_size(const QByteArray& header_10_bytes);

    QByteArray  fh_set_frame_content_size(const QByteArray& vec, uint new_size);

    QByteArray  fh_set_frame_content(const QByteArray& vec_org, const QByteArray& data);

    int         fh_get_mp3_attr(QByteArray tag, QByteArray& result);

    bool        fh_open_and_read_file(QString filename);


public:

    FileHeader(QString filename);


    bool      is_valid();


    QByteArray  fh_get_frame_content(const QByteArray& vec);

    int         fh_get_frame_content_size(const QByteArray& vec);

    QByteArray read(const QByteArray& what);


    void write(const QByteArray& arr);

    bool commit();
};

bool id3_write_discnumber(FileHeader& fh, int discnumber, int n_discs=1);
bool id3_extract_discnumber(FileHeader& fh, int* discnumber, int* n_discs);

bool taglib_id3_extract_discnumber(TagLib::FileRef& fh, int* discnumber);


#endif
