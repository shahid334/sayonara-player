#include <HelperStructs/MetaData.h>
#include <QString>
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


namespace ID3{


MetaData getMetaDataOfFile(QString file);

void setMetaDataOfFile(MetaData& md);


void getMetaDataOfFile(TagLib::FileRef&, QString file, MetaData&);
};



