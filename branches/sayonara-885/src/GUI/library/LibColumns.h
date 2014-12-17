#ifndef LIBCOLUMNS_H
#define LIBCOLUMNS_H

enum LibColumn {

	LibColumn_ArtistNAlbums=0,
	LibColumn_ArtistName,
	LibColumn_ArtistNTracks,

	LibColumn_AlbumSampler,
	LibColumn_AlbumName,
	LibColumn_AlbumDuration,
	LibColumn_AlbumNTracks,
	LibColumn_AlbumYear,
	LibColumn_AlbumTracks,

	LibColumn_TrackTitle,
	LibColumn_TrackArtist,
	LibColumn_TrackAlbum,
	LibColumn_TrackYear,
	LibColumn_TrackDuration,
	LibColumn_TrackBitrate,
	LibColumn_TrackFilesize,
	LibColumn_TrackRating
};

#endif // LIBCOLUMNS_H
