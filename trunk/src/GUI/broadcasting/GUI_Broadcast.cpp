/* GUI_Broadcast.cpp */

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



#include "HelperStructs/Helper.h"
#include "GUI/broadcasting/GUI_Broadcast.h"
#include "GUI/broadcasting/GUI_BroadcastSetup.h"

#include <QMessageBox>


GUI_Broadcast::GUI_Broadcast(QString name, StreamServer* server, QWidget *parent) :
	PlayerPlugin(name, parent),
	Ui::GUI_Broadcast()
{
	setupUi(this);

	_server       = server;

	btn_dismiss->setEnabled(false);
	btn_dismiss_all->setEnabled(false);

	connect(btn_dismiss, SIGNAL(clicked()), this, SLOT(dismiss_clicked()));
	connect(btn_dismiss_all, SIGNAL(clicked()), this, SLOT(dismiss_all_clicked()));
	connect(btn_options, SIGNAL(clicked()), this, SLOT(option_clicked()));
	connect(combo_clients, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));
	connect(btn_retry, SIGNAL(clicked()), this, SLOT(retry()));

	connect(_server, SIGNAL(sig_new_connection_request(const QString&)),this, SLOT(new_connection_request(const QString&)));
	connect(_server, SIGNAL(sig_new_connection(const QString&)), this, SLOT(new_connection(const QString&)));
	connect(_server, SIGNAL(sig_connection_closed(const QString&)), this, SLOT(connection_closed(const QString&)));
	connect(_server, SIGNAL(sig_can_listen(bool)), this, SLOT(can_listen(bool)));

	_server->retry();


	set_status_label();

	REGISTER_LISTENER(SetNoDB::MP3enc_found, mp3_enc_found);
}


GUI_Broadcast::~GUI_Broadcast(){

}

void GUI_Broadcast::language_changed(){
	retranslateUi(this);
}

void GUI_Broadcast::set_status_label(){
	int n_listeners = combo_clients->count();
	QString str_listeners = QString::number(n_listeners);


	if(n_listeners == 1){
		lab_status->setText(str_listeners + " " + tr("listener"));
	}

	else{
		lab_status->setText(str_listeners + " " + tr("listeners"));
	}
}

void GUI_Broadcast::new_connection_request(const QString& ip){

	if(ip.isEmpty()){

		_server->reject_client();
		return;
	}


	QString question  = tr("Someone tries to listen to your music.\n") +
			"IP: " + ip + "\n";

	QString location = Helper::get_location_from_ip(ip);

	if(!location.isEmpty()){
			question += tr("from") + " " + location + "\n";
	}

	question += "\n" + tr("OK?");

	QString title = tr("Incoming request");

	QMessageBox::StandardButton btn;
	btn = QMessageBox::question(this,
								title,
								question,
								QMessageBox::Yes | QMessageBox::No,
								QMessageBox::StandardButton::Yes);

	if(btn == QMessageBox::No){
		_server->reject_client();
	}

	else{
		_server->accept_client();
	}
}


void GUI_Broadcast::new_connection(const QString& ip){

	combo_clients->addItem(ip);
	set_status_label();
	combo_clients->setCurrentIndex(combo_clients->count() -1);
	btn_dismiss_all->setEnabled(true);

}


void GUI_Broadcast::connection_closed(const QString& ip){
	qDebug() << "Connection closed: " << ip;
	int idx = 0;
	for(idx = 0; idx < combo_clients->count(); idx++){
		if(combo_clients->itemText(idx).contains(ip)){
			break;
		}
	}

	if(idx == combo_clients->count()) return;

	combo_clients->removeItem(idx);


	if(combo_clients->count() == 0){
		btn_dismiss->setEnabled(false);
		btn_dismiss_all->setEnabled(false);
	}

	set_status_label();
}

void GUI_Broadcast::can_listen(bool success){

	lab_status->setVisible(success);
	lab_error->setVisible(!success);
	btn_retry->setVisible(!success);

	if(!success){
		QString msg = tr("Cannot broadcast on port %1").arg(_settings->get(Set::Broadcast_Port));
		msg += "\n" + tr("Maybe another application is using this port?");

		QMessageBox::warning(this, tr("Warning"), msg);
	}
}

void GUI_Broadcast::retry(){
	_server->retry();
}


void GUI_Broadcast::dismiss_at(int idx){

	QString ip = combo_clients->itemText(idx);

	if(ip.startsWith("(d)")) return;

	combo_clients->setItemText(idx, QString("(d) ") + ip);

	_server->dismiss(idx);
}


void GUI_Broadcast::dismiss_clicked(){

	int idx = combo_clients->currentIndex();
	dismiss_at(idx);
	btn_dismiss->setEnabled(false);

}


void GUI_Broadcast::dismiss_all_clicked(){

	for(int idx = 0; idx <combo_clients->count(); idx++){
		dismiss_at(idx);
	}

	btn_dismiss_all->setEnabled(false);
	btn_dismiss->setEnabled(false);
}

void GUI_Broadcast::combo_changed(int idx){

	QString text = combo_clients->currentText();

	if(text.startsWith("(d)")){
		btn_dismiss->setEnabled(false);
	}
	else{
		btn_dismiss->setEnabled(true);
	}
}


void GUI_Broadcast::option_clicked(){

	GUI_BroadcastSetup* setup = new GUI_BroadcastSetup(this);
	connect(setup, SIGNAL(accepted()), setup, SLOT(deleteLater()));
	connect(setup, SIGNAL(rejected()), setup, SLOT(deleteLater()));
	setup->show();
}

void GUI_Broadcast::mp3_enc_found(){

	bool active = _settings->get(SetNoDB::MP3enc_found);
	if(!active){
		combo_clients->hide();
		btn_dismiss->hide();
		btn_dismiss_all->hide();
		lab_status->hide();
		lab_error->setText(tr("Cannot find lame mp3 encoder"));
	}

	else{
		lab_error->hide();
		btn_retry->hide();
	}
}
