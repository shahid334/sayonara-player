/* GUI_Equalizer.cpp */

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
 * GUI_Equalizer.cpp
 *
 *  Created on: May 18, 2011
 *      Author: luke
 */
#define SCALE(x) (_m * (x) + _t)


#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/equalizer/GUI_Equalizer.h"
#include "GUI/ui_GUI_Equalizer.h"

#include <QObject>
#include <QDockWidget>
#include <QDebug>
#include <QMessageBox>


#include <vector>

using namespace std;

QString calc_lab(int val){
    if(val > 0){
        double v = val / 2.0;
        if(val % 2 == 0)
            return QString("+") + QString::number(v) + ".0";
        else
            return QString("+") + QString::number(v);
    }

    return QString::number(val) + ".0";

}

GUI_Equalizer::GUI_Equalizer(QString name, QString action_text, QWidget *parent) : PlayerPlugin(name, action_text, parent) {

	_settings = CSettingsStorage::getInstance();
	_ui = new Ui::GUI_Equalizer( );
	_ui->setupUi(this);

	_ui->btn_preset->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	_ui->btn_preset->setText("");

	_sliders.push_back(new EqSlider(_ui->sli_0, _ui->label, 0));
	_sliders.push_back(new EqSlider(_ui->sli_1, _ui->label_2, 1));
	_sliders.push_back(new EqSlider(_ui->sli_2, _ui->label_3, 2));
	_sliders.push_back(new EqSlider(_ui->sli_3, _ui->label_4, 3));
	_sliders.push_back(new EqSlider(_ui->sli_4, _ui->label_5, 4));
	_sliders.push_back(new EqSlider(_ui->sli_5, _ui->label_6, 5));
	_sliders.push_back(new EqSlider(_ui->sli_6, _ui->label_7, 6));
	_sliders.push_back(new EqSlider(_ui->sli_7, _ui->label_8, 7));
	_sliders.push_back(new EqSlider(_ui->sli_8, _ui->label_9, 8));
	_sliders.push_back(new EqSlider(_ui->sli_9, _ui->label_10, 9));

	foreach(EqSlider* s, _sliders){
		connect(s, SIGNAL(valueChanged(int,int)), this, SLOT(sli_changed(int, int)));
	}

	connect(_ui->btn_preset, SIGNAL(clicked()), this, SLOT(btn_preset_clicked()));

	this->fill_eq_presets();

	hide();
    _m = (100.0 / 48.0);
    _t = 50;

}

GUI_Equalizer::~GUI_Equalizer() {

	foreach(EqSlider* s, _sliders){
		delete s;
	}

	delete _ui;
}


QAction* GUI_Equalizer::getAction(){
    PlayerPlugin::calc_action(this->getVisName());
    return _pp_action;
}

void GUI_Equalizer::language_changed(){
    _ui->retranslateUi(this);
}

void GUI_Equalizer::changeSkin(bool dark){

    _dark = dark;
}



void GUI_Equalizer::sli_changed(int idx, int new_val){
	_sliders[idx]->getLabel()->setText(calc_lab(new_val));
	emit eq_changed_signal(idx, new_val);
}


void GUI_Equalizer::but_enabled_changed(bool enabled){

}


void GUI_Equalizer::fill_eq_presets(){

	QStringList items;
	_settings->getEqualizerSettings(_presets);

	foreach(EQ_Setting s, _presets){
		items << s.name;
	}

	_ui->combo_presets->insertItems(0, items);

	int last_idx = _settings->getLastEqualizer();
	if(last_idx < (int) _presets.size() ){
		_ui->combo_presets->setCurrentIndex(last_idx);
		preset_changed(last_idx);
	}

	connect(_ui->combo_presets, SIGNAL(currentIndexChanged(int)), this, SLOT(preset_changed(int)));
}


void GUI_Equalizer::fill_available_equalizers(const QStringList& eqs){
	Q_UNUSED(eqs);
}


void GUI_Equalizer::preset_changed(int index){

	QList<double> setting = this->_presets[index].settings;

	for(int i=0; i<setting.size(); i++){
		if(i > (int) _sliders.size()) break;

		_sliders[i]->setValue( setting[i] );
	}


	CSettingsStorage::getInstance()->setLastEqualizer(index);
}


void GUI_Equalizer::btn_preset_clicked(){

	QString str = "Custom";
	foreach(EqSlider* s, _sliders){
		str += "," + s->getLabel()->text();
	}

	int custom_idx = -1;
	for(uint i=0; i<_presets.size(); i++){

		if(_presets[i].name == "Custom"){
			_presets[i].parseFromString(str);
			custom_idx = i;
			break;
		}
	}

	bool b_save = true;
	int current_idx = _ui->combo_presets->currentIndex();

	if( custom_idx != -1 && custom_idx != current_idx ){

        QMessageBox msgBox(this);
         msgBox.setText(tr("This will overwrite your custom preset"));
         msgBox.setInformativeText(tr("Continue?"));
		 msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		 msgBox.setDefaultButton(QMessageBox::No);
         Helper::set_deja_vu_font(&msgBox);
		 int ret = msgBox.exec();

		 if(ret != QMessageBox::Yes) b_save = false;
	}

	if(b_save){
		CSettingsStorage::getInstance()->setEqualizerSettings(_presets);
		if(custom_idx != -1) _ui->combo_presets->setCurrentIndex(custom_idx);
	}
}


