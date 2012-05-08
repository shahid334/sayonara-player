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
#include "ui_GUI_LastFM_Widget.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"

#include <QString>
#include <QCryptographicHash>

#include <iostream>

using namespace std;

GUI_LastFM::GUI_LastFM() {
	this->ui = new Ui_GUI_LastFM_Widget();
	this->ui->setupUi(this);

	this->ui->lab_image->setPixmap(QPixmap::fromImage(QImage(Helper::getIconPath() + "lastfm_logo.jpg")));
	bool checked = CSettingsStorage::getInstance()->getLastFMCorrections();
	this->ui->cb_correct_id3->setChecked(checked);

	connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_button_pressed()));
	connect(this->ui->cb_correct_id3, SIGNAL(toggled(bool)), this, SLOT(cb_correct_id3_toggled(bool)));



}


GUI_LastFM::GUI_LastFM(QString username, QString password){

	this->ui = new Ui_GUI_LastFM_Widget();
	this->ui->setupUi(this);

	this->ui->lab_image->setPixmap(QPixmap::fromImage(QImage(Helper::getIconPath() + "lastfm_logo.jpg")));

	this->ui->tf_username->setText(username);
	this->ui->tf_username->setText(password);

	connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_button_pressed()));

}


GUI_LastFM::~GUI_LastFM() {
	// TODO Auto-generated destructor stub
}



void GUI_LastFM::save_button_pressed(){

	if(this->ui->tf_username->text().length() < 3) return;
	if(this->ui->tf_password->text().length() < 3) return;

	char c_buffer[33];
	memset(c_buffer, 0, 33);

        QString user, password;
        CSettingsStorage::getInstance() -> getLastFMNameAndPW(user, password);

        if (this->ui->tf_password->text() != password){
            QByteArray password_hashed = QCryptographicHash::hash(this->ui->tf_password->text().toUtf8(), QCryptographicHash::Md5).toHex();
            for(int i=0; i<password_hashed.length(); i++){
                    c_buffer[i] = password_hashed.at(i);
            }

            c_buffer[32] = '\0';
            emit new_lfm_credentials(this->ui->tf_username->text(), QString(c_buffer));
            CSettingsStorage::getInstance()->setLastFMNameAndPW(this->ui->tf_username->text(), QString(c_buffer));
        }
        else {
            emit new_lfm_credentials(this->ui->tf_username->text(), password);
            CSettingsStorage::getInstance()->setLastFMNameAndPW(this->ui->tf_username->text(), password);
        }


	this->close();
}
void GUI_LastFM::show_win(){

    QString user, password;
    CSettingsStorage::getInstance() -> getLastFMNameAndPW(user, password);
    if (user.size() > 0) {
        this->ui->tf_username->setText(user);
        this->ui->tf_password->setText(password);
    }
/*    else {
        this->ui->tf_username->setPlaceholderText("Enter Usename");
        this->ui->tf_password->setPlaceholderText("Enter Password");
    }*/
    this->show();
}


void GUI_LastFM::cb_correct_id3_toggled(bool checked){

	CSettingsStorage::getInstance()->setLastFMCorrections(checked);

}
