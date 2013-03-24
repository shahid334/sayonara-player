/* GUI_Startup_Dialog.cpp */

/* Copyright (C) 2013  Lucio Carreras
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
#include "HelperStructs/CSettingsStorage.h"

GUI_Startup_Dialog::GUI_Startup_Dialog(QWidget *parent) :
    QDialog(parent)
{

    CSettingsStorage* set = CSettingsStorage::getInstance();
    ui = new Ui::GUI_StartupDialog;
    ui->setupUi(this);

    ui->cb_load_pl_on_startup->setChecked(set->getLoadPlaylist());
    ui->cb_load_last_track->setChecked(set->getLoadLastTrack());
    ui->cb_remember_time->setChecked(set->getRememberTime());
    ui->cb_start_playing->setChecked(set->getStartPlaying());

    cb_toggled(true);

    connect(ui->cb_load_pl_on_startup, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
    connect(ui->cb_load_last_track, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
    connect(ui->cb_remember_time, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
    connect(ui->cb_start_playing, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
    connect(ui->btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));

}

GUI_Startup_Dialog::~GUI_Startup_Dialog(){

}


void GUI_Startup_Dialog::cb_toggled(bool b){
    Q_UNUSED(b);

    bool cb_load_pl_checked = ui->cb_load_pl_on_startup->isChecked();

    ui->cb_load_last_track->setEnabled(cb_load_pl_checked);
    ui->cb_remember_time->setEnabled(cb_load_pl_checked);
    ui->cb_start_playing->setEnabled(cb_load_pl_checked);


    bool cb_load_last_track_checked = ui->cb_load_last_track->isChecked() && ui->cb_load_last_track->isEnabled();
    ui->cb_remember_time->setEnabled(cb_load_last_track_checked);

}

void GUI_Startup_Dialog::ok_clicked(){

    CSettingsStorage* set = CSettingsStorage::getInstance();
    set->setLoadPlaylist(ui->cb_load_pl_on_startup->isChecked());
    set->setLoadLastTrack( ui->cb_load_last_track->isChecked() && ui->cb_load_last_track->isEnabled());
    set->setRememberTime(ui->cb_remember_time->isChecked() && ui->cb_remember_time->isEnabled());
    set->setStartPlaying(ui->cb_start_playing->isChecked() && ui->cb_start_playing->isEnabled());
    close();
}



