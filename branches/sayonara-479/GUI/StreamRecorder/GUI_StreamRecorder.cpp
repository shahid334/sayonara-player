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
#include <QMessageBox>
#include <QDir>

GUI_StreamRecorder::GUI_StreamRecorder(QWidget* parent) : QDialog(parent) {

    this->ui = NULL;



    hide();
}


GUI_StreamRecorder::~GUI_StreamRecorder() {
	// TODO Auto-generated destructor stub
}


void GUI_StreamRecorder::changeSkin(bool dark){
    _skin = dark;

}


void GUI_StreamRecorder::sl_cb_activate_toggled(bool b){
	_is_active = b;
	_settings->setStreamRipper(b);
	emit sig_stream_recorder_active(b);
}

void GUI_StreamRecorder::sl_cb_complete_tracks_toggled(bool b){
	_is_complete_tracks = b;
	_settings->setStreamRipperCompleteTracks(b);
}

void GUI_StreamRecorder::sl_cb_create_session_path_toggled(bool b){
    _is_create_session_path = b;
    _settings->setStreamRipperSessionPath(b);
}

void GUI_StreamRecorder::sl_btn_path_clicked(){

	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose target directory"), _path, QFileDialog::ShowDirsOnly);
	if(dir.size() > 0){
		_path = dir;
		_settings->setStreamRipperPath(_path);
		this->ui->le_path->setText(_path);
	}
}

void GUI_StreamRecorder::sl_le_text_changed(QString & str){

}

void GUI_StreamRecorder::show_win(){

    if(ui == NULL){
        ui = new Ui::GUI_StreamRecorder();
        ui->setupUi(this);

        _settings = CSettingsStorage::getInstance();

        _path = _settings->getStreamRipperPath();
        _is_active = _settings->getStreamRipper();
        _is_complete_tracks = _settings->getStreamRipperCompleteTracks();
        _is_create_session_path = _settings->getStreamRipperSessionPath();

        this->ui->le_path->setText(_path);
        this->ui->cb_activate->setChecked(_is_active);
        this->ui->cb_complete_tracks->setChecked(_is_complete_tracks);
        this->ui->cb_create_session_path->setChecked(_is_create_session_path);

        this->ui->cb_complete_tracks->setEnabled(_is_active);
        this->ui->cb_create_session_path->setEnabled(_is_active);
        this->ui->btn_path->setEnabled(_is_active);
        this->ui->le_path->setEnabled(_is_active);

        QPixmap pm(QPixmap(Helper::getIconPath() + "rec.png"));

        this->ui->lab_icon->setPixmap( pm.scaledToWidth(this->ui->lab_icon->maximumWidth()) );
        this->setModal(true);

        connect(this->ui->cb_activate, SIGNAL(toggled(bool)), this, SLOT(sl_cb_activate_toggled(bool)));
        connect(this->ui->cb_complete_tracks, SIGNAL(toggled(bool)), this, SLOT(sl_cb_complete_tracks_toggled(bool)));
        connect(this->ui->cb_create_session_path, SIGNAL(toggled(bool)), this, SLOT(sl_cb_create_session_path_toggled(bool)));
        //connect(this->ui->le_path, SIGNAL(textEdited(QString&)), this, SLOT(sl_le_text_changed(QString&)));
        connect(this->ui->btn_path, SIGNAL(clicked()), this, SLOT(sl_btn_path_clicked()));
        connect(this->ui->btn_ok, SIGNAL(clicked()), this, SLOT(sl_ok()));
    }

    changeSkin(_skin);
    show();
}

void GUI_StreamRecorder::sl_ok(){

    QString str = ui->le_path->text();
    if(!QFile::exists(str)){
        bool create_success = QDir::root().mkpath(str);
        if(!create_success){
            QMessageBox::warning(this, tr("Could not create directory"), str + tr(" could not be created\nPlease choose another folder"));
            this->ui->le_path->setText(_settings->getStreamRipperPath());
            return;
        }
    }

    _settings->setStreamRipperPath(str);
    _path = str;

	CDatabaseConnector::getInstance()->store_settings();
    hide();
    close();
}


void GUI_StreamRecorder::record_button_toggled(bool b){
    if(!ui) return;
    ui->btn_path->setEnabled(!b);
    ui->cb_activate->setEnabled(!b);
    ui->cb_create_session_path->setEnabled(!b);
    ui->le_path->setEnabled(!b);
}
