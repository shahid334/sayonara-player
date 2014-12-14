#include "RadioStation/StreamWriter.h"
#include "HelperStructs/Helper.h"

static char padding[256];

StreamWriter::StreamWriter(QTcpSocket* socket) : SayonaraClass()
{

	create_headers();
	reset();

	memset(padding, 0, 256);

	_stream_title = "Sayonara Radio";
	_socket = socket;

}

StreamWriter::~StreamWriter(){

}

QString StreamWriter::get_ip(){
	return _socket->peerAddress().toString();
}

int StreamWriter::get_sd(){
	return _socket->socketDescriptor();
}

QString StreamWriter::get_user_agent(){
	return _user_agent;
}

void StreamWriter::reset(){
	_sent_bytes = 0;
	_send_data = false;
	_dismissed = false;
	_icy = false;
	_stream_title = "";
	_user_agent = "";
	_host = "";
}

void StreamWriter::create_headers(){
	
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


HttpAnswer StreamWriter::parse_message(){

	_host = "";

	bool get_playlist = false;
	bool get_received = false;
	bool get_mp3 = false;
	bool icy=false;
	bool is_browser=false;


	QString qmsg;
	QStringList lst;
	QByteArray msg;

	msg = _socket->read(8192);


	if(msg.size() == 0) {
		return HttpAnswerFail;
	}

	qmsg = QString(msg);
	lst = qmsg.split("\r\n");
	_icy = false;

	foreach(QString str, lst){

		qDebug() << str;

		QRegExp regex("(GET)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_pl("(GET)(\\s|/)*(playlist.m3u)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_mp3("(GET)(\\s|/)*(track.mp3)(\\s|/)*HTTP", Qt::CaseInsensitive);

		if(str.contains(regex)){
			get_received = true;
			continue;
		}

		if(str.contains(regex_pl)){
			qDebug() << "get Playlist";
			get_playlist = true;
			continue;
		}

		if(str.contains(regex_mp3)){
			qDebug() << "get mp3";
			get_received = true;
			get_mp3 = true;
			continue;
		}


		if(str.toLower().contains("host:")){
			QStringList lst = str.split(":");
			if(lst.size() > 1){

				_host = lst[1].trimmed();
				qDebug() << "Host = " << _host;

			}
		}

		if( str.contains("icy-metadata:", Qt::CaseInsensitive) ){
			if(str.contains(":1") || str.contains(": 1")){
				icy = true;
				continue;
			}
		}

		if(str.contains("user-agent", Qt::CaseInsensitive)){

			if(str.size() > 11){
				_user_agent = str.right( str.size() - 11).toLower();
				if( _user_agent.contains("firefox", Qt::CaseInsensitive) ||
					_user_agent.contains("safari", Qt::CaseInsensitive) ||
					_user_agent.contains("internet explorer", Qt::CaseInsensitive) ||
					_user_agent.contains("opera", Qt::CaseInsensitive))
				{
						is_browser = true;
				}
			}
		}
	}

	if(is_browser && !get_mp3 && !_host.isEmpty()){
		return HttpAnswerMP3;
	}


	if(get_playlist && !_host.isEmpty()){
		return HttpAnswerPlaylist;
	}

	if(get_received){
		_icy = icy;

		return HttpAnswerOK;
	}

	return HttpAnswerFail;
}

bool StreamWriter::send_playlist(const MetaData& md){

	qint64 n_bytes;
	int port = _socket->localPort();

	QByteArray pl = QByteArray("#EXTM3U\n\n"
			"#EXTINF:-1, Lucio Carreras - Sayonara Player Radio\n"
			"http://" + _host.toLocal8Bit() +
			":" + QString::number(port).toLocal8Bit() + "\n");

	QByteArray data = QByteArray("HTTP/1.1 200 OK\r\n"
									"content-type:audio/x-mpegurl\r\n"
									"Content-Length: " + QString::number(pl.size()).toLocal8Bit() + "\r\n"
									"Connection: keep-alive\r\n\r\n" +
									pl
								 );

	n_bytes = _socket->write(data);

	return (n_bytes > 0);
}



bool StreamWriter::send_html5(){

	int n_bytes;

	QByteArray html = QByteArray(
			"<html>"
				"<head>"
				"</head>"
				"<body>"
				"<audio autoplay controls>"
					"<source src=\"track.mp3\" type=\"audio/mpeg\">"
					"Your browser does not support the audio element."
				"</audio>"
				"</body>"
			"</html>");

	QByteArray data = QByteArray("HTTP/1.1 200 OK\r\n"
								   "content-type: text/html\r\n"
								   "content-length: " + QString::number(html.size()).toLocal8Bit() +
								   "\r\nConnection: keep-alive\r\n\r\n" +
								   html
								   );

	//qDebug() << "send html5 " << data;

	n_bytes = _socket->write(data);

	return (n_bytes > 0);
}


bool StreamWriter::send_header(bool reject){
	
	qint64 n_bytes;	
	bool success;

	if(reject){
		n_bytes = _socket->write( _reject_header );
	}

	else if(_icy){
		n_bytes = _socket->write( _icy_header );
	}

	else{
		n_bytes = _socket->write( _header );
	}

	success = (n_bytes > 0);

    if(success && !reject){
        enable();
	}

	else{
		_send_data = false;	
	}
	
	return success;
}


void StreamWriter::change_track(const MetaData& md){
	_stream_title = md.title + " by " + md.artist;
}


bool StreamWriter::send_icy_data(){
	
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

	n_bytes = _socket->write( metadata );

    success = (n_bytes > 0);

    return success;
}


bool StreamWriter::send_data(const uchar* data, quint64 size){

	bool success;
	qint64 n_bytes=0;	

	if(!_send_data){
		return true;
	}

	if(_dismissed) {

		QByteArray c(1, 0x00);
		n_bytes = _socket->write(c);
		return (n_bytes > 0);
	}

	if (_icy){
		if(_sent_bytes + size > 8192){

			quint64 bytes_before = 8192 - _sent_bytes;

			n_bytes = _socket->write( (const char*) data, bytes_before);

            send_icy_data();

            if(size - bytes_before > 0){
				n_bytes = _socket->write( (const char*) (data + bytes_before), size - bytes_before);
                _sent_bytes = n_bytes;
            }

			else{
				_sent_bytes = 0;
			}
        }

		else{
			n_bytes = _socket->write( (const char*) data, size);
			_sent_bytes += n_bytes;
		}
    }

    else{
		n_bytes = _socket->write( (const char*) data, size);
		_sent_bytes = 0;
    }

	success = (n_bytes > 0);

	return success;
}

void StreamWriter::disable(){

    if(_dismissed) return;

	_dismissed = true;
}

void StreamWriter::enable(){

    _send_data = true;
}

void StreamWriter::disconnect(){
	
	disable();
	
	_socket->close();

	reset();
}
