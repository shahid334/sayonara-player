#include "RadioStation/SocketWriter.h"


static char padding[256];

SocketWriter::SocketWriter(int socket, QString ip)

{

	create_headers();
	reset();

	memset(padding, 0, 256);

	_stream_title = "Sayonara Radio";

	_socket = socket;
	_ip = ip;

}

SocketWriter::~SocketWriter(){
	
}

QString SocketWriter::get_ip(){
	return _ip;
}

int SocketWriter::get_sd(){
	return _socket;
}

QString SocketWriter::get_user_agent(){
	return _user_agent;
}

void SocketWriter::reset(){
	_sent_bytes = 0;
	_send_data = false;
	_dismissed = false;
	_icy = false;
	_stream_title = "";
	_ip = "";
	_socket = -1;
	_user_agent = "";
	_host = "";
}

void SocketWriter::create_headers(){
	
	_header = QByteArray(
				"ICY 200 Ok\r\n"
				"icy-notice1:Bliblablupp\r\n"
				"icy-notice2:asdfasd\r\n"
				"icy-name:Sayonara Player Radio\r\n"
				"icy-genre:Angry songs\r\n"
				"icy-url:http://sayonara.luciocarreras.de\r\n"
				"icy-pub:1\r\n"
				"icy-br:192\r\n"
				"content-type:audio/mpeg\r\n"
				"connection:close\r\n"
			);

	_icy_header = QByteArray(
				"ICY 200 Ok\r\n"
				"icy-notice1:Bliblablupp\r\n"
				"icy-notice2:asdfasd\r\n"
				"icy-name:Sayonara Player Radio\r\n"
				"icy-genre:Angry songs\r\n"
				"icy-url:http://sayonara.luciocarreras.de\r\n"
				"icy-pub:1\r\n"
				"icy-br:192\r\n"
				"icy-metaint:8192\r\n"
				"content-type:audio/mpeg\r\n"
				"connection:close\r\n"
			);


	_reject_header = QByteArray("HTTP/1.1 501 Not Implemented\r\nConnection: close\r\n");


	_header.append("\r\n");
	_icy_header.append("\r\n");
	_reject_header.append("\r\n");
}

char msg[4096];
HttpAnswer SocketWriter::parse_message(){


	_host = "";
	ssize_t n_bytes;

	bool get_playlist = false;
	bool get_received = false;
	bool icy=false;


	QString qmsg;
	QStringList lst;


	n_bytes = read(_socket, msg, 4095);
	if(n_bytes < 0) {
			return HttpAnswerFail;
	}

	qmsg = QString::fromLocal8Bit(msg, n_bytes);

	lst = qmsg.split("\r\n");
	_icy = false;

	QMap<QString, QString> map;

	foreach(QString str, lst){

		qDebug() << str;

		QRegExp regex("(GET|HEAD)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_pl("(GET)(\\s|/)*(playlist.m3u)(\\s|/)*HTTP", Qt::CaseInsensitive);

		if(str.contains(regex)){
			get_received = true;
			continue;
		}

		if(str.contains(regex_pl)){
			qDebug() << "get Playlist";
			get_playlist = true;
			continue;
		}

		if(str.toLower().contains("host:")){
			QStringList lst = str.split(":");
			if(lst.size() > 1){

				_host = lst[1].trimmed();
				qDebug() << "Host = " << _host;
			}
		}

		/*if(str.contains("keep-alive", Qt::CaseInsensitive)){
				qDebug() << "Reject keep alive";
				return HttpAnswerReject;
		}*/

		if( str.contains("icy-metadata:", Qt::CaseInsensitive) ){
			if(str.contains(":1") || str.contains(": 1")){
				icy = true;
				continue;
			}
		}

		if(str.contains("user-agent", Qt::CaseInsensitive)){

			if(str.size() > 11){
				_user_agent = str.right( str.size() - 11).toLower();
			}
		}
	}

	if(get_playlist && !_host.isEmpty()){
		return HttpAnswerPlaylist;
	}

	/*if(_user_agent.isEmpty()){
			return HttpAnswerReject;
	}*/


	if(get_received){

		_icy = icy;

		return HttpAnswerOK;
	}

	return HttpAnswerFail;
}

bool SocketWriter::send_playlist(int port, const MetaData& md){

	qint64 n_bytes;

	QByteArray pl = QByteArray("#EXTM3U\n\n"
			"#EXTINF:-1, Lucio Carreras - Sayonara Player Radio\n"
			"http://" + _host.toLocal8Bit() +
			":" + QString::number(port).toLocal8Bit() + "\n");

	QByteArray data = QByteArray("HTTP/1.1 200 OK\r\n"
									"content-type:audio/x-mpegurl\r\n"
									"Content-Length: " + QString::number(pl.size()).toLocal8Bit() +
									"Connection: keep-alive\r\n\r\n" +
									pl
								 );

	n_bytes = write(_socket, data, data.length());
	return (n_bytes > 0);
}

bool SocketWriter::send_header(bool reject){
	
	qint64 n_bytes;	
	bool success;
	bool send_data = !reject;

	if(reject){
		n_bytes = write( _socket, _reject_header.constData(), _reject_header.size() );
	}

	else if(_icy){
		n_bytes = write( _socket, _icy_header.constData(), _icy_header.size() );	
	}

	else{
		n_bytes = write( _socket, _header.constData(), _header.size() );
	}

	success = (n_bytes > 0);

	if(success){
		_send_data = send_data;
	}

	else{
		_send_data = false;	
	}
	
	return success;
}


void SocketWriter::change_track(const MetaData& md){
	_stream_title = md.title + " by " + md.artist;
}


bool SocketWriter::send_icy_data(){
	
	bool success;
    qint64 n_bytes=0;
    QByteArray metadata = QByteArray("StreamTitle='");


    metadata.append( _stream_title.toLocal8Bit() );
    metadata.append("';");
    metadata.append("StreamUrl='http://sayonara.luciocarreras.de';");

    int sz = metadata.size();
    int n_padding = ( (int)((sz + 15) / 16) * 16 - sz );

    metadata.append(padding, n_padding);
    metadata.prepend((char) (int)((sz + 15) / 16));

    n_bytes = write(_socket, metadata.constData(), metadata.size());

    success = (n_bytes > 0);

    return success;
}


bool SocketWriter::send_data(const uchar* data, quint64 size){

	bool success;
	qint64 n_bytes=0;	

	if(!_send_data){
		return true;
	}

	if(_dismissed) {

		char c = 0x00;
		n_bytes = write(_socket, &c, 1);
		return (n_bytes > 0);
	}

	if (_icy){
		if(_sent_bytes + size > 8192){

			quint64 bytes_before = 8192 - _sent_bytes;

            n_bytes = write(_socket, data, bytes_before);

            send_icy_data();

            if(size - bytes_before > 0){
				n_bytes = write(_socket, data + bytes_before, size - bytes_before);
                _sent_bytes = n_bytes;
            }

			else{
				_sent_bytes = 0;
			}
        }

		else{
			n_bytes = write(_socket, data, size);
			_sent_bytes += n_bytes;
		}
    }

    else{
        n_bytes = write(_socket, data, size);
		_sent_bytes = 0;
    }

	success = (n_bytes > 0);

	return success;
}

void SocketWriter::disable(){
	_dismissed = true;
}

void SocketWriter::enable(){
	_send_data = true;
}

void SocketWriter::disconnect(){
	
	disable();
	
	if( close(_socket) < 0 ){
		qDebug() << "Cannot close client socket";
		qDebug() << strerror(errno);
	}

	reset();
}
