#include "RadioStation/StreamServer.h"


StreamServer::StreamServer(QObject* parent) : 
	QThread(parent),
	_socket(-1),
	_port(1024),
	_reject(false)
{

}

StreamServer::~StreamServer(){
	server_close();
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
		server_close();

        return false;
    }

    qDebug() << "Listening on port " << _port;
    status = listen(_socket, 10);
    if(status < 0){
        qDebug() << "Cannot listen to port " << _port;
        qDebug() << strerror(errno);

		server_close();

        return false;
    }

    return true;

}

void StreamServer::run(){

	SocketWriter* sw;
	HttpAnswer answer;

	while(!create_socket()){
		msleep(1000);
	}

	while(_socket != -1){


		msleep(1000);

		sw = client_accept();
		if(!sw) continue;


		answer = sw->parse_message();
		if( answer == HttpAnswerReject ){
			sw->send_header(true);
			qDebug() << "Rejected: " << sw->get_user_agent() << ": " << sw->get_ip();
			continue;
		}

		else if( answer == HttpAnswerFail ){
			sw->send_header(true);
			qDebug() << "Rejected: " << sw->get_user_agent() << ": " << sw->get_ip();
			continue;
		}

		else if( answer == HttpAnswerIgnore){
		}

		else{
			sw->send_header(false);
			qDebug() << "Accepted: " << sw->get_user_agent() << ": " << sw->get_ip();
		}
	}
}

SocketWriter* StreamServer::client_accept(){
	
	int client_socket;
	QString client_ip;
	SocketWriter* sw=NULL;

	socklen_t addr_len;
	SocketAdress addr_in;
    memset( &addr_in, 0, sizeof(addr_in));

    addr_len = (socklen_t) sizeof(SocketAdress);
/*
fd_set read_fd_set, active_fd_set;
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
}

	*/


    qDebug() << "Wait for incoming connection...";
	client_socket =  accept(_socket, (SocketAdress_t*) (&addr_in),
						   &addr_len);

	if(client_socket < 0) return NULL;

	client_ip = QString(inet_ntoa( addr_in.sin_addr ));

	sw = new SocketWriter(client_socket, client_ip);
	_lst_sw << sw;

	emit sig_new_connection( sw->get_ip() );

	return sw;
}

void StreamServer::new_data(uchar* data, quint64 size){


	QList<int> idx_list;
	int idx = 0;

	foreach(SocketWriter* sw, _lst_sw){

		bool success;
		success = sw->send_data(data, size);

		if(!success){
			idx_list << idx;

			qDebug() << "Connection closed by client " << sw->get_user_agent() << " (" << sw->get_ip() << ")";
			emit sig_connection_closed(sw->get_ip());
			sw->disconnect();

/*			delete sw;
			sw = 0;*/
		}

		idx++;
	}

	foreach(int idx, idx_list){
		if(idx < _lst_sw.size()){
			_lst_sw.removeAt(idx);
		}
	}
}

void StreamServer::update_track(const MetaData & md){
	foreach(SocketWriter* sw, _lst_sw){
		sw->change_track(md);
	}
}

void StreamServer::server_close(){
	
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

	if(idx >= _lst_sw.size()) return;

	SocketWriter* sw = _lst_sw[idx];

	emit sig_connection_closed( sw->get_ip() );

	sw->disconnect();

	/*delete sw;
	sw = 0;*/

	_lst_sw.removeAt(idx);

}

void StreamServer::disconnect_all(){

	foreach(SocketWriter* sw, _lst_sw){
		sw->disconnect();
		/*delete sw;
		sw = 0;*/
	}

	_lst_sw.clear();
}
