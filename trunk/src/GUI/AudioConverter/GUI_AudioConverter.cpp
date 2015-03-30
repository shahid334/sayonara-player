/* GUI_AudioConverter.cpp */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#include "GUI_AudioConverter.h"
#include "HelperStructs/Helper.h"
#include <QFileDialog>
#include <QMessageBox>

GUI_AudioConverter::GUI_AudioConverter(QString name, QWidget *parent) :
	PlayerPlugin(name, parent),
	Ui::GUI_AudioConvert()
{

	setupUi(this);

	LameBitrate br = (LameBitrate) _settings->get(Set::Engine_ConvertQuality);

	rb_cbr->setChecked(false);
	rb_vbr->setChecked(false);

	connect(rb_cbr, SIGNAL(toggled(bool)), this, SLOT(rb_cbr_toggled(bool)));
	connect(rb_vbr, SIGNAL(toggled(bool)), this, SLOT(rb_vbr_toggled(bool)));
	connect(cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));
	connect(cb_active, SIGNAL(toggled(bool)), this, SLOT(cb_active_toggled(bool)));

	int idx = -1;

	switch(br) {
		case LameBitrate_64:
		case LameBitrate_128:
		case LameBitrate_192:
		case LameBitrate_256:
		case LameBitrate_320:
			rb_cbr->setChecked(true);
			idx = cb_quality->findData((int) br);
			if(idx < 0 || idx >= cb_quality->count()) break;
			else cb_quality->setCurrentIndex(idx);
			break;

		default:
			rb_vbr->setChecked(true);
			idx = cb_quality->findData((int) br);
			if(idx < 0 || idx >= cb_quality->count()) break;
			else cb_quality->setCurrentIndex(idx);
			break;
	}

	_mp3_enc_available = true;

	REGISTER_LISTENER(SetNoDB::MP3enc_found, mp3_enc_found);
}

void GUI_AudioConverter::language_changed(){
	retranslateUi(this);
}


void GUI_AudioConverter::fill_cbr() {

	disconnect(cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));
	cb_quality->clear();

	cb_quality->addItem("64", LameBitrate_64);
	cb_quality->addItem("128", LameBitrate_128);
	cb_quality->addItem("192", LameBitrate_192);
	cb_quality->addItem("256", LameBitrate_256);
	cb_quality->addItem("320", LameBitrate_320);
	connect(cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));

	cb_quality->setCurrentIndex(2);
}

void GUI_AudioConverter::fill_vbr() {
	disconnect(cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));
	cb_quality->clear();

	cb_quality->addItem(tr("0 (Best)"), LameBitrate_var_0);
	cb_quality->addItem("1", LameBitrate_var_1);
	cb_quality->addItem("2", LameBitrate_var_2);
	cb_quality->addItem("3", LameBitrate_var_3);
	cb_quality->addItem("4", LameBitrate_var_4);
	cb_quality->addItem("5", LameBitrate_var_5);
	cb_quality->addItem("6", LameBitrate_var_6);
	cb_quality->addItem("7", LameBitrate_var_7);
	cb_quality->addItem("8", LameBitrate_var_8);
	cb_quality->addItem(tr("9 (Worst)"), LameBitrate_var_9);
	connect(cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));

	cb_quality->setCurrentIndex(2);
}


void GUI_AudioConverter::stopped(){

	if(!isVisible()) return;
	if(!cb_active->isChecked()) return;

	cb_active->setChecked( false );
}

void GUI_AudioConverter::closeEvent(QCloseEvent* e){

	PlayerPlugin::closeEvent(e);
}



void GUI_AudioConverter::rb_cbr_toggled(bool b) {
	if(!b) return;
	fill_cbr();
}

void GUI_AudioConverter::rb_vbr_toggled(bool b) {
	if(!b) return;
	fill_vbr();
}

void GUI_AudioConverter::cb_active_toggled(bool b) {

	if(!_mp3_enc_available){
		QMessageBox::warning(this, tr("Error"), tr("Cannot find lame mp3 encoder"));
		disconnect(cb_active, SIGNAL(toggled(bool)), this, SLOT(cb_active_toggled(bool)));
		cb_active->setChecked(false);
		connect(cb_active, SIGNAL(toggled(bool)), this, SLOT(cb_active_toggled(bool)));
		return;
	}

	if(b) {

		QString cvt_target_path = _settings->get(Set::Engine_CovertTargetPath);
		QString dir = QFileDialog::getExistingDirectory(this, "Choose target directory", cvt_target_path);

		if(dir.size() > 0) {
			_settings->set(Set::Engine_CovertTargetPath, dir);
			emit sig_active();
		}

		else {
			disconnect(cb_active, SIGNAL(toggled(bool)), this, SLOT(cb_active_toggled(bool)));
			cb_active->setChecked(false);
			connect(cb_active, SIGNAL(toggled(bool)), this, SLOT(cb_active_toggled(bool)));
		}

	}

	else emit sig_inactive();
}

void GUI_AudioConverter::quality_changed(int index) {
	LameBitrate q = (LameBitrate) cb_quality->itemData(index).toInt();
	qDebug() << "Quality: " << q;
	_settings->set(Set::Engine_ConvertQuality, (int) q);
}

void GUI_AudioConverter::mp3_enc_found(){
	_mp3_enc_available = _settings->get(SetNoDB::MP3enc_found);
}

