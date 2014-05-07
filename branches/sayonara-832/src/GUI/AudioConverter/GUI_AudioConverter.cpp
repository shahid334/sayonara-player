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

GUI_AudioConverter::GUI_AudioConverter(QString name, QWidget *parent) :
	PlayerPlugin(name, parent)
{
	ui = new Ui::GUI_AudioConvert();
	ui->setupUi(this);

	_settings = CSettingsStorage::getInstance();
	LameBitrate br = _settings->getConvertQuality();

	ui->lab_logo->setPixmap(QPixmap(Helper::getIconPath() + "audio_convert.png").scaled(ui->lab_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

	ui->rb_cbr->setChecked(false);
	ui->rb_vbr->setChecked(false);


	connect(ui->rb_cbr, SIGNAL(toggled(bool)), this, SLOT(rb_cbr_toggled(bool)));
	connect(ui->rb_vbr, SIGNAL(toggled(bool)), this, SLOT(rb_vbr_toggled(bool)));
	connect(ui->cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));
	connect(ui->cb_active, SIGNAL(toggled(bool)), this, SLOT(cb_active_toggled(bool)));

	int idx = -1;

	switch(br){
		case LameBitrate_64:
		case LameBitrate_128:
		case LameBitrate_192:
		case LameBitrate_256:
		case LameBitrate_320:
			ui->rb_cbr->setChecked(true);
			idx = ui->cb_quality->findData((int) br);
			if(idx < 0 || idx >= ui->cb_quality->count()) break;
			else ui->cb_quality->setCurrentIndex(idx);
			break;

		default:
			ui->rb_vbr->setChecked(true);
			idx = ui->cb_quality->findData((int) br);
			if(idx < 0 || idx >= ui->cb_quality->count()) break;
			else ui->cb_quality->setCurrentIndex(idx);
			break;
	}
}


void GUI_AudioConverter::fill_cbr(){

	disconnect(ui->cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));
	ui->cb_quality->clear();

	ui->cb_quality->addItem("64", LameBitrate_64);
	ui->cb_quality->addItem("128", LameBitrate_128);
	ui->cb_quality->addItem("192", LameBitrate_192);
	ui->cb_quality->addItem("256", LameBitrate_256);
	ui->cb_quality->addItem("320", LameBitrate_320);
	connect(ui->cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));

	ui->cb_quality->setCurrentIndex(2);
}

void GUI_AudioConverter::fill_vbr(){
	disconnect(ui->cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));
	ui->cb_quality->clear();

	ui->cb_quality->addItem(tr("0 (Best)"), LameBitrate_var_0);
	ui->cb_quality->addItem("1", LameBitrate_var_1);
	ui->cb_quality->addItem("2", LameBitrate_var_2);
	ui->cb_quality->addItem("3", LameBitrate_var_3);
	ui->cb_quality->addItem("4", LameBitrate_var_4);
	ui->cb_quality->addItem("5", LameBitrate_var_5);
	ui->cb_quality->addItem("6", LameBitrate_var_6);
	ui->cb_quality->addItem("7", LameBitrate_var_7);
	ui->cb_quality->addItem("8", LameBitrate_var_8);
	ui->cb_quality->addItem(tr("9 (Worst)"), LameBitrate_var_9);
	connect(ui->cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));

	ui->cb_quality->setCurrentIndex(2);
}

void GUI_AudioConverter::rb_cbr_toggled(bool b){
	if(!b) return;
	fill_cbr();
}

void GUI_AudioConverter::rb_vbr_toggled(bool b){
	if(!b) return;
	fill_vbr();
}

void GUI_AudioConverter::cb_active_toggled(bool b){

	if(b) {
		QString dir = QFileDialog::getExistingDirectory(this, "Choose target directory", _settings->getConvertTgtPath());
		if(dir.size() > 0){
			_settings->setConvertTgtPath(dir);
			emit sig_active();
		}

		else {
			disconnect(ui->cb_active, SIGNAL(toggled(bool)), this, SLOT(cb_active_toggled(bool)));
			ui->cb_active->setChecked(false);
			connect(ui->cb_active, SIGNAL(toggled(bool)), this, SLOT(cb_active_toggled(bool)));
		}

	}

	else emit sig_inactive();
}

void GUI_AudioConverter::quality_changed(int index){
	LameBitrate q = (LameBitrate) ui->cb_quality->itemData(index).toInt();
	qDebug() << "Quality: " << q;
	_settings->setConvertQuality(q);
}

