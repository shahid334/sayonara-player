#include "Socket/StreamServer.h"


StreamServer::StreamServer(QObject* parent) : 
	QThread(parent),
	_socket(-1),
	_port(1024),
	_reject(false)
{

}

StreamServer::~StreamServer(){
	this->close();
	disconnect_all();
}

bool StreamServer::create_socket(){
	
	int status;
    int reuse_addr = 1;
	SocketAdress srv_adress;

    _socket = socket(AF_INET, SOCK_STREAM, 0);

    memset( &srv_adress, 0, sizeof(srv_adress));

    srv_adress.sin_family = AF_INET;
    srv_adress.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_adress.sin_port = htons(_port);

    if(_socket < 0){
        qDebug() << "Server socket invalid";
        qDebug() << strerror(errno);

        return false;
    }

    status = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
    if(status < 0){
        qDebug() << "Reuse adress: " << strerror(errno);
    }


    status = bind(_socket, (SocketAdress_t*) &srv_adress, sizeof(srv_adress));
    if(status < 0){
        qDebug() << "Cannot bind port " << _port;
        qDebug() << strerror(errno);
        this->close();

        return false;
    }

    qDebug() << "Listening on port " << _port;
    status = listen(_socket, 10);
    if(status < 0){
        qDebug() << "Cannot listen to port " << _port;
        qDebug() << strerror(errno);

		this->close();

        return false;
    }

    return true;

}

void StreamServer::run(){
	bool success;
	SocketWriter* sw;

	while(!create_socket()){
		msleep(1000);
	}

	while(_socket != -1){

		sw = this->accept();
		if(!sw) continue;

		sw->send_header(_reject);
		sw->enable();
	}
}

SocketWriter* StreamServer::accept(){
	
	int client_socket;
	QString client_ip;
	SocketWriter* sw=NULL;

	socklen_t addr_len;
    SocketAdress addr_in;
    memset( &addr_in, 0, sizeof(addr_in));

    addr_len = (socklen_t) sizeof(SocketAdress);

    qDebug() << "Wait for incoming connection...";
    client_socket =  accept(_srv_socket, (SocketAdress_t*) &(addr_in),
                           &addr_len);

	if(client_socket < 0) return NULL;

	client_ip = QString(inet_ntoa( addr_in.sin_addr ));

	sw = new SocketWriter(client_socket, client_ip);
	_lst_sw << sw;

	return sw;
}

void StreamServer::new_data(uchar* data, quint64 size){

	qint64 n_bytes;
	QList idx_list;
	int idx = 0;

	foreach(SocketWriter* sw, _lst_sw){

		n_bytes = sw->send_data(data, size);

		if(n_bytes < 0){
			idx_list << idx;
			sw->disconnect();
			sw->deleteLater();
		}

		idx++;
	}

	foreach(int idx, idx_list){
		_lst_sw.removeAt(idx);
	}
}

void StreamServer::close(){
	
	if( shutdown(_socket, 2) < 0 ){
		qDebug() << "Cannot shutdown server socket";
		qDebug() << strerror(errno);
	}

	if( close(_socket) < 0 ){
		qDebug() << "Cannot close server socket";
		qDebug() << strerror(errno);
	}

	_socket = -1;
}

void StreamServer::disconnect(int idx){

	SocketWriter* sw = _lst_sw[idx];

	sw->disconnect();
	sw->deleteLater();

	_lst_sw->removeAt(idx);
}

void StreamServer::disconnect_all(){

	foreach(SocketWriter* sw, _lst_sc){
		sw->disconnect();
		sw->deleteLater();
	}

	_lst_sw->clear();
}
