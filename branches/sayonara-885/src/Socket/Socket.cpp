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
#include "Socket/Socket.h"
#include "HelperStructs/CSettingsStorage.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif


Socket::Socket() {

	CSettingsStorage* db = CSettingsStorage::getInstance();

	_port = db->getSocketFrom();
	_port_to = db->getSocketTo();
	_block = false;

	_srv_socket = -1;
	_client_socket = -1;
	_connected = false;

}


Socket::~Socket() {
	qDebug() << "disconnect";
	if(_srv_socket > 0)
		close(_srv_socket);
	if(_client_socket > 0)
		close(_client_socket);

	_client_socket = -1;
	_srv_socket = -1;

	_connected = false;
	_port = 1234;
}


void Socket::run() {
	char msg[4096];

	qDebug() << "Starting socket on port "<<_port;

	while(1) {

		if(!sock_connect()) {
			qDebug() << "Cannot establish socket";
			return;
		}

		int err = -1;
		emit sig_new_fd(_client_socket);

		while( true ) {
			err = read(_client_socket, msg, 4096);


			if(err != -1) {

				QString msg_string(msg);

				if(msg_string.contains("GET") && !_list.isEmpty()){

					int n_bytes;
					QByteArray answer = QByteArray("HTTP/1.1 200 Ok\r\nContent-Type: audio/mpeg;\r\n\r\n");
					answer += QString::number(_list[0].size(), 16) + "\r\n" + _list[0] + "\r\n";


					n_bytes = write(_client_socket, (uchar*) answer.constData(), answer.size());
					qDebug() << "Wrote " << n_bytes;
					_block = true;
					_list.pop_front();
					_block = false;
				}
			}

			else{
				qDebug() << "error on port " << _client_socket << " Port = " << _port;
			}
		}

		qDebug() << "Socket closed";

		sock_disconnect();
	}
}

bool Socket::sock_connect() {

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

	qDebug() << "bind successful: port = " << _port;

	listen(_srv_socket, 3);

	while(_client_socket == -1) {
		_client_socket = accept(_srv_socket, NULL, NULL);
		usleep(10000);
	}

	_connected = true;
	return true;
}

void Socket::sock_disconnect() {

	qDebug() << "disconnect";
	close(_srv_socket);
	close(_client_socket);
	_client_socket = -1;
	_srv_socket = -1;

	_connected = false;
	_port = 1234;

}

void Socket::new_data(uchar* data, quint64 size){

	if(_block) return;
	/*if(_client_socket < 0 || _block) {

		return;
	}*/

	//qDebug() << "Wrote " <<  write(_client_socket, data, size);
	QByteArray arr((const char*) data, size);
	_list << arr;
	//delete data;
}



