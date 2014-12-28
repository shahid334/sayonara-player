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
#include "StreamPlugins/LastFM/LastFM.h"

#include <QMessageBox>


GUI_LastFM::GUI_LastFM(QWidget* parent) :
	SayonaraDialog(parent),
	Ui_GUI_LastFM_Dialog() {

	init();

}


GUI_LastFM::GUI_LastFM(QWidget* parent, QString username, QString password) :
	SayonaraDialog(parent),
	Ui_GUI_LastFM_Dialog() {

	setupUi(this);

    lab_image->setPixmap(Helper::getPixmap("lastfm_logo.jpg"));
	tf_username->setText(username);
	tf_username->setText(password);

	init();
}

void GUI_LastFM::init(){
	setupUi(this);

	bool checked;
	bool enabled = _settings->get(Set::LFM_Active);

	cb_activate->setChecked(enabled);
	setLFMActive(enabled);

    lab_image->setPixmap(Helper::getPixmap("lastfm_logo.jpg"));

	checked = _settings->get(Set::LFM_Corrections);
	cb_correct_id3->setChecked(checked);

	checked = _settings->get(Set::LFM_ShowErrors);
	cb_error_messages->setChecked(checked);

	connect(btn_save, SIGNAL(clicked()), this, SLOT(save_button_pressed()));
	connect(btn_clear_session, SIGNAL(clicked()), this, SLOT(clear_session_pressed()));
	connect(cb_correct_id3, SIGNAL(toggled(bool)), this, SLOT(cb_correct_id3_toggled(bool)));
	connect(cb_activate, SIGNAL(toggled(bool)), this, SLOT(cb_activate_toggled(bool)));
	hide();
}


GUI_LastFM::~GUI_LastFM() {

}

void GUI_LastFM::changeSkin(bool dark) {

    _skin = dark;
}

void GUI_LastFM::language_changed() {

	retranslateUi(this);
}

void GUI_LastFM::clear_session_pressed() {

}


void GUI_LastFM::save_button_pressed() {

	if(tf_username->text().length() < 3) return;
	if(tf_password->text().length() < 3) return;

	QString user, password;
	LastFM::get_login(user, password);

	if (tf_password->text() != password) {

		QStringList user_pw;
		user_pw << tf_username->text() << tf_password->text();
		_settings->set(Set::LFM_Login, user_pw);
    }

	_settings->set( Set::LFM_Active, cb_activate->isChecked() );
	_settings->set( Set::LFM_Corrections, cb_correct_id3->isChecked() );
	_settings->set( Set::LFM_ShowErrors, cb_error_messages->isChecked() );

    hide();
    close();
}


void GUI_LastFM::show_win() {

	QString user, password;
	LastFM::get_login(user, password);

	if (user.size() > 0) {
		tf_username->setText(user);
		tf_password->setText(password);
	}

#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    else {
		tf_username->setPlaceholderText("Enter Usename");
		tf_password->setPlaceholderText("Enter Password");
    }

#endif
    changeSkin(_skin);
	show();
}


void GUI_LastFM::cb_correct_id3_toggled(bool checked) {

	_settings->set(Set::LFM_Corrections, checked);
}

void GUI_LastFM::setLFMActive(bool enabled) {
	tf_username->setEnabled(enabled);
	tf_password->setEnabled(enabled);
	cb_correct_id3->setEnabled(enabled);
}

void GUI_LastFM::cb_activate_toggled(bool b) {
	setLFMActive(b);

}
