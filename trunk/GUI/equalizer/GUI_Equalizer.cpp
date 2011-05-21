/*
 * GUI_Equalizer.cpp
 *
 *  Created on: May 18, 2011
 *      Author: luke
 */


#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "library/CDatabaseConnector.h"
#include "GUI/equalizer/GUI_Equalizer.h"

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>

#include <ui_GUI_equalizer.h>

#include <vector>

using namespace std;

GUI_Equalizer::GUI_Equalizer(QWidget* parent) : QWidget(parent) {

	this->_ui = new Ui_GUI_Equalizer( );
	this->_ui->setupUi(this);

	this->_ui->btn_preset->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	this->_ui->but_enabled->setIcon(QIcon(Helper::getIconPath() + "power_on.png"));
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

	connect(this->_ui->but_enabled, SIGNAL(toggled(bool)), this, SLOT(but_enabled_changed(bool)));
	connect(this->_ui->combo_presets, SIGNAL(currentIndexChanged(int)), this, SLOT(preset_changed(int)));
	connect(this->_ui->btn_preset, SIGNAL(clicked()), this, SLOT(btn_preset_clicked()));

}

GUI_Equalizer::~GUI_Equalizer() {

	// TODO Auto-generated destructor stub
}



void GUI_Equalizer::sli_0_changed(int new_val){
	emit eq_changed_signal(0, new_val);
}
void GUI_Equalizer::sli_1_changed(int new_val){
	emit eq_changed_signal(1, new_val);
}
void GUI_Equalizer::sli_2_changed(int new_val){
	emit eq_changed_signal(2, new_val);
}
void GUI_Equalizer::sli_3_changed(int new_val){
	emit eq_changed_signal(3, new_val);
}
void GUI_Equalizer::sli_4_changed(int new_val){
	emit eq_changed_signal(4, new_val);
}
void GUI_Equalizer::sli_5_changed(int new_val){
	emit eq_changed_signal(5, new_val);
}
void GUI_Equalizer::sli_6_changed(int new_val){
	emit eq_changed_signal(6, new_val);
}
void GUI_Equalizer::sli_7_changed(int new_val){
	emit eq_changed_signal(7, new_val);
}
void GUI_Equalizer::sli_8_changed(int new_val){
	emit eq_changed_signal(8, new_val);
}
void GUI_Equalizer::sli_9_changed(int new_val){
	emit eq_changed_signal(9, new_val);
}

void GUI_Equalizer::but_enabled_changed(bool enabled){

	if(enabled)this->_ui->but_enabled->setIcon(QIcon(Helper::getIconPath() + "power_on.png"));
	else this->_ui->but_enabled->setIcon(QIcon(Helper::getIconPath() + "power_off.png"));

	emit eq_enabled_signal(enabled);
}


void GUI_Equalizer::fill_eq_presets(const vector<EQ_Setting>& presets){

	_presets = presets;

	for(uint i=0; i<presets.size(); i++){

		this->_ui->combo_presets->insertItem(i, presets[i].name);

	}
}


void GUI_Equalizer::fill_available_equalizers(const QStringList& eqs){
	//this->_ui->but_enabled->setText(eqs[0]);
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
