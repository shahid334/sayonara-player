/* GUILibraryInfoBox.cpp

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Sep 2, 2012 
 *
 */

#include "GUI/library/InfoBox/GUILibraryInfoBox.h"
#include "GUI/ui_GUI_Library_Info_Box.h"

#include "DatabaseAccess/CDatabaseConnector.h"
#include "StreamPlugins/LastFM/LastFM.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"

#include <QMap>
#include <QPixmap>


GUI_Library_Info_Box::GUI_Library_Info_Box(QWidget* parent) :
	SayonaraDialog(parent),
	Library_Info_Box()
{
	setupUi(this);

	_db = CDatabaseConnector::getInstance();
	_lfm = LastFM::getInstance();

    hide();
}

GUI_Library_Info_Box::~GUI_Library_Info_Box() {

}



void GUI_Library_Info_Box::language_changed() {

	retranslateUi(this);
}


void GUI_Library_Info_Box::psl_refresh() {

    MetaDataList v_md;
	AlbumList v_albums;
	ArtistList v_artists;
	bool lfm_active = _settings->get(Set::LFM_Active);

	_db->getTracksFromDatabase(v_md);
	_db->getAllAlbums(v_albums);
	_db->getAllArtists(v_artists);

	_n_tracks = v_md.size();
	_n_albums = v_albums.size();
	_n_artists = v_artists.size();
	_duration_ms = 0;
    _filesize = 0;

	for( const MetaData& md : v_md ) {
		_duration_ms += md.length_ms;
        _filesize += md.filesize;
	}

	_duration_string = Helper::cvt_ms_to_string(_duration_ms, false);
    _filesize_str = Helper::calc_filesize_str(_filesize);

	if( !lfm_active ) {
		_n_lfm_playcount = -1;
		_n_lfm_days_registered = -1;
		lab_lfm_playcount->setText("Last.fm not active");
	}

	else {
		QMap<QString, QString> map;
		if(_lfm->lfm_get_user_info(map)) {
			_n_lfm_playcount = map["playcount"].toInt();
			QString reg_date = map["register_date"];
			int y, m, d;
			y = reg_date.left(4).toInt();
			m = reg_date.mid(5, 2).toInt();
			d = reg_date.mid(8, 2).toInt();
			QDate date(y, m, d);
			_n_lfm_days_registered = date.daysTo(QDate::currentDate());
			lab_lfm_playcount->setText(QString::number(_n_lfm_playcount) + " -> " + QString::number((_n_lfm_playcount * 1.0) / _n_lfm_days_registered, ' ', 2) );
		}

		else{
			_n_lfm_playcount = -1;
			_n_lfm_days_registered = -1;
			lab_lfm_playcount->setText("-");
		}
	}

	lab_album_count->setText(QString::number(_n_albums));
	lab_track_count->setText(QString::number(_n_tracks));
	lab_artist_count->setText(QString::number(_n_artists));
	lab_duration_value->setText(_duration_string + "s");
	lab_filesize->setText(_filesize_str);

	show();
}


void GUI_Library_Info_Box::lfm_data_available() {

}

