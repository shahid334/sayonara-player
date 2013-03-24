/* ID3_Fileheader.cpp */

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



#include "HelperStructs/Tagging/ID3_Fileheader.h"

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
#include <QFile>
#include <QDebug>

void stretch_file(QFile* f, long offset, int n_bytes);


const int N_Tags = 75;
Frame taglist[] = {
	{"TPE1", "Artist"},		{"TPE2", "Band"},		{"TIT2", "Title"}, 		{"TCOM", "Composer"},
	{"TALB", "Album"},		{"TDRC", "Year"},		{"TRCK", "TrackNumber"},{"TPOS", "Discnumber"},
	{"COMM", "Comment"},	{"TLEN", "Length"},		{"TORY", "Org Release year"}, {"TYER", "Year"},
 	{"PRIV", "Other"},

	{"AENC", "unknown"},	{"APIC", "unknown"},	{"COMR", "unknown"},	{"ENCR", "unknown"},
	{"EQUA", "unknown"},	{"ETCO", "unknown"},	{"GEOB", "unknown"},	{"GRID", "unknown"},
	{"IPLS", "unknown"},	{"LINK", "unknown"},	{"MCDI", "unknown"},	{"MLLT", "unknown"},
	{"OWNE", "unknown"},	{"PCNT", "unknown"},	{"POPM", "unknown"},   	{"SYTC", "unknown"},
	{"POSS", "unknown"},	{"RBUF", "unknown"},	{"RVAD", "unknown"},	{"WPUB", "unknown"},
	{"RVRB", "unknown"},	{"SYLT", "unknown"},		

// text attributes	
	{"TCON", "unknown"},	{"TCOP", "unknown"},	{"TDAT", "unknown"},	{"TDLY", "unknown"},
	{"TENC", "unknown"},	{"TEXT", "unknown"},	{"TFLT", "unknown"},	{"TIME", "unknown"},
	{"TIT1", "unknown"},	{"TIT3", "unknown"},	{"TKEY", "unknown"},	{"TMED", "unknown"},
	{"TOAL", "unknown"},	{"TOFN", "unknown"},	{"TOLY", "unknown"},	{"TOPE", "unknown"},
	{"TOWN", "unknown"},	{"TPE3", "unknown"},	{"TPE4", "unknown"},	{"TSIZ", "unknown"},
	{"TPUB", "unknown"},	{"TRDA", "unknown"},	{"TRSN", "unknown"},	{"TSSE", "unknown"},
	{"TRSO", "unknown"},	{"TSRC", "unknown"},	{"TXXX", "unknown"},	{"TLAN", "unknown"},
	{"TBPM", "unknown"},	

	{"UFID", "unknown"},	{"USER", "unknown"},	{"USLT", "unknown"},	{"WPAY", "unknown"},
	{"WCOM", "unknown"},	{"WCOP", "unknown"},	{"WOAF", "unknown"},	{"WOAR", "unknown"},
	{"WOAS", "unknown"},	{"WORS", "unknown"},	{"WXXX", "unknown"}
		
};


ID3_FileHeader::ID3_FileHeader(QString filename){

    header_size = 0;
    org_size = 0;

    valid = fh_open_and_read_file(filename);
}

bool ID3_FileHeader::is_valid(){
    return valid;
}

bool ID3_FileHeader::fh_is_unicode_frame(const QByteArray& data_wo_header, bool* little_endian){


	QByteArray be;
	be.push_back((char) 0xFF);
	be.push_back((char) 0xFE);

	QByteArray le;
	le.push_back((char) 0xFE);
	le.push_back((char) 0xFF);

	int idx_be = data_wo_header.left(4).indexOf(be);
	int idx_le = data_wo_header.left(4).indexOf(le);

	if(idx_be >= 0){
		*little_endian = false;
		return true;
	}

	else if(idx_le >= 0){
		*little_endian = true;
		return true;
	}

	else{
		*little_endian = false;
		return false;
	}

}



void ID3_FileHeader::fh_update_size(){

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


qint64 ID3_FileHeader::fh_read_header_size(const QByteArray& ten){

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
QByteArray ID3_FileHeader::fh_calc_frame_content_size_int_to_byte(uint new_size){

    char c_new_size[4];

    c_new_size[0] = new_size >> 21; c_new_size[0] &= 0x7F;
    c_new_size[1] = new_size >> 14; c_new_size[1] &= 0x7F;
    c_new_size[2] = new_size >> 7; c_new_size[2] &= 0x7F;
    c_new_size[3] = new_size; c_new_size[3] &= 0x7F;

    return QByteArray(c_new_size, 4);
}


// input: entire vector, with header, size, flags, content
// output: of size of content
int ID3_FileHeader::fh_calc_frame_content_size_byte_to_int(const QByteArray& ten){

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
void ID3_FileHeader::fh_set_frame_content(const QByteArray& four, const QByteArray& data_wo_header){

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
QByteArray ID3_FileHeader::fh_extract_frame_content(const QByteArray& data_w_header){

    QByteArray result;
    int len_frame = fh_calc_frame_content_size_byte_to_int(data_w_header.left(10)) + 10;

    if(len_frame > data_w_header.size()) return result;

    for(int i=10; i<len_frame; i++)
        result.push_back(data_w_header[i]);

    return result;
}


QString ID3_FileHeader::fh_convert_frame_content_to_text(const QByteArray& data_wo_header){

	QString ret;
	bool little_endian;
	bool is_unicode = fh_is_unicode_frame(data_wo_header, &little_endian);

	QByteArray le;
		le.push_back((char) 0xFE);
		le.push_back((char) 0xFF);

	QByteArray be;
		be.push_back((char) 0xFF);
		be.push_back((char) 0xFE);


	if(is_unicode){
		
		if(little_endian){ // FE FF
			int idx = data_wo_header.indexOf(le) + 2;
			for(int i=idx; i<data_wo_header.size(); i+=2){
				uchar num = (uchar)(data_wo_header[i] * 256) + (uchar)(data_wo_header[i+1]);
				QChar c(num);
				ret.push_back(c);
			}
		}

		else if(!little_endian){ // FF FE
			int idx = data_wo_header.indexOf(be) + 2;
			for(int i=idx; i<data_wo_header.size(); i+=2){
				uchar num = (uchar)(data_wo_header[i]) + (uchar)(data_wo_header[i+1] * 256);
				QChar c(num);
				ret.push_back(c);
			}
		}
	}

	else{
		for(int i=0; i<data_wo_header.size(); i+=2){
			char c = data_wo_header[i];
			ret.push_back(c);
		}
	}

	return ret;
}


bool ID3_FileHeader::fh_open_and_read_file(QString filename){
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
	valid = false;
        return false;
    }

    header_size = fh_read_header_size(header_header);
    qDebug() << "Header size = " << header_size;
    org_size = header_size;
    f->seek(0);
    raw_data = f->read(10 + header_size);

	// this O(n²) looks slower as it is
    for(qint64 i=10; i<raw_data.size(); i++){

        QByteArray four = raw_data.mid(i, 4);

		for(int t=0; t<N_Tags; t++){

            // tag found
            if( four == QByteArray(taglist[t].four, 4) ){

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
        }
    }

    QList<QByteArray> keys = all_frames.keys();
    foreach(QByteArray key, keys){
        //qDebug() << "have " << key;
    }

    f->close();
    return true;
}




QString ID3_FileHeader::read(QByteArray& four, QByteArray& dst_arr){

    dst_arr = all_frames.value(four);
	if(dst_arr.size() == 0) return "";

	fh_extract_frame_content(dst_arr);
	return fh_convert_frame_content_to_text(dst_arr);
}


QByteArray ID3_FileHeader::read(QByteArray& four){
	return all_frames.value(four);
}


bool ID3_FileHeader::commit(){

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
	for(int i=0; i<1024; i++){
            raw_data.push_back((char) 0x00);
	}

        increase_size = -arr_difference + 1024;
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



