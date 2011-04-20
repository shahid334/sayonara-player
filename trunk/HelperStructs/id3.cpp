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

	TagLib::FileRef f(TagLib::FileName(file.toUtf8()));
		string artist = f.tag()->artist().to8Bit(false);
		string album = f.tag()->album().to8Bit(false);
		string title = f.tag()->title().to8Bit(false);
		uint year = f.tag()->year();

		int length = f.audioProperties()->length();

		MetaData md;
		md.album = cvtQString2FirstUpper(QString(album.c_str()));
		md.artist = cvtQString2FirstUpper(QString(artist.c_str()));
		md.title = cvtQString2FirstUpper(QString(title.c_str()));
		md.filepath = file;
		md.length_ms = length * 1000;
		md.year = year;


		if(md.title.length() == 0){
			int idx = md.filepath.lastIndexOf('/');
			md.title = md.filepath.right(md.filepath.length() - idx -1);
			md.title = md.title.left(md.title.length() - 4);
		}

		return md;

}
