/* globals.h */

/* Copyright (C) 2012  Lucio Carreras
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

#include <QString>
#include <QDebug>


#ifndef GLOBALS_H_
#define GLOBALS_H_


#define SAYONARA_ORANGE_STR QString("#e8841a")


#define SAYONARA_ORANGE_COL QColor(232, 132, 26)




#define RADIO_OFF 0
#define RADIO_LFM 1
#define RADIO_STATION 2

#define PLUGIN_NUM 4
#define PLUGIN_NONE 0
#define PLUGIN_LFM_RADIO 1
#define PLUGIN_STREAM 2
#define PLUGIN_EQUALIZER 3
#define PLUGIN_PLAYLIST_CHOOSER 4


#define GOOGLE_IMG_ICON QString("icon")
#define GOOGLE_IMG_SMALL QString("small")
#define GOOGLE_IMG_MEDIUM QString("medium")
#define GOOGLE_IMG_LARGE QString("large")
#define GOOGLE_IMG_XLARGE QString("xlarge")
#define GOOGLE_IMG_XXLARGE QString("xxlarge")
#define GOOGLE_IMG_HUGE QString("huge")

#define GOOGLE_FT_JPG QString("jpg")
#define GOOGLE_FT_PNG QString("png")
#define GOOGLE_FT_GIF QString("gif")
#define GOOGLE_FT_BMP QString("bmp")


#define SINGLETON(x) private: \
						x(); \
						x(const x&); \
						x& operator=(const x&); \
						\
					public:\
							virtual ~x(); \
						\
					static x* getInstance(){ \
							static x inst; \
							return &inst; \
					}\


namespace Sort{

	enum SortOrder{
		NoSorting=0,
		ArtistNameAsc,
		ArtistNameDesc,
		ArtistTrackcountAsc,
		ArtistTrackcountDesc,
		AlbumNameAsc,
		AlbumNameDesc,
		AlbumYearAsc,
		AlbumYearDesc,
		AlbumTracksAsc,
		AlbumTracksDesc,
		AlbumDurationAsc,
		AlbumDurationDesc,
		TrackNumAsc,
		TrackNumDesc,
		TrackTitleAsc,
		TrackTitleDesc,
		TrackAlbumAsc,
		TrackAlbumDesc,
		TrackArtistAsc,
		TrackArtistDesc,
		TrackYearAsc,
		TrackYearDesc,
		TrackLenghtAsc,
		TrackLengthDesc,
		TrackBitrateAsc,
		TrackBitrateDesc,
		TrackSizeAsc,
		TrackSizeDesc
	};

};





#endif /* GLOBALS_H_ */
