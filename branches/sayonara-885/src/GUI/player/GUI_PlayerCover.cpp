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

    QIcon icon;
	if(radio) {
        icon = Helper::getIcon("radio.png");
	}

	else {
        icon = Helper::getIcon("logo.png");
	}

    albumCover->setIcon(icon);
	albumCover->repaint();
}


void GUI_Player::fetch_cover() {

	set_std_cover( (_md.radio_mode != RadioModeOff) );

	if(_md.album_id > -1) {
		m_cov_lookup->fetch_album_cover_by_id(_md.album_id);
	}


	else{
		m_cov_lookup->fetch_album_cover_standard(_md.artist, _md.album);
	}
}


void GUI_Player::coverClicked() {

	if(_md.album_id >= 0) {
	   m_alternate_covers->start(_md.album_id);
    }

    else {
		m_alternate_covers->start( _md.album, _md.artist);
    }

    this->setFocus();
}


void GUI_Player::sl_alternate_cover_available(const CoverLocation& lc) {

	Q_UNUSED(lc);

	if(!m_metadata_available) return;
    fetch_cover();
}


void GUI_Player::sl_no_cover_available() {

   set_std_cover( (_md.radio_mode != RadioModeOff) );
}


void GUI_Player::sl_cover_found(const CoverLocation& cl) {

	QIcon icon(cl.cover_path);

	albumCover->setIcon(icon);
	albumCover->repaint();
}

/** COVER END **/

