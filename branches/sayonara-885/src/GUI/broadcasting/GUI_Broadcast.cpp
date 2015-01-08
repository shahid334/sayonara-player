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



#include "broadcasting/GUI_Broadcast.h"
#include "HelperStructs/Helper.h"
#include "broadcasting/GUI_BroadcastSetup.h"
#include <QMessageBox>


GUI_Broadcast::GUI_Broadcast(QString name, QWidget *parent) :
	PlayerPlugin(name, parent),
	Ui::GUI_Broadcast()
{
	setupUi(this);

	btn_dismiss->setEnabled(false);
	btn_dismiss_all->setEnabled(false);

	connect(btn_dismiss, SIGNAL(clicked()), this, SLOT(dismiss_clicked()));
	connect(btn_dismiss_all, SIGNAL(clicked()), this, SLOT(dismiss_all_clicked()));
	connect(btn_options, SIGNAL(clicked()), this, SLOT(option_clicked()));
	connect(combo_clients, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));

	set_status_label();

}


GUI_Broadcast::~GUI_Broadcast(){

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
		emit sig_rejected();
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
		emit sig_rejected();
	}

	else{
		emit sig_accepted();
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


void GUI_Broadcast::dismiss_at(int idx){

	QString ip = combo_clients->itemText(idx);

	if(ip.startsWith("(d)")) return;

	combo_clients->setItemText(idx, QString("(d) ") + ip);

	emit sig_dismiss(idx);
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
