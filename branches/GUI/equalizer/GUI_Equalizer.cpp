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
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/equalizer/GUI_Equalizer.h"

#include <QObject>
#include <QDockWidget>
#include <QDebug>
#include <QMessageBox>

#include <ui_GUI_Equalizer.h>

#include <vector>

using namespace std;

GUI_Equalizer::GUI_Equalizer(QWidget* parent) : QDockWidget(parent) {

    this->_ui = new Ui::GUI_Equalizer( );
	this->_ui->setupUi(this);

	this->_ui->btn_preset->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	this->_ui->btn_preset->setText("");

	connect(this->_ui->sli_0, SIGNAL(valueChanged(int)), this, SLOT(sli_0_changed(int)));
	connect(this->_ui->sli_1, SIGNAL(valueChanged(int)), this, SLOT(sli_1_changed(int)));
	connect(this->_ui->sli_2, SIGNAL(valueChanged(int)), this, SLOT(sli_2_changed(int)));
	connect(this->_ui->sli_3, SIGNAL(valueChanged(int)), this, SLOT(sli_3_changed(int)));
	connect(this->_ui->sli_4, SIGNAL(valueChanged(int)), this, SLOT(sli_4_changed(int)));
	connect(this->_ui->sli_5, SIGNAL(valueChanged(int)), this, SLOT(sli_5_changed(int)));
	connect(this->_ui->sli_6, SIGNAL(valueChanged(int)), this, SLOT(sli_6_changed(int)));
	connect(this->_ui->sli_7, SIGNAL(valueChanged(int)), this, SLOT(sli_7_changed(int)));
	connect(this->_ui->sli_8, SIGNAL(valueChanged(int)), this, SLOT(sli_8_changed(int)));
	connect(this->_ui->sli_9, SIGNAL(valueChanged(int)), this, SLOT(sli_9_changed(int)));


	connect(this->_ui->btn_preset, SIGNAL(clicked()), this, SLOT(btn_preset_clicked()));

    hide();

}

GUI_Equalizer::~GUI_Equalizer() {

	delete _ui;
}


void GUI_Equalizer::changeSkin(bool dark){

    this->_ui->combo_presets->setStyleSheet(Style::get_combobox_style(dark));
    QString sli_style = Style::get_v_slider_style(dark);
    this->_ui->sli_0->setStyleSheet(sli_style);
    this->_ui->sli_1->setStyleSheet(sli_style);
    this->_ui->sli_2->setStyleSheet(sli_style);
    this->_ui->sli_3->setStyleSheet(sli_style);
    this->_ui->sli_4->setStyleSheet(sli_style);
    this->_ui->sli_5->setStyleSheet(sli_style);
    this->_ui->sli_6->setStyleSheet(sli_style);
    this->_ui->sli_7->setStyleSheet(sli_style);
    this->_ui->sli_8->setStyleSheet(sli_style);
    this->_ui->sli_9->setStyleSheet(sli_style);

    this->_ui->btn_preset->setStyleSheet(Style::get_btn_style(dark));
}



void GUI_Equalizer::sli_0_changed(int new_val){
	this->_ui->label->setText(QString::number(new_val));
	emit eq_changed_signal(0, new_val);
}
void GUI_Equalizer::sli_1_changed(int new_val){
	this->_ui->label_2->setText(QString::number(new_val));
	emit eq_changed_signal(1, new_val);
}
void GUI_Equalizer::sli_2_changed(int new_val){
	this->_ui->label_3->setText(QString::number(new_val));
	emit eq_changed_signal(2, new_val);
}
void GUI_Equalizer::sli_3_changed(int new_val){
	this->_ui->label_4->setText(QString::number(new_val));
	emit eq_changed_signal(3, new_val);
}
void GUI_Equalizer::sli_4_changed(int new_val){
	this->_ui->label_5->setText(QString::number(new_val));
	emit eq_changed_signal(4, new_val);
}
void GUI_Equalizer::sli_5_changed(int new_val){
	this->_ui->label_6->setText(QString::number(new_val));
	emit eq_changed_signal(5, new_val);
}
void GUI_Equalizer::sli_6_changed(int new_val){
	this->_ui->label_7->setText(QString::number(new_val));
	emit eq_changed_signal(6, new_val);
}
void GUI_Equalizer::sli_7_changed(int new_val){
	this->_ui->label_8->setText(QString::number(new_val));
	emit eq_changed_signal(7, new_val);
}
void GUI_Equalizer::sli_8_changed(int new_val){
	this->_ui->label_9->setText(QString::number(new_val));
	emit eq_changed_signal(8, new_val);
}
void GUI_Equalizer::sli_9_changed(int new_val){
	this->_ui->label_10->setText(QString::number(new_val));
	emit eq_changed_signal(9, new_val);
}

void GUI_Equalizer::but_enabled_changed(bool enabled){

}


void GUI_Equalizer::fill_eq_presets(const vector<EQ_Setting>& presets){

	_presets = presets;

	for(uint i=0; i<presets.size(); i++){

		this->_ui->combo_presets->insertItem(i, presets[i].name);

	}

	int last_idx = CSettingsStorage::getInstance()->getLastEqualizer();
	if(last_idx < (int) presets.size() ){

		this->_ui->combo_presets->setCurrentIndex(last_idx);
		preset_changed(last_idx);
	}


	connect(this->_ui->combo_presets, SIGNAL(currentIndexChanged(int)), this, SLOT(preset_changed(int)));
}


void GUI_Equalizer::fill_available_equalizers(const QStringList& eqs){
	Q_UNUSED(eqs);
}


void GUI_Equalizer::preset_changed(int index){

	QList<double> setting = this->_presets[index].settings;

	for(int i=0; i<setting.size(); i++){
		int val = (int) setting[i];
		if( i==0) this->_ui->sli_0->setValue(val);
		else if(i == 1) this->_ui->sli_1->setValue(val);
		else if(i == 2) this->_ui->sli_2->setValue(val);
		else if(i == 3) this->_ui->sli_3->setValue(val);
		else if(i == 4) this->_ui->sli_4->setValue(val);
		else if(i == 5) this->_ui->sli_5->setValue(val);
		else if(i == 6) this->_ui->sli_6->setValue(val);
		else if(i == 7) this->_ui->sli_7->setValue(val);
		else if(i == 8) this->_ui->sli_8->setValue(val);
		else if(i == 9) this->_ui->sli_9->setValue(val);

		emit eq_changed_signal(i, val);
	}


	CSettingsStorage::getInstance()->setLastEqualizer(index);

}


void GUI_Equalizer::btn_preset_clicked(){
	QString str = "Custom,";
	str += _ui->label->text() + ",";
	str += _ui->label_2->text() + ",";
	str += _ui->label_3->text() + ",";
	str += _ui->label_4->text() + ",";
	str += _ui->label_5->text() + ",";
	str += _ui->label_6->text() + ",";
	str += _ui->label_7->text() + ",";
	str += _ui->label_8->text() + ",";
	str += _ui->label_9->text() + ",";
	str += _ui->label_10->text();

	int custom_idx = -1;
	for(uint i=0; i<_presets.size(); i++){

		if(_presets[i].name == "Custom"){
			_presets[i].parseFromString(str);
			custom_idx = i;
			break;
		}
	}

	bool b_save = true;
	int current_idx = this->_ui->combo_presets->currentIndex();

	if( custom_idx != -1 && custom_idx != current_idx ){
		 QMessageBox msgBox;
		 msgBox.setText("This will overwrite your custom preset");
		 msgBox.setInformativeText("Continue?");
		 msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		 msgBox.setDefaultButton(QMessageBox::No);
		 int ret = msgBox.exec();

		 if(ret != QMessageBox::Yes) b_save = false;
	}

	if(b_save){
		CSettingsStorage::getInstance()->setEqualizerSettings(_presets);
		if(custom_idx != -1) this->_ui->combo_presets->setCurrentIndex(custom_idx);
	}

}

void 	GUI_Equalizer::closeEvent ( QCloseEvent * event ){
	event->ignore();
    hide();
    close();
	emit close_event();
}
