#include "HelperStructs/MetaData.h"
#include <QString>

namespace Podcast{

    bool parse_podcast_xml_file(QString podcast_filename, MetaDataList& v_md);
    bool parse_podcast_xml_file_content(const QString& content, MetaDataList& v_md);

};

