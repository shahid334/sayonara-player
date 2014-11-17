/* GUI_PlaylistEntry.cpp */

/* Copyright (C) 2011  Lucio Carreras
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


/*
 * GUI_PlaylistEntry.cpp
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

#include "GUI/playlist/entry/GUI_PlaylistEntryBig.h"
#include "Settings/Settings.h"
#include "HelperStructs/Helper.h"



GUI_PlaylistEntryBig::GUI_PlaylistEntryBig(QWidget* parent) :
	GUI_PlaylistEntry(parent),
	Ui::PlaylistEntryBig(){

	setupUi(this);
}

GUI_PlaylistEntryBig::~GUI_PlaylistEntryBig() {

}


void GUI_PlaylistEntryBig::setContent(const MetaData& md, int idx) {

    QString titlestr;
	bool show_numbers = Settings::getInstance()->get(Set::PL_ShowNumbers);

	if(show_numbers){
		titlestr = QString::number(idx) + ". " + md.title.trimmed();
	}

	else{
		titlestr = md.title.trimmed();
	}

	lab_title->setText(titlestr);
	lab_artist->setText(md.artist.trimmed());
	lab_time->setText(Helper::cvtMsecs2TitleLengthString(md.length_ms, true));
    if(md.album == "") {
		lab_album->setText("");
        return;
    }

	lab_album->setText(QString(" [ ") + Helper::get_album_w_disc(md) + " ]");

}


