#include "HelperStructs/Tagging/id3access.h"

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


char* taglist[] = {
    "AENC", "APIC", "COMM", "COMR", "ENCR", "EQUA", "ETCO", "GEOB", "GRID", "IPLS", "LINK", "MCDI",
    "MLLT", "OWNE", "PRIV", "PCNT", "POPM", "POSS", "RBUF", "RVAD", "RVRB", "SYLT", "SYTC", "TALB",
    "TBPM", "TCOM", "TCON", "TCOP", "TDAT", "TDLY", "TENC", "TEXT", "TFLT", "TIME", "TIT1", "TIT2",
    "TIT3", "TKEY", "TLAN", "TLEN", "TMED", "TOAL", "TOFN", "TOLY", "TOPE", "TORY", "TOWN", "TPE1",
    "TPE2", "TPE3", "TPE4", "TPOS", "TPUB", "TRCK", "TRDA", "TRSN", "TRSO", "TSIZ", "TSRC", "TSSE",
    "TYER", "TXXX", "UFID", "USER", "USLT", "WCOM", "WCOP", "WOAF", "WOAR", "WOAS", "WORS", "WPAY",
    "WPUB", "WXXX", "TDRC", NULL
};

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



bool id3_write_discnumber(FileHeader& fh, int discnumber, int n_discs){
   
   if(discnumber <= 0) return false;
   if(n_discs == -1) n_discs = 1;


   char c_new_frame[] = { 0x03, discnumber + '0', '/', n_discs + '0', 0x00 };

   QByteArray arr(c_new_frame, 5);
   fh.fh_set_frame_content("TPOS", arr);
   fh.commit();

   return true;
}


bool id3_extract_discnumber(FileHeader& fh, int* discnumber, int* n_discs){

    *discnumber = 0;
    *n_discs = 0;

    QByteArray vec = fh.read("TPOS");
    int frame_size = vec.size();

    if(frame_size == 0) return false;

    bool slash_found = false;
    for(int i=10; i<frame_size; i++){

        char c = vec[i];

        if(c == '/') {
            slash_found = true;
            continue;
        }

        // Ascii: '/'=47, '0'=48, '1'... '9'=57
        if(c >= '0' && c <= '9'){


            if(!slash_found)
                *discnumber = 10*(*discnumber) + (c - '0');

            else
                *n_discs = 10*(*n_discs) + (c - '0');
        }
    }

    return true;
}



void stretch_file(QFile* f, long offset, int n_bytes){
    if(n_bytes <= 0 ) return;

    const int buffersize = 16384;

    f->seek(0);

    qint64 filesize = f->size();
    f->seek(filesize-buffersize);

    qint64 i=filesize - buffersize;

    while(i>offset){
           QByteArray arr = f->read(buffersize);

           f->seek( f->pos() - buffersize + n_bytes);
           f->write(arr);

           i-=buffersize;
           if(i <= offset) break;
           f->seek(f->pos() - (buffersize + n_bytes + buffersize));
    }

    i += buffersize;
    int rest = (i-offset);

    f->seek(offset);

    QByteArray restbuffer = f->read(rest);
    f->seek(f->pos() - rest + n_bytes);
    f->write(restbuffer);
}





FileHeader::FileHeader(QString filename){

    header_size = 0;
    org_size = 0;

    valid = fh_open_and_read_file(filename);
}

bool FileHeader::is_valid(){
    return valid;
}







void FileHeader::fh_update_size(){

    // header size $(-X XX XX XX ) * 4

    int new_size = raw_data.size();


    char c_new_size[4];

    c_new_size[0] = new_size >> 21; c_new_size[0] &= 0x7F;
    c_new_size[1] = new_size >> 14; c_new_size[1] &= 0x7F;
    c_new_size[2] = new_size >> 7; c_new_size[2] &= 0x7F;
    c_new_size[3] = new_size; c_new_size[3] &= 0x7F;

    #if QT_VERSION >= QT_VERSION_CHECK(4,7,0)
        raw_data.replace(6, 4, c_new_size, 4);
    #else
    	QByteArray ba(c_new_size, 4);    
	raw_data.replace(6, 4, ba);
    #endif

    header_size = new_size;
}


qint64 FileHeader::fh_read_header_size(const QByteArray& ten){

    qint64 headersize = 0;
    int multiplier = 1;

    for(int i=9; i>=6; i--){
            headersize += (ten[i] * multiplier);
            multiplier *= 128;
    }

    return headersize;
}


// input: entire vector, with header, size, flags, content
// ountput: only size str
QByteArray FileHeader::fh_calc_frame_content_size_int_to_byte(uint new_size){

    char c_new_size[4];

    c_new_size[0] = new_size >> 21; c_new_size[0] &= 0x7F;
    c_new_size[1] = new_size >> 14; c_new_size[1] &= 0x7F;
    c_new_size[2] = new_size >> 7; c_new_size[2] &= 0x7F;
    c_new_size[3] = new_size; c_new_size[3] &= 0x7F;

    return QByteArray(c_new_size, 4);
}


// input: entire vector, with header, size, flags, content
// output: of size of content
int FileHeader::fh_calc_frame_content_size_byte_to_int(const QByteArray& ten){

    if(ten.size() < 10) return 0;

    int size = 0;
    int multiplier = 1;
    for(int i=7; i>=4; i--){

            size += (ten[i] * multiplier);
            multiplier *= 128;
    }

    return size;
}


// input: entire vector, with header, size, flags, content
// output: same, but modified
void FileHeader::fh_set_frame_content(const QByteArray& four, const QByteArray& data_wo_header){

    QByteArray result = four;
    QByteArray sz = fh_calc_frame_content_size_int_to_byte(data_wo_header.size());
    result.push_back(sz);
    result.push_back((char)0x00);
    result.push_back((char)0x00);
    result.append(data_wo_header);

    all_frames[four] = result;
}


// input: entire vector, with header, size, flags, content
// output: only content
QByteArray FileHeader::fh_extract_frame_content(const QByteArray& data_w_header){

    QByteArray result;
    int len_frame = fh_calc_frame_content_size_byte_to_int(data_w_header.left(10)) + 10;

    if(len_frame > data_w_header.size()) return result;

    for(int i=10; i<len_frame; i++)
        result.push_back(data_w_header[i]);

    return result;
}





bool FileHeader::fh_open_and_read_file(QString filename){
    f = new QFile(filename);
    qDebug() << "Read file";
    if(!f->open(QIODevice::ReadOnly)){
        valid = false;
        return false;
    }

    f->seek(0);
    QByteArray header_header = f->read(10);
    valid = header_header.startsWith("ID3");

    if(!valid) {
        f->close();
        return false;

    }



    header_size = fh_read_header_size(header_header);
    qDebug() << "Header size = " << header_size;
    org_size = header_size;
    f->seek(0);
    raw_data = f->read(10 + header_size);



    for(qint64 i=10; i<raw_data.size(); i++){

        QByteArray four = raw_data.mid(i, 4);

        int t=0;
        while(taglist[t]){


            // tag found
            if( four == QByteArray(taglist[t], 4) ){

                QByteArray arr(raw_data.mid(i, 10));
                int framesize = fh_calc_frame_content_size_byte_to_int(arr);
                //qDebug() << "Found tag = " << four << ", " << framesize;
                /*for(int i=0; i<arr.size(); i++){
                    qDebug() << (uint) arr.at(i);
                }*/


                all_frames.insert(four, raw_data.mid(i, 10+framesize));

                i += 10 + framesize - 1;
                break;
            }

            // tag not found
            t++;
        }
    }

    QList<QByteArray> keys = all_frames.keys();
    foreach(QByteArray key, keys){
        //qDebug() << "have " << key;
    }

    f->close();
    return true;
}



QByteArray FileHeader::read(const QByteArray& four){

    return all_frames.value(four);
}




bool FileHeader::commit(){

    int increase_size = 0;
    int raw_data_size = raw_data.size();
    //qDebug() << "old array size = " << raw_data_size;
    raw_data.clear();

    char c[] = {'I', 'D', '3', 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    raw_data = QByteArray(c, 10);

    QList<QByteArray> keys = all_frames.keys();
    foreach(QByteArray key, keys){
        //qDebug() << "write " << key;
        raw_data.push_back(all_frames.value(key));
    }

   // qDebug() << "new array size " << raw_data.size();

    int arr_difference = raw_data_size - raw_data.size();

    //old array was bigger
    if(arr_difference >= 0){
        for(int i=0; i<arr_difference; i++){
            raw_data.push_back((char) 0x00);
        }
    }

    else {
        increase_size = -arr_difference;
    }

    this->fh_update_size();


    f->open(QFile::ReadWrite);
    if(!f->isOpen()) return false;
    stretch_file(f, org_size, increase_size);
    f->seek(0);
    f->write(raw_data);
    f->close();
    return true;
}

