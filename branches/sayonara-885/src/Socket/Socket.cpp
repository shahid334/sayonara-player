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

//#define SOCKET_DEBUG

Socket::Socket(QObject* parent) : QThread(parent) {

	CSettingsStorage* db = CSettingsStorage::getInstance();

	_port = db->getSocketFrom();
	_port_to = db->getSocketTo();

	_icy = false;
	_srv_socket = -1;
	_client_socket = -1;
	_connected = false;
	_header_sent = false;
	_send_data = false;
	_bytes_written= 0;
	_header=  QByteArray("ICY 200 Ok\r\n"
										   "icy-notice1:Bliblablupp\r\n"
										   "icy-notice2:asdfasd\r\n"
										   "icy-name:Awesome Sayonara stream\r\n"
										   "icy-genre:None\r\n"
										   "icy-url:http://sayonara.luciocarreras.de\r\n"
										   "content-type:audio/mpeg\r\n"
										   "icy-pub:1\r\n"
										   "icy-br:128\r\n"

										   );

	_icy_header = QByteArray("icy-metaint:8192\r\n");

}


Socket::~Socket() {
	if(_srv_socket > 0){
		close(_srv_socket);
	}

	if(_client_socket > 0){
		close(_client_socket);
	}

	_client_socket = -1;
	_srv_socket = -1;
	_header_sent = false;
	_send_data = false;
	_connected = false;
	_icy = false;

	_port = 1024;

}


bool Socket::parse_message(){

	ssize_t n_bytes;
	char msg[4096];
	bool get_received = false;

	QString qmsg;
	QStringList lst;
	QByteArray header = _header;

	n_bytes = read(_client_socket, msg, 4095);
	if(n_bytes < 0) return false;

	qmsg = QString::fromLocal8Bit(msg, n_bytes);

	lst = qmsg.split("\r\n");
	_icy = false;

	foreach(QString str, lst){
		qDebug() << str;
		if(str.contains("GET")){
			get_received = true;
			continue;
		}

		if( str.contains("icy-metadata:", Qt::CaseInsensitive))
		{
			if(str.contains(":1") || str.contains(": 1")){

				qDebug() << "Set ICY to true";
				_icy = true;
				header.append(_icy_header);
				continue;
			}
		}
	}

	header.append("\r\n");
	if( send_header(header) && get_received){
		return true;
	}

	return false;

}


void Socket::run() {

	while(true){

		if(_client_socket == -1){
			sock_connect();
		}

		if(_connected){

			_send_data = parse_message();
			if(!_send_data){
				sock_disconnect();
			}
		}

		usleep(1000000);
	}

	sock_disconnect();
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

		if(_port > _port_to){
			qDebug() << "Socket: Cannot bind";
			return false;
		}

		_srv_info.sin_port = htons(_port);
		usleep(10000);
	}

	qDebug() << "Listening on port " << _port;
	listen(_srv_socket, 3);


	addr_len = (socklen_t) sizeof(struct sockaddr);

	/*_client_socket = accept(_srv_socket, (struct sockaddr *) &(addr_in),
						   &addr_len);*/

	_client_socket = accept(_srv_socket, NULL, NULL);


	if(_client_socket < 0) {
		qDebug() << "Socket: Cannot establish connection";
		return false;
	}

	//_ip = QString(inet_ntoa( addr_in.sin_addr));
	_ip = "192.168.1.105";

	emit sig_new_connection_req(_ip);

	return true;
}

void Socket::sock_disconnect() {

	qDebug() << "Socket closed";
	if(_srv_socket > 0){
		close(_srv_socket);
	}

	if(_client_socket > 0){
		close(_client_socket);
	}

	_client_socket = -1;
	_srv_socket = -1;
	_header_sent = false;
	_send_data = false;
	_connected = false;
	_icy = false;

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
			qDebug() << "Connected t0 " << _ip;
			_connected = true;
			emit sig_new_connection(_ip);
			emit sig_new_connection();
		}
	}

}

void Socket::new_data(uchar* data, quint64 size){

	if(!_connected) return;
	if(!_send_data) return;

	qint64 n_bytes;

	n_bytes = send_stream_data(data, size);

#ifdef SOCKET_DEBUG
	qDebug() << "Wrote " << n_bytes << " bytes";
#endif


	if(n_bytes == -1){
		sock_disconnect();
	}

}



bool Socket::send_header(const QByteArray& header){

	ssize_t n_bytes = write(_client_socket, header.constData(), header.size());
	if(n_bytes >= header.size()) return true;

	return false;
}


bool Socket::send_icy_data(){
	ssize_t n_bytes;
	QByteArray metadata = QByteArray("StreamTitle='Super Sayonara';"
									 "StreamUrl='';");
	for(int i=0; i<6;i++){
		metadata.append((char) 0x00);
	}

	metadata.prepend(0x03);
	n_bytes = write(_client_socket, metadata.constData(), metadata.size());

	return (n_bytes >= metadata.size());
}

qint64 Socket::send_stream_data(uchar* data , quint64 size){

	ssize_t n_bytes;

	qDebug() << _bytes_written + size << ";" << _icy;
	if(_bytes_written + size > 8192 && _icy){

		quint64 bytes_before = 8192 - _bytes_written;
		n_bytes = write(_client_socket, data, bytes_before);

		qDebug() << "time for icy package";
		send_icy_data();

		_bytes_written = 0;

		if(size - bytes_before > 0){
			n_bytes = write(_client_socket, data + bytes_before, size - bytes_before);
			_bytes_written = n_bytes;
		}
	}

	else{
		n_bytes = write(_client_socket, data, size);
		_bytes_written += n_bytes;
	}

	qDebug() << _bytes_written;

	return n_bytes;

}
