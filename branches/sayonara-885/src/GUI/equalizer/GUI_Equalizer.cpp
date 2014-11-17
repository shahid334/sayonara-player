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
#include "Settings/Settings.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/equalizer/GUI_Equalizer.h"
#include "GUI/ui_GUI_Equalizer.h"

#include <QDockWidget>
#include <QMessageBox>


QString calc_lab(int val) {
    if(val > 0) {
        double v = val / 2.0;
        if(val % 2 == 0)
            return QString("+") + QString::number(v) + ".0";
        else
            return QString("+") + QString::number(v);
    }

    return QString::number(val) + ".0";

}

GUI_Equalizer::GUI_Equalizer(QString name, QWidget *parent) :
	PlayerPlugin(name, parent),
	Ui::GUI_Equalizer(){

	_active_idx = -1;

	_settings = Settings::getInstance();
	setupUi(this);

    btn_preset->setIcon(Helper::getIcon("save.png"));
	btn_preset->setText("");

	_sliders.push_back(new EqSlider(sli_0, label, 0));
	_sliders.push_back(new EqSlider(sli_1, label_2, 1));
	_sliders.push_back(new EqSlider(sli_2, label_3, 2));
	_sliders.push_back(new EqSlider(sli_3, label_4, 3));
	_sliders.push_back(new EqSlider(sli_4, label_5, 4));
	_sliders.push_back(new EqSlider(sli_5, label_6, 5));
	_sliders.push_back(new EqSlider(sli_6, label_7, 6));
	_sliders.push_back(new EqSlider(sli_7, label_8, 7));
	_sliders.push_back(new EqSlider(sli_8, label_9, 8));
	_sliders.push_back(new EqSlider(sli_9, label_10, 9));

	foreach(EqSlider* s, _sliders) {
		connect(s, SIGNAL(sig_value_changed(int,int)), this, SLOT(sli_changed(int, int)));
		connect(s, SIGNAL(sig_slider_pressed(int)), this, SLOT(sli_pressed(int)));
		connect(s, SIGNAL(sig_slider_released(int)), this, SLOT(sli_released(int)));
	}

	connect(btn_preset, SIGNAL(clicked()), this, SLOT(btn_preset_clicked()));

	fill_eq_presets();

	hide();
    _m = (100.0 / 48.0);
    _t = 50;

}

GUI_Equalizer::~GUI_Equalizer() {

	foreach(EqSlider* s, _sliders) {
		delete s;
	}
}



void GUI_Equalizer::language_changed() {
	retranslateUi(this);
}

void GUI_Equalizer::changeSkin(bool dark) {

    _dark = dark;
}

void GUI_Equalizer::sli_pressed(int idx){
	_active_idx= idx;
	for(int i=0; i<_sliders.size(); i++){
		_old_val[i] = _sliders[i]->getValue();
	}
}


void GUI_Equalizer::sli_released(int idx){
	_active_idx = -1;
}


static double scale[] = {1.0, 0.6, 0.20, 0.06, 0.01};

void GUI_Equalizer::sli_changed(int idx, int new_val) {
	EqSlider* s = _sliders[idx];
	s->getLabel()->setText(calc_lab(new_val));

	emit eq_changed_signal(idx, new_val);

	// this slider has been changed actively
	if( idx == _active_idx && cb_sinus->isChecked() ){
		int delta = new_val - _old_val[idx];

		for(int i=idx-9; i<idx+9; i++){
			if(i < 0) continue;
			if(i == idx) continue;
			if(i >= _sliders.size()) break;

			// how far is the slider away from me?
			int x = abs(_active_idx - i);

			if(x > 4) continue;

			double new_val = _old_val[i] + (delta * scale[x]);

			_sliders[i]->setValue(new_val);
		}
	}
}


void GUI_Equalizer::but_enabled_changed(bool enabled) {

}


void GUI_Equalizer::fill_eq_presets() {

	QStringList items;

	int last_idx = _settings->get(Set::Eq_Last);

	_presets << _settings->get(Set::Eq_Flat);
	_presets << _settings->get(Set::Eq_Rock);
	_presets << _settings->get(Set::Eq_LightRock);
	_presets << _settings->get(Set::Eq_Bass);
	_presets << _settings->get(Set::Eq_Mid);
	_presets << _settings->get(Set::Eq_Treble);
	_presets << _settings->get(Set::Eq_Custom);

	foreach(EQ_Setting s, _presets) {
		items << s.name;
	}

	combo_presets->insertItems(0, items);

	if(last_idx < (int) _presets.size() ) {
		combo_presets->setCurrentIndex(last_idx);
		preset_changed(last_idx);
	}

	connect(combo_presets, SIGNAL(currentIndexChanged(int)), this, SLOT(preset_changed(int)));
}


void GUI_Equalizer::fill_available_equalizers(const QStringList& eqs) {
	Q_UNUSED(eqs);
}


void GUI_Equalizer::preset_changed(int index) {

	QList<double> setting = _presets[index].settings;

	for(int i=0; i<setting.size(); i++) {
		if(i > (int) _sliders.size()) break;

		_sliders[i]->setValue( setting[i] );
		_old_val[i] = setting[i];
	}

	Settings::getInstance()->set(Set::Eq_Last, index);
}


void GUI_Equalizer::btn_preset_clicked() {

    bool b_save = true;
    int current_idx = combo_presets->currentIndex();

	QString str = "Custom";

	foreach(EqSlider* s, _sliders) {
		str += "," + s->getLabel()->text();
	}

	int custom_idx = -1;
	for(int i=0; i<_presets.size(); i++) {

		if(_presets[i].name == "Custom") {
            _presets[i] = EQ_Setting::fromString(str);
			custom_idx = i;
			break;
		}
	}

	if( custom_idx != -1 && custom_idx != current_idx ) {

        QMessageBox msgBox(this);
         msgBox.setText(tr("This will overwrite your custom preset"));
         msgBox.setInformativeText(tr("Continue?"));
		 msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		 msgBox.setDefaultButton(QMessageBox::No);
         Helper::set_deja_vu_font(&msgBox);
		 int ret = msgBox.exec();

		 if(ret != QMessageBox::Yes) b_save = false;
	}

	if(b_save) {

		_settings->set(Set::Eq_Custom, _presets[6]);

        if(custom_idx != -1){
            combo_presets->setCurrentIndex(custom_idx);
        }
	}
}


