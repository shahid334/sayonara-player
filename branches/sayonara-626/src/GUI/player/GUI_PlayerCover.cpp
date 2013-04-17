/*
 * GUI_PlayerCover.cpp
 *
 *  Created on: 07.12.2012
 *      Author: luke
 */

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


#include "GUI/player/GUI_Player.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"

#include <QIcon>
#include <QString>

/** COVERS **/
void GUI_Player::coverClicked() {

   if(m_metadata.radio_mode == RADIO_STATION){
        QString searchstring = QString("Radio ") + m_metadata.title;
        QString targetpath = Helper::get_cover_path(m_metadata.artist, m_metadata.album);

        m_alternate_covers->start(searchstring, targetpath);
    }

    else {

        QString searchstring;
        if(m_metadata.album.size() != 0 || m_metadata.artist != 0){
            searchstring = m_metadata.album + " " + m_metadata.artist;
        }

        else {
            searchstring = m_metadata.title + " " + m_metadata.artist;
        }

        searchstring = searchstring.trimmed();

        QString targetpath = Helper::get_cover_path(m_metadata.artist, m_metadata.album);

        m_alternate_covers->start(searchstring, targetpath);
    }



    this->setFocus();
}

void GUI_Player::sl_alternate_cover_available(QString target_class, QString coverpath){

    qDebug() << "Album: "
             << m_metadata.album
             << ", Artist: "
             << m_metadata.artist
             << " - "
             << Helper::get_cover_path(m_metadata.artist, m_metadata.album);

    QString own_coverpath = Helper::get_cover_path(m_metadata.artist, m_metadata.album);
    if(coverpath != own_coverpath) return;

    ui->albumCover->setIcon(QIcon(coverpath));
}

void GUI_Player::sl_no_cover_available(){


    QString coverpath = Helper::getIconPath() + "logo.png";
    ui->albumCover->setIcon(QIcon(coverpath));
}


// public slot
// cover was found by CoverLookup
void GUI_Player::cover_changed(QString caller_class, QString cover_path) {

	if(m_class_name != caller_class) return;


    if(!QFile::exists(cover_path)){

        cover_path = Helper::getIconPath() + "logo.png";
	}

	ui->albumCover->setIcon(QIcon(cover_path));
	ui->albumCover->repaint();
}

/** COVER END **/

