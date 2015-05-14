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

#include "GUI/StreamRecorder/GUI_StreamRecorder.h"
#include "HelperStructs/Helper.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

GUI_StreamRecorder::GUI_StreamRecorder(QWidget* parent) :
	SayonaraDialog(parent),

	Ui_GUI_StreamRecorder() {

	setupUi(this);

	_path = _settings->get(Set::Engine_SR_Path);
	_is_active = _settings->get(Set::Engine_SR_Active);
	_is_create_session_path = _settings->get(Set::Engine_SR_SessionPath);

	le_path->setText(_path);
	cb_activate->setChecked(_is_active);
	cb_create_session_path->setChecked(_is_create_session_path);

	cb_create_session_path->setEnabled(_is_active);
	btn_path->setEnabled(_is_active);
	le_path->setEnabled(_is_active);

	setModal(true);

	connect(cb_activate, SIGNAL(toggled(bool)), this, SLOT(sl_cb_activate_toggled(bool)));
	connect(cb_create_session_path, SIGNAL(toggled(bool)), this, SLOT(sl_cb_create_session_path_toggled(bool)));
	connect(btn_path, SIGNAL(clicked()), this, SLOT(sl_btn_path_clicked()));
	connect(btn_ok, SIGNAL(clicked()), this, SLOT(sl_ok()));

    hide();
}


GUI_StreamRecorder::~GUI_StreamRecorder() {

}


void GUI_StreamRecorder::language_changed() {

	retranslateUi(this);
}


void GUI_StreamRecorder::sl_cb_activate_toggled(bool b) {
	_is_active = b;
	_settings->set(Set::Engine_SR_Active, b);
}

void GUI_StreamRecorder::sl_cb_create_session_path_toggled(bool b) {
    _is_create_session_path = b;
	_settings->set(Set::Engine_SR_SessionPath, b);
}

void GUI_StreamRecorder::sl_btn_path_clicked() {

	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose target directory"), _path, QFileDialog::ShowDirsOnly);
	if(dir.size() > 0) {
		_path = dir;
		_settings->set(Set::Engine_SR_Path, _path);
		le_path->setText(_path);
	}
}


void GUI_StreamRecorder::sl_ok() {

	QString str = le_path->text();
    if(!QFile::exists(str)) {
        bool create_success = QDir::root().mkpath(str);
        if(!create_success) {
			QString sr_path = _settings->get(Set::Engine_SR_Path);
			le_path->setText(sr_path);

			QMessageBox::warning(this, tr("Could not create directory"), str + tr(" could not be created\nPlease choose another folder"));

            return;
        }
    }

	_settings->set(Set::Engine_SR_Path, str);
	_settings->set(Set::Engine_SR_Active, cb_activate->isChecked());
    _path = str;

    hide();
    close();
}


void GUI_StreamRecorder::record_button_toggled(bool b) {

	btn_path->setEnabled(!b);
	cb_activate->setEnabled(!b);
	cb_create_session_path->setEnabled(!b);
	le_path->setEnabled(!b);
}
