/* Socket.cpp

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Jul 17, 2012 
 *
 */
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include "Socket/Socket.h"
#include "HelperStructs/CSettingsStorage.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif


Socket::Socket(QObject* parent) : QThread(parent) {

	CSettingsStorage* db = CSettingsStorage::getInstance();

	_port = db->getSocketFrom();
	_port_to = db->getSocketTo();
	_block = false;

	_srv_socket = -1;
	_client_socket = -1;
	_connected = false;
	_shot = false;
}


Socket::~Socket() {
	qDebug() << "disconnect";
	if(_srv_socket > 0)
		close(_srv_socket);
	if(_client_socket > 0)
		close(_client_socket);

	_client_socket = -1;
	_srv_socket = -1;
	_shot = false;

	_connected = false;
	_port = 1024;
}


void Socket::run() {

	char msg[4096];

	qDebug() << "Starting socket on port "<<_port;

	while(true){
		if(_client_socket == -1){
			sock_connect();
		}

		if(_connected){
			/*read(_client_socket, msg, 4095);
			QString qmsg(msg);
			if(qmsg.contains("GET")){
				_shot = false;
			}*/
		}

		usleep(1000000);
	}
}

bool Socket::sock_connect() {

	qDebug() << "Wait for incoming connection";
	socklen_t addr_len;
	struct sockaddr_in addr_in;

	_srv_socket = socket(AF_INET, SOCK_STREAM, 0);
	_srv_info.sin_family = AF_INET;
	_srv_info.sin_addr.s_addr = htonl(INADDR_ANY);
	_srv_info.sin_port = htons(_port);

	_connected = false;

	while(bind(_srv_socket, (struct sockaddr*) & _srv_info, sizeof(_srv_info)) == -1) {
		_port++;

		if(_port > _port_to) return false;

		_srv_info.sin_port = htons(_port);
		usleep(10000);
	}

	listen(_srv_socket, 3);


	addr_len = (socklen_t) sizeof(struct sockaddr);

	_client_socket = accept(_srv_socket, (struct sockaddr *) &(addr_in),
						   &addr_len);

	if(_client_socket < 0) return false;


	_ip = QString(inet_ntoa( addr_in.sin_addr));
	emit sig_new_connection_req(_ip);

	return true;
}

void Socket::sock_disconnect() {

	qDebug() << "Socket closed";
	close(_srv_socket);
	close(_client_socket);
	_client_socket = -1;
	_srv_socket = -1;
	_shot = false;

	_connected = false;
	_port = 1024;


	emit sig_connection_closed(_ip);
	_ip = "";
}

void Socket::connection_valid(bool b){

	if(!b) {
		sock_disconnect();
	}

	else{
		if(_srv_socket != -1){
			_connected = true;
			emit sig_new_connection(_ip);
		}
	}

}

void Socket::new_data(uchar* data, quint64 size){

	if(!_connected) return;
	/*if(!data || size== 0){
		_shot = false;
		return;
	}*/

	int n_bytes;

	if(!_shot){
		QByteArray answer = QByteArray("ICY 200 Ok\r\n"
									   "icy-notice1:Bliblablupp\r\n"
									   "icy-name:Awesome Sayonara stream\r\n"
									   "icy-genre:None\r\n"
									   "icy-url:http://sayonara.luciocarreras.de\r\n"
									   "content-type:audio/mpeg;\r\n"
									   "icy-br:160\r\n"
									   "icy-pub:1\r\n"
									   "\r\n"
									   );
		answer.append((const char*) data, size);
		data = (uchar*) answer.data();
		size = answer.size();
		_shot = true;
	}

	n_bytes = write(_client_socket, data, size);

	if(n_bytes == -1){
		sock_disconnect();
	}
}



