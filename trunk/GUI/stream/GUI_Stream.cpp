/* GUI_Stream.cpp */

/* Copyright (C) 2012  Lucio Carreras
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


#include "ui_GUI_Stream.h"
#include "GUI/stream/GUI_Stream.h"
#include "HelperStructs/Helper.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QDockWidget>
#include <QIcon>
#include <QMap>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>


GUI_Stream::GUI_Stream(QWidget* parent): QDockWidget(parent)  {
	this->ui = new Ui::GUI_Stream();
	this->ui->setupUi(this);

	init_gui();
	_cur_station = -1;

	QMap<QString, QString> data;
	CDatabaseConnector::getInstance()->getAllStreams(data);
	if(data.size() > 0)
		setup_stations(data);

	this->ui->btn_listen->setIcon(QIcon(Helper::getIconPath() + "play.png"));

	this->connect(this->ui->btn_listen, SIGNAL(clicked()), this, SLOT(listen_clicked()));
	this->connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_clicked()));
	this->connect(this->ui->btn_delete, SIGNAL(clicked()), this, SLOT(delete_clicked()));
	this->connect(this->ui->combo_stream, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_index_changed(int)));
	this->connect(this->ui->combo_stream, SIGNAL(editTextChanged(const QString&)), this, SLOT(combo_text_changed(const QString&)));

	this->connect(this->ui->le_url, SIGNAL(textEdited(const QString&)), this, SLOT(url_text_changed(const QString&)));

    hide();
}


GUI_Stream::~GUI_Stream() {
	// TODO Auto-generated destructor stub
}


void GUI_Stream::listen_clicked(){

	QString url;
	QString name;

	if(_cur_station == -1){
        url = this->ui->le_url->text();
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


void GUI_Stream::setup_stations(const QMap<QString, QString>& radio_stations){

	_stations = radio_stations;
	if(radio_stations.size() > 0){
		_cur_station = -1;
	}

	this->ui->combo_stream->clear();

	_cur_station_adress = "";
	_cur_station_name = "";
	_cur_station = 0;

	_stations[""] = "";

	for(QMap<QString, QString>::iterator it = _stations.begin(); it != _stations.end(); it++){
		this->ui->combo_stream->addItem(it.key(), it.value());
	}

	this->ui->btn_listen->setEnabled(false);
	this->ui->btn_save->setEnabled(false);
	this->ui->btn_delete->setEnabled(false);
}


void GUI_Stream::init_gui(){
	this->ui->btn_delete->setIcon(QIcon(Helper::getIconPath() + "delete.png"));
	this->ui->btn_save->setIcon(QIcon(Helper::getIconPath() + "save.png"));
}


void GUI_Stream::combo_index_changed(int idx){

	_cur_station = idx;
	_cur_station_name = this->ui->combo_stream->itemText(_cur_station);

	QString adress = _stations[_cur_station_name];
	if(adress.size() > 0){
		_cur_station_adress = adress;
		this->ui->le_url->setText(_cur_station_adress);
	}

	if(idx == 0){
		this->ui->le_url->setText("");
	}


	this->ui->btn_delete->setEnabled(idx > 0);
	this->ui->btn_save->setEnabled(false);
	this->ui->btn_listen->setEnabled(this->ui->le_url->text().size() > 5);
	this->ui->combo_stream->setToolTip(_cur_station_adress);
}


void GUI_Stream::combo_text_changed(const QString& text){
	_cur_station = -1;

	this->ui->btn_delete->setEnabled(false);
	this->ui->btn_save->setEnabled((text.size() > 0));
	this->ui->btn_listen->setEnabled(this->ui->le_url->text().size() > 5);
	this->ui->combo_stream->setToolTip("");
}

void GUI_Stream::url_text_changed(const QString& text){

	QString key = _stations.key(text);

	if(! key.isEmpty() ){

		int idx = this->ui->combo_stream->findText(key, Qt::MatchCaseSensitive);
		if(idx != -1){
			this->ui->combo_stream->setCurrentIndex(idx);
			_cur_station = idx;
			this->ui->btn_save->setEnabled(false);
			this->ui->btn_delete->setEnabled(true);
		}
	}

	// new adress
	else{

		this->ui->btn_delete->setEnabled(false);

		bool save_enabled =
				this->ui->combo_stream->currentText().size() > 0 &&
				this->ui->le_url->text().size() > 5 &&
				_cur_station == -1;

		this->ui->btn_save->setEnabled(save_enabled);
		this->ui->btn_listen->setEnabled(text.size() > 5);
		if(_cur_station != -1){
			_cur_station = -1;
			this->ui->combo_stream->setEditText("new");
			_cur_station = -1;
		}
	}

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
				setup_stations(map);
			}
		}
	}

	_cur_station = -1;
}


void GUI_Stream::save_clicked(){
	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	QString name = this->ui->combo_stream->currentText();
	QString url = this->ui->le_url->text();

	bool success = false;
	if(name.size() > 0 && url.size() > 0){
		success = db->addStream(name, url);
	}

	if(success){
		QMap<QString, QString> map;
		if(db->getAllStreams(map)){
			setup_stations(map);
		}
	}


	_cur_station = -1;
    this->ui->le_url->setText(url);
    url_text_changed(url);
}

void GUI_Stream::closeEvent ( QCloseEvent * event ){
	event->ignore();
    hide();
    close();
	emit sig_close_event();
}


