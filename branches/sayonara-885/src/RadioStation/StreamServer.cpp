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


	while( !_server->isListening() ){

		listen_for_connection();
		Helper::sleep_ms(1000);
	}



	REGISTER_LISTENER(Set::BroadCast_Active, active_changed);
	REGISTER_LISTENER(Set::Broadcast_Port, port_changed);
	REGISTER_LISTENER(Set::Broadcast_Prompt, prompt_changed);
}

StreamServer::~StreamServer(){

	Helper::sleep_ms(500);
	while(_server){
		Helper::sleep_ms(1000);
	}
}


void StreamServer::run(){

    qDebug() << "Start stream server";


	forever{
		while(_server && _server->isListening()){
			Helper::sleep_ms(100);
		}

		if(_server == NULL){
			break;
		}

		Helper::sleep_ms(1000);
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
		_server->errorString();
		_server->close();

		return false;
	}

	return true;
}


void StreamServer::new_client_request(){

	QTcpSocket* socket = _server->nextPendingConnection();
	if(!socket) return;

	QString ip = socket->peerAddress().toString();
	_queue_map[ip] = socket;

	emit sig_new_connection_request(ip);
}

void StreamServer::accept_client(const QString& ip){

	QTcpSocket* s = _queue_map[ip];

	SocketWriter* sw = new SocketWriter(s);

	HttpAnswer answer = sw->parse_message();

	switch(answer){

		case HttpAnswerFail:
		case HttpAnswerReject:
			qDebug() << "Rejected: " << sw->get_user_agent() << ": " << sw->get_ip();
			sw->send_header(true);
			disconnect(sw);
			break;

		case HttpAnswerIgnore:
			break;

		case HttpAnswerPlaylist:
			sw->send_playlist(_md);
			disconnect(sw);
			break;

		case HttpAnswerMP3:
			sw->send_html5();
			break;

		default:
			qDebug() << "Accepted: " << sw->get_user_agent() << ": " << sw->get_ip();
			emit sig_new_connection(ip);

			sw->send_header(false);
			sw->change_track(_md);

			_lst_sw << sw;
			_settings->set(SetNoDB::Broadcast_Clients, ++_n_clients);
	}

	_queue_map.remove(ip);
}


void StreamServer::reject_client(const QString& ip){
	_queue_map.remove(ip);
}


void StreamServer::new_data(uchar* data, quint64 size){

	QList<int> idx_list;
	int idx = 0;

	foreach(SocketWriter* sw, _lst_sw){

		bool success = sw->send_data(data, size);

		if(!success){

			qDebug() << "Connection closed by client "
					 << sw->get_user_agent()
					 << " (" << sw->get_ip() << ")";

			idx_list << idx;
			disconnect(sw);
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
	_md = md;
	foreach(SocketWriter* sw, _lst_sw){
		sw->change_track(md);
	}
}

void StreamServer::server_close(){
	
	if(_server){
		_server->close();
	}

}

// when user forbids further streaming
void StreamServer::dismiss(int idx){

	if( idx >= _lst_sw.size() ) return;

	SocketWriter* sw = _lst_sw[idx];

	sw->disable();

	if(_n_clients > 0) _n_clients--;
	_settings->set(SetNoDB::Broadcast_Clients, _n_clients);

}

// real socket disconnect (if no further sending is possible)
void StreamServer::disconnect(SocketWriter* sw){

	emit sig_connection_closed(sw->get_ip());

	sw->disconnect();

	int idx = _lst_sw.indexOf(sw);
	if(idx >= 0 && idx < _lst_sw.size()){
		_lst_sw.removeAt(idx);
	}

	if(_n_clients > 0) _n_clients--;
	_settings->set(SetNoDB::Broadcast_Clients, _n_clients);

	delete sw;
	sw = 0;
}

void StreamServer::disconnect_all(){

	foreach(SocketWriter* sw, _lst_sw){
		sw->disconnect();
		delete sw;
		sw = 0;
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
        server_close();
    }

    else{
        this->start();
    }
}

void StreamServer::prompt_changed(){
	_prompt = _settings->get(Set::Broadcast_Prompt);
}

void StreamServer::port_changed(){
	//_port = _settings->get(Set::Broadcast_Port);
}
