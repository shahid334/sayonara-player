#include <HelperStructs/MetaData.h>
#include <HelperStructs/id3.h>
#include <HelperStructs/Helper.h>

#include <QString>
#include <QObject>

#include <string>
#ifdef _WIN32
	#include <tag.h>
	#include <taglib.h>
	#include <fileref.h>
#else
	#include <taglib/tag.h>
	#include <taglib/taglib.h>
	#include <taglib/fileref.h>
#endif
using namespace Helper;

MetaData ID3::getMetaDataOfFile(QString file){
	MetaData md;
	TagLib::FileRef f(TagLib::FileName(file.toUtf8()));
		if(f.isNull()) return md;
		string artist = f.tag()->artist().to8Bit(true);
		string album = f.tag()->album().to8Bit(true);
		string title = f.tag()->title().to8Bit(true);
		uint year = f.tag()->year();
		uint track = f.tag()->track();
		int bitrate = f.audioProperties()->bitrate();


		int length = f.audioProperties()->length();


		md.album = cvtQString2FirstUpper(QString::fromLocal8Bit(album.c_str()));
		md.artist = cvtQString2FirstUpper(QString::fromLocal8Bit(artist.c_str()));
		md.title = cvtQString2FirstUpper(QString::fromLocal8Bit(title.c_str()));
		md.filepath = file;
		md.length_ms = length * 1000;
		md.year = year;
		md.track_num = track;
		md.bitrate = bitrate;


		if(md.title.length() == 0){
			int idx = md.filepath.lastIndexOf('/');
			md.title = md.filepath.right(md.filepath.length() - idx -1);
			md.title = md.title.left(md.title.length() - 4);
		}

		return md;

}
