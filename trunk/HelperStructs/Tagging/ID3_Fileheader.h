#ifndef ID3_FILEHEADER_H_
#define ID3_FILEHEADER_H_

#include <QMap>
#include <QFile>
#include <QString>
#include <QByteArray>
/*

ID3v2/file identifier   "ID3"
ID3v2 version           $03 00
ID3v2 flags             %abc00000
ID3v2 size              4 * %0xxxxxxx
*/

struct Frame {
	QByteArray four;
	QString name;
};




struct ID3_FileHeader {

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
	bool		fh_is_unicode_frame(const QByteArray& data_wo_header, bool* little_endian);



public:

    ID3_FileHeader(QString filename);

    bool      is_valid();

	QString 	read(QByteArray& four, QByteArray& dst_arr);
	QByteArray 	read(QByteArray& four);
    
	QByteArray  fh_extract_frame_content(const QByteArray& data_w_header);
	QString		fh_convert_frame_content_to_text(const QByteArray& data_wo_header);
    QByteArray  fh_calc_frame_content_size_int_to_byte(uint new_size);
	int 		fh_calc_frame_content_size_byte_to_int(const QByteArray& ten);
	void        fh_set_frame_content(const QByteArray& four, const QByteArray& data_wo_header);
	bool		commit();
};


#endif
                             
