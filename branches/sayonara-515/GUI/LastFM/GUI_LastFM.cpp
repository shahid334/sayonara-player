/* GUI_LastFM.cpp */

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
 * GUI_LastFM.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: luke
 */

#include "GUI/LastFM/GUI_LastFM.h"
#include "GUI/ui_GUI_LastFM_Dialog.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/CSettingsStorage.h"
#include <QMessageBox>

#include <QString>
#include <QCryptographicHash>


#include <iostream>

using namespace std;

GUI_LastFM::GUI_LastFM(QWidget* parent) : QDialog(parent) {

    ui = 0;

    hide();
}


GUI_LastFM::GUI_LastFM(QString username, QString password){

	this->ui = new Ui_GUI_LastFM_Dialog();
	this->ui->setupUi(this);

	this->ui->lab_image->setPixmap(QPixmap::fromImage(QImage(Helper::getIconPath() + "lastfm_logo.jpg")));

	this->ui->tf_username->setText(username);
	this->ui->tf_username->setText(password);


}


GUI_LastFM::~GUI_LastFM() {
	// TODO Auto-generated destructor stub
}

void GUI_LastFM::changeSkin(bool dark){

    _skin = dark;
}

void GUI_LastFM::clear_session_pressed(){

  CSettingsStorage* settings = CSettingsStorage::getInstance();
  int ret = QMessageBox::warning(this, "Warning", "If you click yes, you will have to authorize Sayonara again on Last.FM<br /><br />Only use this, if Last.FM does not work as expected!<br /><br />Continue?", QMessageBox::Yes, QMessageBox::No);
  if(ret == QMessageBox::Yes) settings->setLastFMSessionKey("");
}


void GUI_LastFM::save_button_pressed(){

	if(this->ui->tf_username->text().length() < 3) return;
	if(this->ui->tf_password->text().length() < 3) return;
    CSettingsStorage* settings = CSettingsStorage::getInstance();

    QString user, password;
    settings-> getLastFMNameAndPW(user, password);

    if (this->ui->tf_password->text() != password){

        emit new_lfm_credentials(this->ui->tf_username->text(), this->ui->tf_password->text());
        settings->setLastFMNameAndPW(this->ui->tf_username->text(), this->ui->tf_password->text());
    }

    settings->setLastFMShowErrors(this->ui->cb_error_messages->isChecked());

    hide();
    close();
}


void GUI_LastFM::show_win(){

     CSettingsStorage* settings = CSettingsStorage::getInstance();
     if(!ui){
        this->ui = new Ui_GUI_LastFM_Dialog();
        this->ui->setupUi(this);

        bool enabled = settings->getLastFMActive();
        this->ui->cb_activate->setChecked(enabled);
        setLFMActive(enabled);

        this->ui->lab_image->setPixmap(QPixmap::fromImage(QImage(Helper::getIconPath() + "lastfm_logo.jpg")));
        bool checked = settings->getLastFMCorrections();
        this->ui->cb_correct_id3->setChecked(checked);

        checked = settings->getLastFMShowErrors();
        this->ui->cb_error_messages->setChecked(checked);

        connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_button_pressed()));
        connect(this->ui->btn_clear_session, SIGNAL(clicked()), this, SLOT(clear_session_pressed()));
        connect(this->ui->cb_correct_id3, SIGNAL(toggled(bool)), this, SLOT(cb_correct_id3_toggled(bool)));
        connect(this->ui->cb_activate, SIGNAL(toggled(bool)), this, SLOT(cb_activate_toggled(bool)));
    }


    QString user, password;
    settings -> getLastFMNameAndPW(user, password);
    if (user.size() > 0) {
        this->ui->tf_username->setText(user);
        this->ui->tf_password->setText(password);
    }

#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    else {
        this->ui->tf_username->setPlaceholderText("Enter Usename");
        this->ui->tf_password->setPlaceholderText("Enter Password");
    }

#endif
    changeSkin(_skin);
    this->show();
}


void GUI_LastFM::cb_correct_id3_toggled(bool checked){

	CSettingsStorage::getInstance()->setLastFMCorrections(checked);
}

void GUI_LastFM::setLFMActive(bool enabled){
	this->ui->tf_username->setEnabled(enabled);
	this->ui->tf_password->setEnabled(enabled);
	this->ui->cb_correct_id3->setEnabled(enabled);
}

void GUI_LastFM::cb_activate_toggled(bool b){
	setLFMActive(b);
	CSettingsStorage::getInstance()->setLastFMActive(b);
	emit sig_activated(b);
}
