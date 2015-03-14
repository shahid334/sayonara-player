/* StreamServer.cpp */

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



#include "RadioStation/StreamServer.h"
#include "HelperStructs/Helper.h"
#include <QHostAddress>

StreamServer::StreamServer(QObject* parent) : 
	QThread(parent),
	SayonaraClass()
{
	_n_clients = 0;
	_port = _settings->get(Set::Broadcast_Port);

	_server = new QTcpServer();

	_server->setMaxPendingConnections(10);
	connect(_server, SIGNAL(newConnection()), this, SLOT(new_client_request()));
	connect(_server, SIGNAL(destroyed()), this, SLOT(server_destroyed()));

	_pending_socket = NULL;

	REGISTER_LISTENER(Set::BroadCast_Active, active_changed);
	REGISTER_LISTENER(Set::Broadcast_Prompt, prompt_changed);
	REGISTER_LISTENER_NO_CALL(Set::Broadcast_Port, port_changed);


	if( !_server->isListening() ){
		listen_for_connection();
	}

	REGISTER_LISTENER(SetNoDB::MP3enc_found, mp3_enc_found);
}

StreamServer::~StreamServer(){

	Helper::sleep_ms(500);
	while(_server){
		Helper::sleep_ms(1000);
	}
}

void StreamServer::server_destroyed(){
	qDebug() << "Server destroyed";
}

void StreamServer::run(){

	emit sig_can_listen(_server->isListening());

	forever{

		if(!_settings->get(Set::BroadCast_Active)){
			Helper::sleep_ms(100);
		}

		else if(_server && _server->isListening()){
			Helper::sleep_ms(100);
		}

		if(_server == NULL){
			break;
		}

		Helper::sleep_ms(250);
    }

	qDebug() << "Radio station: Bye";
}

bool StreamServer::listen_for_connection(){

	bool success;

	if(!_server){
		qDebug() << "Server socket invalid";
		return false;
	}

	qDebug() << "Listening on port " << _port;

	success = _server->listen(QHostAddress::Any, _port);

	if(!success){
		qDebug() << "Cannot listen on port " << _port;
		qDebug() << _server->errorString();

		_server->close();
	}

	return success;
}


void StreamServer::new_client_request(){

	_pending_socket = _server->nextPendingConnection();

	if(!_pending_socket) return;

	QString ip = _pending_socket->peerAddress().toString();

	if( _settings->get(Set::Broadcast_Prompt) ){

		bool ip_found = false;

		if(_allowed_ips.contains(ip)){
			ip_found = true;
		}

		if(!ip_found){
			emit sig_new_connection_request(ip);
			return;
		}
	}

	accept_client();

}

void StreamServer::accept_client(){


	qDebug() << "*** New request " << _pending_socket->peerAddress() << "***";

	StreamWriter* sw = new StreamWriter(_pending_socket, _md);

	connect(sw, SIGNAL(sig_disconnected(StreamWriter*)), this, SLOT(disconnected(StreamWriter*)));
	connect(sw, SIGNAL(sig_new_connection(const QString&)), this, SLOT(new_connection(const QString&)));

	_lst_sw << sw;
	_n_clients++;
	_pending_socket = NULL;

	_settings->set(SetNoDB::Broadcast_Clients, _n_clients);

	qDebug() << "Number of active connections: " << _n_clients;
	qDebug() << "";

	emit sig_new_connection(sw->get_ip());
}


void StreamServer::reject_client(){
	_pending_socket = NULL;
}


void StreamServer::new_data(uchar* data, quint64 size){

	foreach(StreamWriter* sw, _lst_sw){

		sw->send_data(data, size);
	}
}

void StreamServer::update_track(const MetaData & md){
	_md = md;
	foreach(StreamWriter* sw, _lst_sw){
		sw->change_track(md);
	}
}

void StreamServer::server_close(){
	
	if(_server){
		_server->close();
		qDebug() << "Server closed..";
	}
}

// when user forbids further streaming
void StreamServer::dismiss(int idx){

	if( idx >= _lst_sw.size() ) return;

	StreamWriter* sw = _lst_sw[idx];

	sw->disable();
}

// real socket disconnect (if no further sending is possible)
void StreamServer::disconnect(StreamWriter* sw){

	sw->disconnect();

}

void StreamServer::disconnect_all(){

	foreach(StreamWriter* sw, _lst_sw){
		sw->disconnect();
	}

	_lst_sw.clear();
}

void StreamServer::stop(){

	server_close();
	disconnect_all();
	delete _server;
	_server = NULL;
}

void StreamServer::active_changed(){

	bool active = _settings->get(Set::BroadCast_Active);

    if(!active) {
		disconnect_all();
	}

    else{

		bool success = _server->isListening();

		if( !success ){
			success = listen_for_connection();
			emit sig_can_listen(success);
		}

		if(success){
			this->start();
		}
    }
}

void StreamServer::disconnected(StreamWriter* sw){

	if(!sw) return;

	qDebug() << "*** STREAM WRITER DISCONNECTED ***";
	qDebug() << "Number of active connections: " << _n_clients - 1;
	qDebug() << "";
	int idx = _lst_sw.indexOf(sw);
	if(idx >= 0 && idx < _lst_sw.size()){
		_lst_sw.removeAt(idx);
	}

	emit sig_connection_closed(sw->get_ip());

	if(_n_clients > 0) _n_clients--;
	_settings->set(SetNoDB::Broadcast_Clients, _n_clients);

	delete sw;
	sw = 0;


	if(_n_clients == 0 && !_settings->get(Set::BroadCast_Active)){
		server_close();
	}
}

void StreamServer::prompt_changed(){
	_prompt = _settings->get(Set::Broadcast_Prompt);
}

void StreamServer::port_changed(){

	bool success;

	disconnect_all();
	server_close();

	_port = _settings->get(Set::Broadcast_Port);

	success = listen_for_connection();

	emit sig_can_listen(success);
}

void StreamServer::new_connection(const QString& ip){

}

void StreamServer::retry(){

	if(!_settings->get(Set::BroadCast_Active)){
		return;
	}


	bool success;
	if(_server->isListening()){
		success = true;
	}

	else{
		success = listen_for_connection();
	}

	emit sig_can_listen(success);
}

void StreamServer::mp3_enc_found(){

	bool active = _settings->get(SetNoDB::MP3enc_found);

	if(!active){
		stop();
	}
}
