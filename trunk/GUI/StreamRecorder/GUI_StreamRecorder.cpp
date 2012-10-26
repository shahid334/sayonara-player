/* GUI_StreamRecorder.cpp

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
 * May 13, 2012 
 *
 */

#include "GUI_StreamRecorder.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/CSettingsStorage.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QWidget>
#include <QDebug>
#include <QFileDialog>

GUI_StreamRecorder::GUI_StreamRecorder(QWidget* parent) {

    this->ui = NULL;

    hide();
}


GUI_StreamRecorder::~GUI_StreamRecorder() {
	// TODO Auto-generated destructor stub
}


void GUI_StreamRecorder::changeSkin(bool dark){
    QString button_style = Style::get_btn_style(dark);

    if(!ui) return;
    this->ui->btn_cancel->setStyleSheet(button_style);
    this->ui->btn_ok->setStyleSheet(button_style);
    this->ui->btn_path->setStyleSheet(button_style);
    this->ui->le_path->setStyleSheet(Style::get_lineedit_style(dark));
}


void GUI_StreamRecorder::sl_cb_activate_toggled(bool b){
	_is_active = b;
	_settings->setStreamRipper(b);
	emit sig_stream_recorder_active(b);
}

void GUI_StreamRecorder::sl_cb_complete_tracks_toggled(bool b){
	_is_complete_tracks = b;
	_settings->setStreamRipperCompleteTracks(b);
	emit sig_complete_tracks(b);
}

void GUI_StreamRecorder::sl_cb_create_playlist_toggled(bool b){
	_is_create_playlist = b;
	_settings->setStreamRipperPlaylist(b);
	emit sig_create_playlist(b);
}

void GUI_StreamRecorder::sl_btn_path_clicked(){

	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose target directory"), _path, QFileDialog::ShowDirsOnly);
	if(dir.size() > 0){
		_path = dir;
		_settings->setStreamRipperPath(_path);
		this->ui->le_path->setText(_path);
		emit sig_path_changed(_path);
	}
}

void GUI_StreamRecorder::show_win(){

    if(ui == NULL){
        ui = new Ui::GUI_StreamRecorder();
        ui->setupUi(this);

        _settings = CSettingsStorage::getInstance();

        _path = _settings->getStreamRipperPath();
        _is_active = _settings->getStreamRipper();
        _is_complete_tracks = _settings->getStreamRipperCompleteTracks();
        _is_create_playlist = _settings->getStreamRipperPlaylist();

        this->ui->le_path->setText(_path);
        this->ui->cb_activate->setChecked(_is_active);
        this->ui->cb_complete_tracks->setChecked(_is_complete_tracks);
        this->ui->cb_create_playlist->setChecked(_is_create_playlist);

        this->ui->cb_complete_tracks->setEnabled(_is_active);
        this->ui->cb_create_playlist->setEnabled(_is_active);
        this->ui->btn_path->setEnabled(_is_active);

        QPixmap pm(QPixmap(Helper::getIconPath() + "rec.png"));

        this->ui->lab_icon->setPixmap( pm.scaledToWidth(this->ui->lab_icon->maximumWidth()) );
        this->setModal(true);

        connect(this->ui->cb_activate, SIGNAL(toggled(bool)), this, SLOT(sl_cb_activate_toggled(bool)));
        connect(this->ui->cb_complete_tracks, SIGNAL(toggled(bool)), this, SLOT(sl_cb_complete_tracks_toggled(bool)));
        connect(this->ui->cb_create_playlist, SIGNAL(toggled(bool)), this, SLOT(sl_cb_create_playlist_toggled(bool)));
        connect(this->ui->btn_path, SIGNAL(clicked()), this, SLOT(sl_btn_path_clicked()));
        connect(this->ui->btn_ok, SIGNAL(clicked()), this, SLOT(sl_ok()));
    }

    show();
}

void GUI_StreamRecorder::sl_ok(){
	CDatabaseConnector::getInstance()->store_settings();
    hide();
    close();
}
