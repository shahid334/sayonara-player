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
#include <sys/ioctl.h>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#define SOCKET_DEBUG
//#undef SOCKET_DEBUG


static char padding[16];
static char select_buffer[512];
Socket::Socket(QObject* parent) : QThread(parent) {

	strncpy(select_buffer, "HTTP/1.1 503", 13);

	CSettingsStorage* db = CSettingsStorage::getInstance();

	_port = db->getSocketFrom();

	_header=  QByteArray("ICY 200 Ok\r\n"
										   "icy-notice1:Bliblablupp\r\n"
										   "icy-notice2:asdfasd\r\n"
										   "icy-name:Awesome Sayonara stream\r\n"
										   "icy-genre:None\r\n"
										   "icy-url:http://sayonara.luciocarreras.de\r\n"
										   "content-type:audio/mpeg\r\n"
										   "connection:close\r\n"
										   "icy-pub:1\r\n"
										   "icy-br:128\r\n"

										   );

	_icy_header = QByteArray("icy-metaint:8192\r\n");

	client_reset();

}


Socket::~Socket() {

	client_disconnect();
	server_disconnect();

	qDebug() << "Thread destroyed";
}



void Socket::run() {


	bool success;
	while(1){

		while( !init_server_socket() ){
			msleep(1000);
		}

		while( _srv_socket > 0 ){

			success = client_connect();
			if(!success) {
				continue;
			}

			HttpAnswer a = parse_message();
			if(a == HttpAnswerFail){
				client_disconnect();
				continue;
			}

			else if(a == HttpAnswerReject){
				send_reject_header();
				//client_disconnect();
				//continue;
			}

			success = send_header();
			if(!success){
				client_disconnect();
				continue;
			}

			_send_data = true;

			while(_connected){
				msleep(1000);
			}
		}
	}
}
char msg[4096];
bool Socket::check_for_new_data(){

	if(_client_socket < 0) {
		qDebug() << "No new data... client_disconnected";
		return false;
	}
	struct timeval t;
	t.tv_sec = 1;
	fd_set sock_set;
	int client_socket = _client_socket;
	int read_socks;

	FD_ZERO(&sock_set);
	FD_SET(client_socket, &sock_set);

	read_socks = select( _client_socket + 1, &sock_set, NULL, NULL, &t);

	if(FD_ISSET(_client_socket, &sock_set)){
		//read(_client_socket, msg,4095);
		qDebug() << "New data availabel";
		return true;
	}

	qDebug() << "No New data availabel";
	return false;
}


bool Socket::init_server_socket(){

	int status;
	int reuse_addr = 1;

	_srv_socket = socket(AF_INET, SOCK_STREAM, 0);

	_connected = false;
	memset( &_srv_info, 0, sizeof(_srv_info));


	_srv_info.sin_family = AF_INET;
	_srv_info.sin_addr.s_addr = htonl(INADDR_ANY);
	_srv_info.sin_port = htons(_port);

	if(_srv_socket < 0){
		qDebug() << "Server socket invalid";
		qDebug() << strerror(errno);

		return false;
	}

	status = setsockopt(_srv_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
	if(status < 0){
		qDebug() << "Reuse adress: " << strerror(errno);
	}


	status = bind(_srv_socket, (struct sockaddr*) & _srv_info, sizeof(_srv_info));
	if(status < 0){
		qDebug() << "Cannot bind port " << _port;
		qDebug() << strerror(errno);
		server_disconnect();

		return false;
	}

	_srv_info.sin_port = htons(_port);

	qDebug() << "Listening on port " << _port;
	status = listen(_srv_socket, 1);
	if(status < 0){
		qDebug() << "Cannot listen to port " << _port;
		qDebug() << strerror(errno);
		close(_srv_socket);
		server_disconnect();
		return false;

	}

	return true;
}


HttpAnswer Socket::parse_message(){

	ssize_t n_bytes;
	char msg[4096];
	bool get_received = false;
	bool icy=false;
	bool connection_close = false;

	QString qmsg;
	QStringList lst;

	n_bytes = read(_client_socket, msg, 4095);
	if(n_bytes < 0) {
		return HttpAnswerFail;
	}

	qmsg = QString::fromLocal8Bit(msg, n_bytes);

	lst = qmsg.split("\r\n");
	_icy = false;

#ifdef SOCKET_DEBUG
	qDebug();
	qDebug();
	qDebug() << "*** Parse Message ***";
#endif
	foreach(QString str, lst){

#ifdef SOCKET_DEBUG
			qDebug() << str;
#endif

		if(str.contains("GET")){
			get_received = true;
			continue;
		}

		if(str.contains("user-agent: gvfs", Qt::CaseInsensitive)){
			qDebug() << "Reject!!";
			return HttpAnswerReject;
		}

		if( str.contains("icy-metadata:", Qt::CaseInsensitive))
		{
			if(str.contains(":1") || str.contains(": 1")){

				icy = true;
				continue;
			}
		}

		if(str.contains("user-agent", Qt::CaseInsensitive)){
			qDebug() << str;
			if(str.size() > 11){
				_user_agent = str.right( str.size() - 11).toLower();
			}
		}
	}

	if(_user_agent.isEmpty()){
		return HttpAnswerReject;
	}
	_icy = icy;

#ifdef SOCKET_DEBUG

	qDebug() << "*** Parse Message end ***";
	qDebug();
	qDebug();
#endif

	if(get_received){
		return HttpAnswerOK;
	}

	return HttpAnswerFail;

}

bool Socket::client_connect(){

	socklen_t addr_len;
	struct sockaddr_in addr_in;
	memset( &addr_in, 0, sizeof(addr_in));

	addr_len = (socklen_t) sizeof(struct sockaddr);

	/*fd_set read_fd_set, active_fd_set;
	FD_SET(_srv_socket, &active_fd_set);
	read_fd_set = active_fd_set;
	qDebug() << "Select...";
	status = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);
	if(status < 0){
		_connected = false;
		return false;
	}

	for(int i=0; i<FD_SETSIZE; i++){
		if(FD_ISSET(i, &read_fd_set) && i == _srv_socket){
			qDebug() << "Some client asks for permission...";
		}
	}*/

	qDebug() << "Wait for incoming connection...";
	_client_socket =  accept(_srv_socket, (struct sockaddr *) &(addr_in),
						   &addr_len);

	if(_client_socket < 0) {
		qDebug() << "Socket: Cannot establish connection";
		qDebug() << strerror(errno);

		return false;
	}

	_ip = QString(inet_ntoa( addr_in.sin_addr));

	qDebug() << "Connected to " << _ip;

	emit sig_new_connection(_ip);
	emit sig_new_connection();
	_connected = true;

	return true;
}


void Socket::client_reset(){
	_client_socket = -1;

	_send_data = false;
	_connected = false;
	_icy = false;
	_bytes_written = 0;

	_port = 1024;
	_user_agent = "";
	_ip = "";

}

void Socket::client_disconnect() {

	if(_client_socket > 0){

		if( shutdown(_client_socket, 2) < 0 ){
			qDebug() << "Cannot close client socket";
			qDebug() << strerror(errno);
		}

		/*while(check_for_new_data()){
			if( read(_client_socket, msg, 4095) <= 0){
				break;
			}

			else{
				qDebug() << msg;
			}

			msleep(1000);
		}*/

		close(_client_socket);
	}

	emit sig_connection_closed(_ip);

	client_reset();

	qDebug() << "Client disconnected";

}

void Socket::server_disconnect(){
	if(_srv_socket > 0){

		if( close(_srv_socket) < 0){
			qDebug() << "Cannot close server socket";
			qDebug() << strerror(errno);
		}

		_srv_socket = -1;

		qDebug() << "Server disconnected";
	}

	else{
		qDebug() << "Server already disconnected";
	}
}

void Socket::connection_valid(bool b){

	if(!b) {
		server_disconnect();
		client_disconnect();
	}
}


void Socket::new_data(uchar* data, quint64 size){

	if(!_connected) return;
	if(!_send_data) return;

	qint64 n_bytes;

	n_bytes = send_stream_data(data, size);

#ifdef SOCKET_DEBUG
	//qDebug() << "Wrote " << n_bytes << " bytes";
#endif

	if(n_bytes == -1){
		qDebug() << "Client quit. Close connection...";
		client_disconnect();
	}
}



bool Socket::send_header(){

	bool success;

	QByteArray header = _header;
	if(_icy){
		header.append(_icy_header);
	}

	header.append("\r\n");

	ssize_t n_bytes = write(_client_socket, header.constData(), header.size());
	success = (n_bytes >= header.size());
	if(!success){
		qDebug() << "Could not send header";
	}

	return success;
}



void Socket::send_reject_header(){

	char* msg = "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\n\r\n";
	write(_client_socket, msg, 57);

}


bool Socket::send_icy_data(){

	bool success;
	ssize_t n_bytes=0;
	QByteArray metadata = QByteArray("StreamTitle='");
	metadata.append(_stream_title.toLocal8Bit());
	metadata.append("';");
	metadata.append("StreamUrl='http://sayonara.luciocarreras.de';");

	int sz = metadata.size();
	int n_padding = ( (int)((sz + 15) / 16) * 16 - sz );

	metadata.append(padding, n_padding);
	metadata.prepend((char) (int)((sz + 15) / 16));

	n_bytes = write(_client_socket, metadata.constData(), metadata.size());
	success = (n_bytes >= metadata.size());
	return success;
}

qint64 Socket::send_stream_data(uchar* data , quint64 size){

	ssize_t n_bytes=0;

#ifdef SOCKET_DEBUG
	//qDebug() << _bytes_written + size << ";" << _icy;
#endif

	if(_bytes_written + size > 8192){

		if( _icy ){
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
			_bytes_written = 0; // avoid overflow.
		}
	}

	else{
		n_bytes = write(_client_socket, data, size);
		_bytes_written += n_bytes;
	}

#ifdef SOCKET_DEBUG
	//qDebug() << _bytes_written;
#endif

	return n_bytes;
}

void Socket::psl_update_track(const MetaData& md){
	_stream_title = "Sayonara Player: " + md.title + " by " + md.artist;
}

void Socket::stop(){
	client_disconnect();
	server_disconnect();
}
