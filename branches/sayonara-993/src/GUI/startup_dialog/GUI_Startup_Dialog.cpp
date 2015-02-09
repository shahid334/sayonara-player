/* GUI_Startup_Dialog.cpp */

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



#include "GUI/startup_dialog/GUI_Startup_Dialog.h"
#include "GUI/ui_GUI_Startup_Dialog.h"


GUI_Startup_Dialog::GUI_Startup_Dialog(QWidget *parent) :
	SayonaraDialog(parent),
	Ui::GUI_StartupDialog()
{
	setupUi(this);

	bool load_pl_on_startup, load_last_track, remember_time, start_playing;

	load_pl_on_startup = _settings->get(Set::PL_Load);
	load_last_track = _settings->get(Set::PL_LoadLastTrack);
	remember_time = _settings->get(Set::PL_RememberTime);
	start_playing = _settings->get(Set::PL_StartPlaying);

	cb_load_pl_on_startup->setChecked(load_pl_on_startup);
	cb_load_last_track->setChecked(load_last_track);
	cb_remember_time->setChecked(remember_time);
	cb_start_playing->setChecked(start_playing);


    cb_toggled(true);

	connect(cb_load_pl_on_startup, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
	connect(cb_load_last_track, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
	connect(cb_remember_time, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
	connect(cb_start_playing, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
	connect(btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));

}

GUI_Startup_Dialog::~GUI_Startup_Dialog() {

}

void GUI_Startup_Dialog::language_changed() {
	retranslateUi(this);
}

void GUI_Startup_Dialog::cb_toggled(bool b) {
    Q_UNUSED(b);

	bool cb_load_pl_checked = cb_load_pl_on_startup->isChecked();

	cb_load_last_track->setEnabled(cb_load_pl_checked);
	cb_remember_time->setEnabled(cb_load_pl_checked);
	cb_start_playing->setEnabled(cb_load_pl_checked);


	bool cb_load_last_track_checked = cb_load_last_track->isChecked() && cb_load_last_track->isEnabled();
	cb_remember_time->setEnabled(cb_load_last_track_checked);

}

void GUI_Startup_Dialog::ok_clicked() {

	_settings->set( Set::PL_Load, cb_load_pl_on_startup->isChecked() );
	_settings->set( Set::PL_LoadLastTrack, (cb_load_last_track->isChecked() && cb_load_last_track->isEnabled()) );
	_settings->set( Set::PL_RememberTime, (cb_remember_time->isChecked() && cb_remember_time->isEnabled()) );
	_settings->set( Set::PL_StartPlaying, (cb_start_playing->isChecked() && cb_start_playing->isEnabled()) );
    close();
}



