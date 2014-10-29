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
#undef SOCKET_DEBUG


static char padding[16];

Socket::Socket(QObject* parent) : QThread(parent) {

	memset(padding, 0, 16);
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
	_srv_socket = -1;
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

	if(_client_socket > 0){
		close(_client_socket);
	}

	close(_srv_socket);
	shutdown(_srv_socket, 2);
	_srv_socket = -1;

	qDebug() << "Thread destroyed";
}



void Socket::run() {

	init_socket();

	while( _srv_socket > 0 ){

		if(_client_socket == -1){

			sock_connect();
		}

		if(_connected){

			_send_data = parse_message();
			if(!_send_data){
				sock_disconnect();
			}
		}

		usleep(10000);
	}

	sock_disconnect();

	if(_srv_socket > 0){
		close(_srv_socket);
	}
}



bool Socket::init_socket(){

	if(_srv_socket > 0) return true;

	_connected = false;
	memset( &_srv_info, 0, sizeof(_srv_info));

	int reuse_addr;
	int status;

	_srv_info.sin_family = AF_INET;
	_srv_info.sin_addr.s_addr = htonl(INADDR_ANY);
	_srv_info.sin_port = htons(_port);

	_srv_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(_srv_socket < 0){
		qDebug() << "Server socket invalid";
		qDebug() << strerror(errno);
		return false;
	}

	setsockopt(_srv_socket, SOL_PACKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

	status = bind(_srv_socket, (struct sockaddr*) & _srv_info, sizeof(_srv_info));
	if(status < 0){
		qDebug() << "Cannot bind port " << _port;
		qDebug() << strerror(errno);
		close(_srv_socket);
		_srv_socket = -1;
		return false;
	}

	qDebug() << "Listening on port " << _port;
	status = listen(_srv_socket, 3);
	if(status < 0){
		qDebug() << "Cannot listen to port " << _port;
		qDebug() << strerror(errno);
		close(_srv_socket);
		_srv_socket = -1;
		return false;
	}

	return true;

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

#ifdef SOCKET_DEBUG
		qDebug() << str;
#endif

		if(str.contains("GET")){
			get_received = true;
			continue;
		}

		if( str.contains("icy-metadata:", Qt::CaseInsensitive))
		{
			if(str.contains(":1") || str.contains(": 1")){

#ifdef SOCKET_DEBUG
				qDebug() << "Set ICY to true";
#endif

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


bool Socket::sock_connect() {

	qDebug() << "Wait for incoming connection";

	socklen_t addr_len;
	struct sockaddr_in addr_in;
	memset( &addr_in, 0, sizeof(addr_in));

	addr_len = (socklen_t) sizeof(struct sockaddr);

	_client_socket = accept(_srv_socket, (struct sockaddr *) &(addr_in),
						   &addr_len);


	if(_client_socket < 0) {
		qDebug() << "Socket: Cannot establish connection";
		qDebug() << strerror(errno);
		return false;
	}

	_ip = QString(inet_ntoa( addr_in.sin_addr));

	//emit sig_new_connection_req(_ip);

	qDebug() << "Connected t0 " << _ip;
	_connected = true;
	emit sig_new_connection(_ip);
	emit sig_new_connection();

	return true;
}

void Socket::sock_disconnect() {

	if(_client_socket > 0){
		shutdown(_client_socket, 2);
		close(_client_socket);
	}

	_client_socket = -1;
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

	else if(0){
		if(_srv_socket != -1){
			qDebug() << "Connected to " << _ip;
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
	QByteArray metadata = QByteArray("StreamTitle='");
	metadata.append(_stream_title.toLocal8Bit());
	metadata.append("';");
	metadata.append("StreamUrl='http://sayonara.luciocarreras.de';");

	int sz = metadata.size();
	int n_padding = ( (int)((sz + 15) / 16) * 16 - sz );

	metadata.append(padding, n_padding);
	metadata.prepend((char) (int)((sz + 15) / 16));

	n_bytes = write(_client_socket, metadata.constData(), metadata.size());

	return (n_bytes >= metadata.size());
}

qint64 Socket::send_stream_data(uchar* data , quint64 size){

	ssize_t n_bytes;

#ifdef SOCKET_DEBUG
	qDebug() << _bytes_written + size << ";" << _icy;
#endif

	if(_bytes_written + size > 8192 && _icy){

		quint64 bytes_before = 8192 - _bytes_written;
		n_bytes = write(_client_socket, data, bytes_before);

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

#ifdef SOCKET_DEBUG
	qDebug() << _bytes_written;
#endif

	return n_bytes;
}

void Socket::psl_update_track(const MetaData& md){
	_stream_title = "Sayonara Player: " + md.title + " by " + md.artist;
}

void Socket::stop(){
	sock_disconnect();
	close(_srv_socket);
	shutdown(_srv_socket, 2);
	_srv_socket = -1;
}
