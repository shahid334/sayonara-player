/*
 * GUI_LastFM.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: luke
 */

#include "GUI/LastFM/GUI_LastFM.h"
#include <ui_GUI_LastFM_Widget.h>
#include <HelperStructs/Helper.h>

#include <QString>
#include <QDebug>
#include <QCryptographicHash>

#include <iostream>
#include <CSettingsStorage.h>


using namespace std;



GUI_LastFM::GUI_LastFM() {
	this->ui = new Ui_GUI_LastFM_Widget();
	this->ui->setupUi(this);

	this->ui->lab_image->setPixmap(QPixmap::fromImage(QImage(Helper::getIconPath() + "lastfm_logo.jpg")));

	connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_button_pressed()));

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


	QByteArray password_hashed = QCryptographicHash::hash(this->ui->tf_password->text().toUtf8(), QCryptographicHash::Md5).toHex();
	for(int i=0; i<password_hashed.length(); i++){
		c_buffer[i] = password_hashed.at(i);
	}

	c_buffer[32] = '\0';

	emit new_lfm_credentials(this->ui->tf_username->text(), QString(c_buffer));
        CSettingsStorage::getInstance()->setLastFMNameAndPW(this->ui->tf_username->text(),QString(c_buffer));
	this->close();
}
void GUI_LastFM::show_win(){

	this->show();
}
