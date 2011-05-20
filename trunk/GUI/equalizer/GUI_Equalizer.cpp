/*
 * GUI_Equalizer.cpp
 *
 *  Created on: May 18, 2011
 *      Author: luke
 */


#include "HelperStructs/Equalizer_presets.h"
#include "GUI/equalizer/GUI_Equalizer.h"

#include <QObject>
#include <QWidget>
#include <QDebug>

#include <ui_GUI_equalizer.h>

#include <vector>

using namespace std;

GUI_Equalizer::GUI_Equalizer(QWidget* parent) : QWidget(parent) {

	this->_ui = new Ui_GUI_Equalizer( );
	this->_ui->setupUi(this);

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

	connect(this->_ui->cb_enabled, SIGNAL(toggled(bool)), this, SLOT(cb_enabled_changed(bool)));
	connect(this->_ui->combo_presets, SIGNAL(currentIndexChanged(int)), this, SLOT(preset_changed(int)));
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

void GUI_Equalizer::cb_enabled_changed(bool enabled){
	emit eq_enabled_signal(enabled);
}


void GUI_Equalizer::fill_eq_presets(const vector<EQ_Setting>& presets){
	qDebug() << "isnert presets";
	for(uint i=0; i<presets.size(); i++){
		QStringList setting;
		for(int j=0; j<presets[i].settings.size(); j++){
			setting.append(QString::number(presets[i].settings[j]));
		}
		this->_ui->combo_presets->insertItem(i, presets[i].name, setting);

	}

}


void GUI_Equalizer::preset_changed(int index){

	QStringList setting = this->_ui->combo_presets->itemData(index).toStringList();

	for(int i=0; i<setting.size(); i++){
		if( i==0) this->_ui->sli_0->setValue(setting[i].toInt());
		else if(i == 1) this->_ui->sli_1->setValue(setting[i].toInt());
		else if(i == 2) this->_ui->sli_2->setValue(setting[i].toInt());
		else if(i == 3) this->_ui->sli_3->setValue(setting[i].toInt());
		else if(i == 4) this->_ui->sli_4->setValue(setting[i].toInt());
		else if(i == 5) this->_ui->sli_5->setValue(setting[i].toInt());
		else if(i == 6) this->_ui->sli_6->setValue(setting[i].toInt());
		else if(i == 7) this->_ui->sli_7->setValue(setting[i].toInt());
		else if(i == 8) this->_ui->sli_8->setValue(setting[i].toInt());
		else if(i == 9) this->_ui->sli_9->setValue(setting[i].toInt());

		emit eq_changed_signal(i, setting[i].toInt());

	}



}
