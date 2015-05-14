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


#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/Helper.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/equalizer/GUI_Equalizer.h"
#include "GUI/ui_GUI_Equalizer.h"


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
	Ui::GUI_Equalizer()
{
	_active_idx = -1;

	setupUi(this);

	sli_0->setData(0, label);
	sli_1->setData(1, label_2);
	sli_2->setData(2, label_3);
	sli_3->setData(3, label_4);
	sli_4->setData(4, label_5);
	sli_5->setData(5, label_6);
	sli_6->setData(6, label_7);
	sli_7->setData(7, label_8);
	sli_8->setData(8, label_9);
	sli_9->setData(9, label_10);

	_sliders.push_back(sli_0);
	_sliders.push_back(sli_1);
	_sliders.push_back(sli_2);
	_sliders.push_back(sli_3);
	_sliders.push_back(sli_4);
	_sliders.push_back(sli_5);
	_sliders.push_back(sli_6);
	_sliders.push_back(sli_7);
	_sliders.push_back(sli_8);
	_sliders.push_back(sli_9);

    cb_gauss->setChecked( _settings->get(Set::Eq_Gauss));

	foreach(EqSlider* s, _sliders) {
		connect(s, SIGNAL(sig_value_changed(int,int)), this, SLOT(sli_changed(int, int)));
		connect(s, SIGNAL(sig_slider_got_focus(int)), this, SLOT(sli_pressed(int)));
		connect(s, SIGNAL(sig_slider_lost_focus(int)), this, SLOT(sli_released(int)));
	}

	connect(btn_tool, SIGNAL(sig_save()), this, SLOT(btn_save_clicked()));
	connect(btn_tool, SIGNAL(sig_delete()), this, SLOT(btn_delete_clicked()));
	connect(btn_tool, SIGNAL(sig_undo()), this, SLOT(btn_reset_clicked()));
    connect(cb_gauss, SIGNAL(toggled(bool)), this, SLOT(cb_gauss_toggled(bool)));
	connect(combo_presets, SIGNAL(editTextChanged(QString)), this, SLOT(text_changed(QString)));

	fill_eq_presets();

	hide();
}

GUI_Equalizer::~GUI_Equalizer() {

	foreach(EqSlider* s, _sliders) {
		delete s;
	}
}



void GUI_Equalizer::language_changed() {
	retranslateUi(this);
}

int GUI_Equalizer::find_combo_text(QString text){
	int ret = -1;

	for(int i=0; i<combo_presets->count(); i++){
		if(combo_presets->itemText(i).compare(text, Qt::CaseInsensitive) == 0){
			ret = i;
		}
	}
	return ret;
}


void GUI_Equalizer::sli_pressed(int idx){
	_active_idx= idx;
	for(int i=0; i<_sliders.size(); i++){
		_old_val[i] = _sliders[i]->value();
	}
}


void GUI_Equalizer::sli_released(int idx){
	_active_idx = -1;
}


static double scale[] = {1.0, 0.6, 0.20, 0.06, 0.01};

void GUI_Equalizer::sli_changed(int idx, int new_val) {

	btn_tool->show_undo(true);

	EqSlider* s = _sliders[idx];
	s->getLabel()->setText(calc_lab(new_val));

	emit sig_eq_changed(idx, new_val);

	// this slider has been changed actively
    if( idx == _active_idx && cb_gauss->isChecked() ){
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


void GUI_Equalizer::fill_eq_presets() {

	QStringList items;
	int last_idx;

	last_idx = _settings->get(Set::Eq_Last);
    _presets = _settings->get(Set::Eq_List);

	items << "";

	foreach(EQ_Setting s, _presets) {
		items << s.name;
	}

	combo_presets->insertItems(0, items);

	btn_tool->show_save(combo_presets->currentText().size() > 0);
	btn_tool->show_delete(combo_presets->currentIndex() > 0);

	connect(combo_presets, SIGNAL(currentIndexChanged(int)), this, SLOT(preset_changed(int)));

	if(last_idx < _presets.size() && last_idx > 0 ) {
		combo_presets->setCurrentIndex(last_idx);
	}

}


void GUI_Equalizer::preset_changed(int index) {


	btn_tool->show_delete(index > 0);

	if(index == 0) return;
	if(index > _presets.size()) return;

	btn_tool->show_undo(false);

	QList<double> setting = _presets[index - 1].settings;

	for(int i=0; i<setting.size(); i++) {
		if(i > _sliders.size()) break;

		_sliders[i]->setValue( setting[i] );
		_old_val[i] = setting[i];
	}

    _settings->set(Set::Eq_Last, index);
}



void GUI_Equalizer::cb_gauss_toggled(bool b){
    _settings->set(Set::Eq_Gauss, b);
}



void GUI_Equalizer::btn_save_clicked() {

	QString text = combo_presets->currentText();

	int found_idx = find_combo_text(text);

	if(found_idx <= 0){
		EQ_Setting s = EQ_Setting::fromString(text + ":0:0:0:0:0:0:0:0:0:0");
		_presets << s;
		combo_presets->addItem(text);
		found_idx = combo_presets->count() - 1;
	}

	for(int i=0; i<_sliders.size(); i++){
		_presets[found_idx - 1].settings[i] = _sliders[i]->value();
	}

	_settings->set(Set::Eq_List, _presets);


	combo_presets->setCurrentIndex(found_idx);
}

void GUI_Equalizer::btn_delete_clicked(){

	btn_tool->show_undo(false);
	int idx = combo_presets->currentIndex();

	combo_presets->setCurrentIndex(0);

	_presets.removeAt(idx - 1);
	combo_presets->removeItem(idx);

	_settings->set(Set::Eq_List, _presets);
}

void GUI_Equalizer::btn_reset_clicked(){

	btn_tool->show_undo(false);
	QString text = combo_presets->currentText();

	int found_idx = find_combo_text(text);

	if(found_idx <= 0){
		foreach(EqSlider* sli, _sliders){
			sli->setValue(0);
		}
	}

	else{
		for(int i=0; i<_sliders.size(); i++){
			_sliders[i]->setValue( _presets[found_idx - 1].settings[i] );
		}
	}
}


void GUI_Equalizer::text_changed(QString str){
	btn_tool->show_save(str.size() > 0);
}
