/*
 * GUI_PlayerCover.cpp
 *
 *  Created on: 07.12.2012
 *      Author: luke
 */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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


#include "GUI/player/GUI_Player.h"

/** COVERS **/

void GUI_Player::set_std_cover(bool radio) {

	QString std_cover_path;
	if(radio) {
		std_cover_path = Helper::getIconPath() + "radio.png";
	}

	else {
		std_cover_path = Helper::getIconPath() + "logo.png";
	}

	ui->albumCover->setIcon(QIcon(std_cover_path));
	ui->albumCover->repaint();

}


void GUI_Player::fetch_cover() {

	set_std_cover( (m_metadata.radio_mode != RADIO_OFF) );

	if(m_metadata.album_id > -1) {
		m_cov_lookup->fetch_album_cover_by_id(m_metadata.album_id);
	}


	else{
		m_cov_lookup->fetch_album_cover_standard(m_metadata.artist, m_metadata.album);
	}
}


void GUI_Player::coverClicked() {

   if(m_metadata.radio_mode == RADIO_STATION) {

		m_alternate_covers->start( m_metadata.album, m_metadata.title );
    }

    else if(m_metadata.album_id >= 0) {
       m_alternate_covers->start(m_metadata.album_id);
    }

    else {

        m_alternate_covers->start( m_metadata.album, m_metadata.artist);
    }

    this->setFocus();
}


void GUI_Player::sl_alternate_cover_available(bool b) {

	if(!b) {
		set_std_cover(m_metadata.radio_mode != RADIO_OFF);
		return;
	}

	fetch_cover();
}


void GUI_Player::sl_no_cover_available() {

   set_std_cover( (m_metadata.radio_mode != RADIO_OFF) );
}


// public slot
// cover was found by CoverLookup
void GUI_Player::cover_found(QString cover_path) {

	QIcon icon(cover_path);

	ui->albumCover->setIcon(icon);
	ui->albumCover->repaint();
}

/** COVER END **/

