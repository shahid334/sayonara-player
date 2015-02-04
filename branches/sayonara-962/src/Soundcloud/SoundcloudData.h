#ifndef SOUNDCLOUDDATA_H
#define SOUNDCLOUDDATA_H

#include <QObject>
#include "HelperStructs/MetaData.h"

#include "Library/Sorting.h"
#include "Soundcloud/SoundcloudHelper.h"



class SoundcloudData : public QObject
{
	Q_OBJECT

public:
	explicit SoundcloudData(QObject *parent = 0);
	virtual ~SoundcloudData();

	void get_all_artists(ArtistList& artists, LibSortOrder so);
	void get_all_albums(AlbumList& albums, LibSortOrder so);
	void get_all_tracks(MetaDataList& v_md, LibSortOrder so);

	bool load();


private:

	SoundcloudParser* _sc_parser;

	LibSortOrder	_sortorder;
};

#endif // SOUNDCLOUDDATA_H
