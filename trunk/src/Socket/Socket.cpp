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

	_srv_socket = -1;
	_client_socket = -1;
	_connected = false;


}


Socket::~Socket() {

}


void Socket::run(){
	char msg[1024];

	qDebug() << "Starting socket on port "<<_port;

	while(1){

		if(!sock_connect()) {
			qDebug() << "Cannot establish socket";
			return;
		}

		int err = -1;

		while( err != 0 ){
			err = recv(_client_socket, msg, 1024, 0);
			qDebug() << "error on port " << _client_socket << " Port = " << _port;
			if(err != -1){

				QString msg_string(msg);
				if(msg_string.startsWith("next")) {
					emit sig_next();
				}
				else if(msg_string.startsWith("prev")){
					emit sig_prev();
				}
				else if(msg_string.startsWith("play")) {
					emit sig_play();
				}
				else if(msg_string.startsWith("pause")) {
					emit sig_pause();
				}

				else if(msg_string.startsWith("stop")) {
					emit sig_stop();
				}

				else if(msg_string.startsWith("vol")) {
					bool ok;
					int volume = msg_string.mid(3,2).toInt(&ok);
					if(ok) emit sig_setVolume(volume);
				}


				else if(msg_string.startsWith("q")) {
					break;
				}
			}

			if( err == 0) break;

			usleep(100000);
		}

		qDebug() << "Socket closed";

		sock_disconnect();
	}
}

bool Socket::sock_connect(){

	_srv_socket = socket(AF_INET, SOCK_STREAM, 0);
	_srv_info.sin_family = AF_INET;
	_srv_info.sin_addr.s_addr = htonl(INADDR_ANY);
	_srv_info.sin_port = htons(_port);

	_connected = false;

	while(bind(_srv_socket, (struct sockaddr*) & _srv_info, sizeof(_srv_info)) == -1){
		_port++;

		if(_port > _port_to) return false;

		_srv_info.sin_port = htons(_port);
		usleep(10000);
	}

	printf("bind successful: port = %d\n", _port);

	listen(_srv_socket, 3);

	while(_client_socket == -1) {
		_client_socket = accept(_srv_socket, (struct sockaddr*) NULL, NULL);
	}

	_connected = true;
	return true;
}

void Socket::sock_disconnect(){

	qDebug() << "disconnect";
	close(_srv_socket);
	close(_client_socket);
	_client_socket = -1;
	_srv_socket = -1;

	_connected = false;
	_port = 1234;

}



