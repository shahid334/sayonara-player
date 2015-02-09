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
	void get_all_albums_by_artist(AlbumList& albums, int artist_id, LibSortOrder so);

	void get_all_tracks(MetaDataList& v_md, LibSortOrder so);
	void get_all_tracks_by_artist(MetaDataList& v_md, int artist_id, LibSortOrder so);
	void get_all_tracks_by_album(MetaDataList& v_md, int album_id, LibSortOrder so);


	bool load();

	void sort_tracks(MetaDataList& v_md, LibSortOrder so);
	void sort_albums(AlbumList& albums, LibSortOrder so);
	void sort_artists(ArtistList& artists, LibSortOrder so);


private:

	SoundcloudParser* _sc_parser;

	LibSortOrder	_sortorder;

};

#endif // SOUNDCLOUDDATA_H
