/* GUI_RadioWidget.cpp */

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
 * GUIRadioWidget.cpp
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#define IDX_ARTIST 0
#define IDX_USER 1
#define IDX_TAG 2
#define IDX_RECOMMENDED 3


#include "GUI/LFMRadio/GUI_LFMRadioWidget.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "StreamPlugins/LastFM/LastFM.h"

#include <QWidget>

#include <QCloseEvent>
#include <QPixmap>

#include "ui_GUI_LFMRadioWidget.h"

GUI_LFMRadioWidget::GUI_LFMRadioWidget(QWidget* parent) : QWidget(parent)  {


    this->_ui = new Ui::GUI_LFMRadioWidget();
	this->_ui->setupUi(this);
    //this->_ui->rb_artist->setChecked(true);
	this->_ui->cb_friends->setVisible(false);

    QPixmap p = QPixmap(Helper::getIconPath() + "lastfm_red_small.png");
	this->_ui->lab_lfm->setPixmap(p);

	_ui->btn_listen->setIcon(QIcon(Helper::getIconPath() + "play.png"));

    /*connect(_ui->rb_artist, SIGNAL(released()), this, SLOT(radio_button_changed()));
	connect(_ui->rb_tag, SIGNAL(released()), this, SLOT(radio_button_changed()));
	connect(_ui->rb_recom, SIGNAL(released()), this, SLOT(radio_button_changed()));
    connect(_ui->rb_user, SIGNAL(released()), this, SLOT(radio_button_changed()));*/
    connect(_ui->combo_mode, SIGNAL(currentIndexChanged(int)), SLOT(mode_index_changed(int)));
	connect(_ui->btn_listen, SIGNAL(released()), this, SLOT(start_listen()));

    hide();
}


GUI_LFMRadioWidget::~GUI_LFMRadioWidget() {
	// TODO Auto-generated destructor stub
}

void GUI_LFMRadioWidget::changeSkin(bool dark){

}

void GUI_LFMRadioWidget::start_listen(){



	bool text_input = false;
	int lfm_listen_mode = LFM_RADIO_MODE_TAG;

    /*if(_ui->rb_artist->isChecked()) {
		lfm_listen_mode = LFM_RADIO_MODE_ARTIST;
        text_input = true;
	}

	else if(_ui->rb_tag->isChecked()) {
		lfm_listen_mode = LFM_RADIO_MODE_TAG;
		text_input = true;
	}

	else if(_ui->rb_recom->isChecked()) lfm_listen_mode = LFM_RADIO_MODE_RECOMMENDED;
    else if(_ui->rb_user->isChecked()) lfm_listen_mode = LFM_RADIO_MODE_USER_LIBRARY;*/

    switch(_ui->combo_mode->currentIndex()){

    case IDX_ARTIST:
        lfm_listen_mode = LFM_RADIO_MODE_ARTIST;
        text_input = true;
        break;

    case IDX_TAG:
       lfm_listen_mode = LFM_RADIO_MODE_TAG;
       text_input = true;
        break;

    case IDX_USER:
      lfm_listen_mode = LFM_RADIO_MODE_USER_LIBRARY;
        break;

    case IDX_RECOMMENDED:
       lfm_listen_mode = LFM_RADIO_MODE_RECOMMENDED;
        break;

    default: break;

    }


	QString text;

	if(text_input)
		text = _ui->le_text->text();

	else
		text = _ui->cb_friends->currentText();

    if(text.size() >= 3){
        qDebug() << "Listen to " << lfm_listen_mode;
        emit listen_clicked(text, lfm_listen_mode);

    }
}

void GUI_LFMRadioWidget:: mode_index_changed(int i){
    if(_friends.size() == 0){
        LastFM::getInstance()->lfm_get_friends(_friends);
        _ui->cb_friends->addItems(_friends);
    }

    switch(i){
        case IDX_ARTIST:
            _ui->cb_friends->setVisible(false);
            _ui->le_text->setVisible(true);
            break;

        case IDX_TAG:
            _ui->cb_friends->setVisible(false);
            _ui->le_text->setVisible(true);
            break;

        case IDX_USER:
            _ui->cb_friends->setVisible(true);
            _ui->le_text->setVisible(false);
            break;

        case IDX_RECOMMENDED:
            _ui->cb_friends->setVisible(true);
            _ui->le_text->setVisible(false);
            break;

        default: break;
    }

}


void GUI_LFMRadioWidget::radio_button_changed(){
/*
	if(_friends.size() == 0){
		LastFM::getInstance()->lfm_get_friends(_friends);
		_ui->cb_friends->addItems(_friends);
	}


	if(_ui->rb_artist->isChecked()){
		_ui->cb_friends->setVisible(false);
		_ui->le_text->setVisible(true);

		_ui->rb_tag->setChecked(false);
		_ui->rb_user->setChecked(false);
		_ui->rb_recom->setChecked(false);
	}

	else if(_ui->rb_tag->isChecked()){
		_ui->cb_friends->setVisible(false);
		_ui->le_text->setVisible(true);

		_ui->rb_artist->setChecked(false);
		_ui->rb_recom->setChecked(false);
		_ui->rb_user->setChecked(false);
	}


	else if(_ui->rb_recom->isChecked()){
		_ui->cb_friends->setVisible(true);
		_ui->le_text->setVisible(false);

		_ui->rb_artist->setChecked(false);
		_ui->rb_user->setChecked(false);
		_ui->rb_tag->setChecked(false);
	}

	else if(_ui->rb_user->isChecked()){
		_ui->cb_friends->setVisible(true);
		_ui->le_text->setVisible(false);

		_ui->rb_artist->setChecked(false);
		_ui->rb_recom->setChecked(false);
		_ui->rb_tag->setChecked(false);
    }*/
}

void GUI_LFMRadioWidget::closeEvent ( QCloseEvent * event ){

    emit close_event();

    event->ignore();
    close();

}
