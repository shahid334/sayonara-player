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


#include "GUI/LastFM/GUI_LFMRadioWidget.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "StreamPlugins/LastFM/LastFM.h"

#include <QWidget>
#include <QPixmap>

#include "GUI/ui_GUI_LFMRadioWidget.h"
#include "PlayerPlugin/PlayerPlugin.h"

GUI_LFMRadioWidget::GUI_LFMRadioWidget(QString name, QString action_text, QWidget *parent) : PlayerPlugin(name, action_text, parent) {


    this->_ui = new Ui::GUI_LFMRadioWidget();
	this->_ui->setupUi(this);
	this->_ui->cb_friends->setVisible(false);

    QPixmap p = QPixmap(Helper::getIconPath() + "lastfm_red_small.png");
	this->_ui->lab_lfm->setPixmap(p);

	_ui->btn_listen->setIcon(QIcon(Helper::getIconPath() + "play.png"));

    connect(_ui->combo_mode, SIGNAL(currentIndexChanged(int)), SLOT(mode_index_changed(int)));
	connect(_ui->btn_listen, SIGNAL(released()), this, SLOT(start_listen()));

    hide();
}


GUI_LFMRadioWidget::~GUI_LFMRadioWidget() {
	// TODO Auto-generated destructor stub
}

QAction* GUI_LFMRadioWidget::getAction(){
    PlayerPlugin::calc_action(this->getVisName());
    return _pp_action;
}

void GUI_LFMRadioWidget::changeSkin(bool dark){

}

void GUI_LFMRadioWidget::language_changed(){
    this->_ui->retranslateUi(this);
}

void GUI_LFMRadioWidget::start_listen(){



	bool text_input = false;
	int lfm_listen_mode = LFM_RADIO_MODE_TAG;

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
