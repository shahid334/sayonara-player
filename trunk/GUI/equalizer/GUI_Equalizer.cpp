/*
 * GUI_Equalizer.cpp
 *
 *  Created on: May 18, 2011
 *      Author: luke
 */

#include "GUI_Equalizer.h"
#include "ui_GUI_equalizer.h"
#include <QWidget>

GUI_Equalizer::GUI_Equalizer(QWidget* parent) {

	this->_ui = new Ui_GUI_Equalizer( );
	this->_ui->setupUi(this);

	connect(this->_ui->sli_0, SIGNAL(sliderMoved(int)), this, SLOT(sli_0_changed(int)));
	connect(this->_ui->sli_1, SIGNAL(sliderMoved(int)), this, SLOT(sli_1_changed(int)));
	connect(this->_ui->sli_2, SIGNAL(sliderMoved(int)), this, SLOT(sli_2_changed(int)));
	connect(this->_ui->sli_3, SIGNAL(sliderMoved(int)), this, SLOT(sli_3_changed(int)));
	connect(this->_ui->sli_4, SIGNAL(sliderMoved(int)), this, SLOT(sli_4_changed(int)));
	connect(this->_ui->sli_5, SIGNAL(sliderMoved(int)), this, SLOT(sli_5_changed(int)));
	connect(this->_ui->sli_6, SIGNAL(sliderMoved(int)), this, SLOT(sli_6_changed(int)));
	connect(this->_ui->sli_7, SIGNAL(sliderMoved(int)), this, SLOT(sli_7_changed(int)));
	connect(this->_ui->sli_8, SIGNAL(sliderMoved(int)), this, SLOT(sli_8_changed(int)));
	connect(this->_ui->sli_9, SIGNAL(sliderMoved(int)), this, SLOT(sli_9_changed(int)));
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

