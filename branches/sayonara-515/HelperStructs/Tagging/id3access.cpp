#include "HelperStructs/Tagging/id3access.h"

#include <QByteArray>
#include <QDebug>

#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include "taglib/id3v2tag.h"
#include "taglib/id3v2frame.h"
#include "taglib/id3v2header.h"
#include "taglib/mpegfile.h"
#include "taglib/mpegheader.h"


bool taglib_id3_extract_discnumber(TagLib::FileRef& fh, int* discnumber){

    TagLib::File* f = fh.file();
    TagLib::MPEG::File* f_mp3;

    f_mp3 = dynamic_cast<TagLib::MPEG::File*>(f);

    if(!f_mp3) {
        return false;
    }

    TagLib::ID3v2::Tag* id3_tag = f_mp3->ID3v2Tag();
    if(!id3_tag){
        return false;
    }

    QByteArray vec;
     TagLib::ID3v2::FrameList l = id3_tag->frameListMap()["TPOS"];
     if(!l.isEmpty()){
         vec = QByteArray(l.front()->toString().toCString(false));
     }

     else return false;

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


   char c_new_frame[] = {'T', 'P', 'O', 'S',
               0, 0, 0, 0x05,
               0, 0,
               0x03, discnumber + '0', '/', n_discs + '0', 0x00 };

   QByteArray arr(c_new_frame, 15);
   fh.write(arr);
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
    empty_space_begins = 0;
    empty_space_len = 0;

    valid = fh_open_and_read_file(filename);
}

bool FileHeader::is_valid(){
    return valid;
}



// input: entire vector, with header, size, flags, content
// output: content
QByteArray FileHeader::fh_get_frame_content(const QByteArray& vec){

    QByteArray result;
    int len_content = fh_get_frame_content_size(vec) + 10;
    if(len_content > vec.size()) return result;

    for(int i=10; i<len_content; i++)
        result.push_back(vec[i]);

    return result;
}


// input: entire vector, with header, size, flags, content
// output: of size of content
int FileHeader::fh_get_frame_content_size(const QByteArray& vec){

    if(vec.size() < 8) return 0;

    int size = 0;
    int multiplier = 1;
    for(int i=7; i>=4; i--){

            size += (vec[i] * multiplier);
            multiplier *= 256;
    }

    return size;
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


qint64 FileHeader::fh_read_header_size(const QByteArray& header_10_bytes){

    qint64 headersize = 0;
    int multiplier = 1;

    for(int i=9; i>=6; i--){
            headersize += (header_10_bytes[i] * multiplier);
            multiplier *= 128;
    }

    return headersize;
}


// input: entire vector, with header, size, flags, content
// ountput: same, but modified
QByteArray FileHeader::fh_set_frame_content_size(const QByteArray& vec, uint new_size){

    if(vec.size() < 8) return vec;
    QByteArray result = vec;

    char arr[4];
    arr[3] = new_size;
    arr[2] = new_size >> 8;
    arr[1] = new_size >> 16;
    arr[0] = new_size >> 24;


    #if QT_VERSION >= QT_VERSION_CHECK(4,7,0)
        raw_data.replace(4, 4, arr, 4);
    #else
    	QByteArray ba(arr, 4);    
	raw_data.replace(4, 4, ba);
    #endif


    return result;
}


// input: entire vector, with header, size, flags, content
// output: same, but modified
QByteArray FileHeader::fh_set_frame_content(const QByteArray& vec_org, const QByteArray& data){

    QByteArray result = vec_org;
    result = fh_set_frame_content_size(vec_org, data.size());
    result = result.left(10);
    result.append(data);

    return result;
}



// input: file, the attribute searching for (e.g. TPOS), a reference to an output vector (EVERYTHING is included there)
// output: where did we finde the tag, negative if not available
int FileHeader::fh_get_mp3_attr(QByteArray tag, QByteArray& result){

    int offset = raw_data.indexOf(tag);
    if(offset < 0) return -1;

    QByteArray tag_header = raw_data.mid(offset, 10);
    int framesize = fh_get_frame_content_size(tag_header);

    result = raw_data.mid(offset, 10 + framesize);

    return offset;
}



bool FileHeader::fh_open_and_read_file(QString filename){
    f = new QFile(filename);

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
    org_size = header_size;
    f->seek(0);
    raw_data = f->read(10 + header_size);

    for(qint64 i=10; i<raw_data.size(); i++){
        char c = raw_data.at(i);
        if(c != 0){
            if(empty_space_len > 128) break;
            empty_space_begins = 0;
            empty_space_len = 0;
        }

        else{
            if(empty_space_begins == 0)
                empty_space_begins = i;

            empty_space_len++;
        }
    }

    if(empty_space_begins == 0) {
        empty_space_begins = header_size;
        empty_space_len = 0;
    }

    else{
        empty_space_begins++;
        empty_space_len--;
    }

    f->close();
    return true;
}


QByteArray FileHeader::read(const QByteArray& what){
    QByteArray result;
    this->fh_get_mp3_attr(what, result);
    return result;
}


void FileHeader::write(const QByteArray& arr){
    things_to_write.push_back(arr);
}


bool FileHeader::commit(){

    int increase_size = 0;

    if(!valid){
        char c[] = {'I', 'D', '3', 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        raw_data = QByteArray(c);

        this->fh_update_size();

        for(int i=0; i<1024; i++){
            raw_data.push_back((char)(0x00));
        }

        increase_size = 1034;
        empty_space_begins = 10;
        empty_space_len = 1024;

        valid = true;
    }


    foreach(QByteArray arr, things_to_write){

        QByteArray attr_arr;
        int old_frame_size = 0;
        long pos = fh_get_mp3_attr(arr.left(4), attr_arr);

        if (pos > 0){
            old_frame_size = attr_arr.size();
        }

        else{
            old_frame_size = 0;
            pos = header_size;
        }

        // old_vec < new_vec
        if( old_frame_size <= arr.size() ){

            // vec does not exist...
            if(old_frame_size <= 0){

                // overwrite empty space
                if(empty_space_len > 128){


		    #if QT_VERSION >= QT_VERSION_CHECK(4,7,0)
		        raw_data.replace(empty_space_begins, arr.size(), arr, arr.size());
		    #else
			raw_data.replace(empty_space_begins, arr.size(), arr);
		    #endif



                    empty_space_begins += arr.size();
                    empty_space_len -= arr.size();
                }

                // append it to end
                else {

                    raw_data.push_back(arr);
                    increase_size += arr.size();
                }
            }

            // vec is there but too small
            else{




	        #if QT_VERSION >= QT_VERSION_CHECK(4,7,0)
	                raw_data.replace(pos, old_frame_size, arr, old_frame_size);
		#else
			raw_data.replace(pos, old_frame_size, arr);
		#endif

                for(int i=old_frame_size; i<arr.size(); i++)
                    raw_data.insert(pos + i, arr[i]);

                if(empty_space_len > 128){

                    raw_data.remove(empty_space_begins, arr.size() -old_frame_size);
                    empty_space_len -=  (arr.size() -old_frame_size);
                }

                else {

                    increase_size += arr.size() - old_frame_size;
                }

            }
        }

        else { // old vec > new vec
	    #if QT_VERSION >= QT_VERSION_CHECK(4,7,0)
                raw_data.replace(pos, arr.size(), arr, arr.size());
            #else
		raw_data.replace(pos, arr.size(), arr);
	    #endif
       
            raw_data.remove(pos + arr.size(), old_frame_size - arr.size());

            for(int i=0; i<old_frame_size - arr.size(); i++){
                raw_data.insert(empty_space_begins+1, (char) 0x00);

            }

            empty_space_len += old_frame_size - arr.size();
        }

        this->fh_update_size();
    }




    f->open(QFile::ReadWrite);
    if(!f->isOpen()) return false;
    stretch_file(f, org_size, increase_size);
    f->seek(0);
    f->write(raw_data);
    f->close();
    return true;
}

