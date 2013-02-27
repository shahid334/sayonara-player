#include <QMap>
#include <QByteArray>
#include <QFile>
#include <QList>
#include <QMap>


typedef unsigned char uchar;
typedef unsigned int uint;

struct FileHeader {
	bool valid;
	uint header_size;
	uint org_size;

	QByteArray raw_data;
	QMap<QByteArray, uint> pos_mapping;
};


bool       id3_get_file_header_valid(QFile& f);
long       id3_get_file_header_size(QFile& f);
void       id3_set_file_header_size(QFile& f, int new_size);
QByteArray id3_get_file_header_end_str(QFile& f);

int        id3_get_frame_content_size(const QByteArray& vec);
QByteArray id3_set_frame_content_size(const QByteArray& vec, uint new_size);
QByteArray id3_get_frame_content(const QByteArray& vec);
QByteArray id3_set_frame_content(const QByteArray& vec_org, const QByteArray& data);

long       id3_get_mp3_attr(QFile& f, char* tag, uint tag_size, QByteArray& result);
void       stretch_file(QFile& f, long offset, int n_bytes);



FileHeader id3_get_file_header(QFile& f){
	
/*	FileHeader fh;

	char bytes[11];
	f.seekg(0, ios_base::beg);
	f.read(bytes, 10);
	bytes[11] = '\0';

	string mystr(bytes);
	if(mystr.cmp(0, 3, "ID3") != 0) {
		fh.valid = false;
		fh.header_size = 0;
		fh.org_size = 0;

		return fh;
	}

	fh.valid = true;
	fh.header_size = id3_get_file_header_size(f);
	fh.org_size = fh.header_size;

	f.seekg(0, ios_base::beg);
	char* header_bytes = new char[fh.header_size];

	f.read(header_bytes, fh.header_size);
	for(uint i=0; i<fh.header_size; i++){
		fh.raw_data.push_back(header_bytes[i]);
	}

	delete header_bytes;

	return fh;	*/
	FileHeader fh;
	return fh;
}

void id3_set_file_header(QFile& f, FileHeader& fh){

//	stretch_file(f, offset, fh.raw_data.header_size - fh.old_size);
	

}


long id3_get_file_header_size(QFile& f){

    long headersize = 0;
    int multiplier = 1;

    char bytes[4];
    f.seek(6);
    f.read(bytes, 4);
    for(int i=3; i>=0; i--){
            headersize += (bytes[i] * multiplier);
            multiplier *= 128;
    }

    f.seek(0);

    return headersize;
}


void id3_set_file_header_size(QFile& f, int new_size){

    // header size $(-X XX XX XX ) * 4
    char c_new_size[4];

    c_new_size[0] = new_size >> 21; c_new_size[0] &= 0x7F;
    c_new_size[1] = new_size >> 14; c_new_size[1] &= 0x7F;
    c_new_size[2] = new_size >> 7; c_new_size[2] &= 0x7F;
    c_new_size[3] = new_size; c_new_size[3] &= 0x7F;

    f.seek(6);
    f.write(c_new_size, 4);
    f.seek(0);
}


QByteArray id3_get_file_header_end_str(QFile& f){

    QByteArray result;
    int taglen = id3_get_file_header_size(f);

    // max 256 Kbyte
    if(taglen > 262143) taglen = 262143;

    f.seek(taglen + 10);
    result = f.read(8);
    return result;
}

// input: entire vector, with header, size, flags, content
// output: of size of content
int id3_get_frame_content_size(const QByteArray& vec){

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
QByteArray id3_set_frame_content_size(const QByteArray& vec, uint new_size){

    if(vec.size() < 8) return vec;
    QByteArray result = vec;

    char arr[4];
    arr[3] = new_size;
    arr[2] = new_size >> 8;
    arr[1] = new_size >> 16;
    arr[0] = new_size >> 24;

    result.replace(4, 4, arr);
    return result;
}


// input: entire vector, with header, size, flags, content
// output: content
QByteArray id3_get_frame_content(const QByteArray& vec){

    QByteArray result;
    int len_content = id3_get_frame_content_size(vec) + 10;
    if(len_content > vec.size()) return result;

    for(int i=10; i<len_content; i++)
        result.push_back(vec[i]);

    return result;
}

// input: entire vector, with header, size, flags, content
// output: same, but modified
QByteArray id3_set_frame_content(const QByteArray& vec_org, const QByteArray& data){

    QByteArray result = vec_org;
    result = id3_set_frame_content_size(vec_org, data.size());
    result = result.left(10);
    result.append(data);

    return result;
}



// input: file, the attribute searching for (e.g. TPOS), a reference to an output vector (EVERYTHING is included there)
// output: where did we finde the tag, negative if not available
long id3_get_mp3_attr(QFile& f, char* tag, uint tag_size, QByteArray& result){

    f.seek(0);
    QByteArray header = f.read(id3_get_file_header_size(f));
    int offset = header.indexOf(tag);
    if(offset < 0) return -1;

    QByteArray tag_header = header.mid(offset, 10);

    int framesize = id3_get_frame_content_size(tag_header);
    result = header.mid(offset, 10 + framesize);

    return offset;
}


bool id3_write_discnumber(QFile& f, int discnumber, int n_discs=1){
   
   if(discnumber <= 0) return false;

   long header_size = id3_get_file_header_size(f);
   long new_header_size = header_size;

   char c_new_frame[] = {'T', 'P', 'O', 'S',
               0, 0, 0, 0x05,
               0, 0,
               0x03, discnumber + '0', '/', n_discs + '0', 0x00 };

   QByteArray attr_vec;
   long pos = id3_get_mp3_attr(f, "TPOS", 4, attr_vec);

   int old_frame_size = 0;
   if (pos > 0){
      old_frame_size = id3_get_frame_content_size(attr_vec);
   }

   else{
       old_frame_size = 0;
       pos = header_size;
   }

   new_header_size = header_size + (5 - old_frame_size);

   stretch_file(f, pos, 5 - old_frame_size);
   id3_set_file_header_size(f, new_header_size);

   f.seek(pos);
   f.write(c_new_frame, 15);
}


bool id3_extract_discnumber(QFile& f, int* discnumber, int* n_discs){

    *discnumber = -1;
    *n_discs = -1;

    QByteArray vec;
    long pos = id3_get_mp3_attr(f, "TPOS", 4, vec);

    if(vec.size() == 0 || pos < 0) return false;

    const int frame_start = 10;        
    int frame_size = id3_get_frame_content_size(vec);

    bool slash_found = false;
    for(int i=0; i<frame_size; i++){
       char c = vec[i+frame_start];

	if(c == '/') {
	    slash_found = true;
	    continue;
	}

	// Ascii: '/'=47, '0'=48, '1'... '9'=57
	if(c >= '0' && c <= '9'){
	    if(!slash_found)	
	       *discnumber = 10*(*discnumber) + c;
		
	    else
	        *n_discs = 10*(*n_discs) + c;
	}
    }

    return true;
}


void stretch_file(QFile& f, long offset, int n_bytes){
    if(n_bytes <= 0 ) return;

    const int buffersize = 16384;

    f.seek(0);

    qint64 filesize = f.size();
    f.seek(filesize-buffersize);

    qint64 i=filesize - buffersize;

    while(i>offset){
           QByteArray arr = f.read(buffersize);

           f.seek( f.pos() - buffersize + n_bytes);
           f.write(arr);

           i-=buffersize;
           if(i <= offset) break;
           f.seek(f.pos() - (buffersize + n_bytes + buffersize));
    }

    i += buffersize;
    int rest = (i-offset);

    f.seek(offset);

    QByteArray restbuffer = f.read(rest);
    f.seek(f.pos() - rest + n_bytes);
    f.write(restbuffer);
}


