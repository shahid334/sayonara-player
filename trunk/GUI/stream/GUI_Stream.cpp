/*
 * GUI_Stream.cpp
 *
 *  Created on: Feb 9, 2012
 *      Author: luke
 */

#include "ui_GUI_Stream.h"
#include "GUI/stream/GUI_Stream.h"
#include "HelperStructs/Helper.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QDialog>
#include <QIcon>
#include <QMap>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>


GUI_Stream::GUI_Stream() {
	this->ui = new Ui::GUI_stream();
	this->ui->setupUi(this);

	init_gui();
	_cur_station = -1;

	QMap<QString, QString> data;
	CDatabaseConnector::getInstance()->getAllStreams(data);
	if(data.size() > 0)
		psl_radio_stations_received(data);

	this->connect(this->ui->btn_listen, SIGNAL(clicked()), this, SLOT(listen_clicked()));
	this->connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_clicked()));
	this->connect(this->ui->btn_delete, SIGNAL(clicked()), this, SLOT(delete_clicked()));
	this->connect(this->ui->combo_stream, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_index_changed(int)));
	this->connect(this->ui->combo_stream, SIGNAL(editTextChanged(const QString&)), this, SLOT(combo_text_changed(const QString&)));

}

GUI_Stream::~GUI_Stream() {
	// TODO Auto-generated destructor stub
}

void GUI_Stream::listen_clicked(){


	QString url;
	QString name;

	if(_cur_station == -1){
		url = this->ui->combo_stream->currentText();
		name = "Radio";
	}

	else{
		url = _cur_station_adress;
		name = _cur_station_name;
	}

	if(url.size() > 5){
		emit sig_play_stream(url.trimmed(), name);
	}

}

void GUI_Stream::psl_radio_stations_received(const QMap<QString, QString>& radio_stations){

	_stations = radio_stations;
	if(radio_stations.size() > 0){
		_cur_station = 0;
	}

	this->ui->combo_stream->clear();
	int i=0;
	for(QMap<QString, QString>::const_iterator it = radio_stations.begin(); it != radio_stations.end(); it++, i++){
		if(i == 0){
			_cur_station_adress = it.value();
			_cur_station_name = it.key();
		}

		this->ui->combo_stream->addItem(it.key(), it.value());
	}
}


void GUI_Stream::init_gui(){
	this->ui->btn_delete->setIcon(QIcon(Helper::getIconPath() + "delete.png"));
	this->ui->btn_save->setIcon(QIcon(Helper::getIconPath() + "save.png"));
}

void GUI_Stream::combo_index_changed(int idx){
	_cur_station = idx;
	_cur_station_name = this->ui->combo_stream->itemText(idx);
	_cur_station_adress = _stations[_cur_station_name];
	this->ui->btn_delete->setEnabled(true);
	this->ui->btn_save->setEnabled(false);

	this->ui->combo_stream->setToolTip(_cur_station_adress);

}

void GUI_Stream::combo_text_changed(const QString& text){
	_cur_station = -1;
	this->ui->btn_delete->setEnabled(false);
	this->ui->btn_save->setEnabled((text.size() > 0));

	this->ui->combo_stream->setToolTip("");
}


void GUI_Stream::delete_clicked(){
	if(_cur_station == -1) return;

	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	QMessageBox msgBox;
	msgBox.setText("Really wanna delete" + _cur_station_name + "?" );
	msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
	int ret = msgBox.exec();
	if(ret == QMessageBox::Yes){
		if(db->deleteStream(_cur_station_name)){
			qDebug() << _cur_station_name << "successfully deleted";
			QMap<QString, QString> map;
			if(db->getAllStreams(map)){
				psl_radio_stations_received(map);
			}
		}
	}

	_cur_station = -1;
}

void GUI_Stream::save_clicked(){
	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	bool ok = false;
	QString name = QInputDialog::getText(NULL, "Please type name", "Please enter name", QLineEdit::Normal, "", &ok);
	bool success = ok;
	if(ok && name.size() > 0){
		success = db->addStream(name, this->ui->combo_stream->currentText());
	}

	if(success){
		QMap<QString, QString> map;
		if(db->getAllStreams(map)){
			psl_radio_stations_received(map);
		}
	}

	_cur_station = -1;
}

