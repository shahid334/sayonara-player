/* GUI_SoundCloudLibrary.cpp */

/* Copyright (C) 2011-2014  Lucio Carreras
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


#include "GUI/soundcloud/GUI_SoundcloudLibrary.h"


GUI_SoundCloudLibrary::GUI_SoundCloudLibrary(SoundcloudLibrary* library, GUI_InfoDialog* info_dialog, QWidget *parent) :
	GUI_AbstractLibrary(library, info_dialog, parent),
	Ui::GUI_SoundcloudLibrary()
{
	setupUi(this);

	set_combo_lib_chooser(combo_lib_chooser);
	set_lv_elems(lv_artist, lv_album, tb_title);
	set_search_elems(combo_searchfilter, btn_clear, le_search);
	set_btn_info(btn_info);
	set_btn_refresh(btn_refresh);
	set_lab_status(lab_status);
	init_finished();

	setAcceptDrops(false);

	connect(btn_add, SIGNAL(clicked()), this, SLOT(btn_add_clicked()));

	hide();
}

AbstractLibrary::TrackDeletionMode GUI_SoundCloudLibrary::show_delete_dialog(int n_tracks){

	return AbstractLibrary::TrackDeletionModeNone;
}

void GUI_SoundCloudLibrary::btn_add_clicked(){
	// popup little window
}
