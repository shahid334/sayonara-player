#ifndef COVERLOCATION_H
#define COVERLOCATION_H

#include "HelperStructs/MetaData.h"
class CoverLocation;

class CoverLocation
{

private:

	static CoverLocation getInvalidLocation();

public:

	QString google_url;
	QString cover_path;

	CoverLocation();

	static CoverLocation get_cover_location(const QString& album_name, const QString& artist_name);
	static CoverLocation get_cover_location(const QString& album_name, const QStringList& artists);
	static CoverLocation get_cover_location(int album_id);
	static CoverLocation get_cover_location(const Album& album);
	static CoverLocation get_cover_location(const Artist& artist);
	static CoverLocation get_cover_location(const QString& artist);
	static CoverLocation get_cover_location(const MetaData& md);

};

#endif // COVERLOCATION_H
