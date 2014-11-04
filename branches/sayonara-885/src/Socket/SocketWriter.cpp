#include "Socket/SocketWriter.h"

SocketWriter::SocketWriter(int socket, QString ip) :
	_socket(socket),
	_ip(ip),
	_send_data(false)
{
	create_headers();
	reset();
}

SocketWriter::~SocketWriter(){
	
}

QString SocketWriter::get_ip(){
	return _ip;
}

int SocketWriter::get_sd(){
	return _socket;
}

SocketWriter::reset(){
	_sent_bytes = 0;
	_send_data = false;
	_icy = bool;
	_stream_title = "";
	_ip = "";
	_socket = -1;
}

void SocketWriter::create_headers(){
	
	_header = QByteArray("ICY 200 Ok\r\n"
                       "icy-notice1:Bliblablupp\r\n"
                       "icy-notice2:asdfasd\r\n"
                       "icy-name:Awesome Sayonara stream\r\n"
                       "icy-genre:None\r\n"
                       "icy-url:http://sayonara.luciocarreras.de\r\n"
                       "content-type:audio/mpeg\r\n"
					   "connection:close\r\n"
    		);

	_icy_header = _header + 
			QByteArray("icy-metaint:8192\r\n"
                   "icy-pub:1\r\n"
                   "icy-br:128\r\n"
			);
	);

	_reject_header = QByteArray("HTTP/1.1 501 Not Implemented\r\nConnection: close\r\n");


	_header.append("\r\n");
	_icy_header.append("\r\n");
	_reject_header("\r\n");
}


bool SocketWriter::send_header(bool reject){
	
	qint64 n_bytes;	
	bool success;
	bool send_data = !recect;



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
	_stream_title = "Sayonara Player: " + md.title + " by " + md.artist;
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


bool SocketWriter::send_data(const uchar*, quint64 size){

	bool success;
	qint64 n_bytes=0;	

	if(!_send_data) {
		n_bytes = write(_socket, NULL, 0);
		return (n_bytes >= 0);
	}

    if(_sent_bytes + size > 8192){

        if( _icy ){
            quint64 bytes_before = 8192 - _sent_bytes;

            n_bytes = write(_socket, data, bytes_before);

            send_icy_data();

            _sent_bytes = 0;

            if(size - bytes_before > 0){
                n_bytes += write(_socket, data + bytes_before, size - bytes_before);
                _sent_bytes = n_bytes;
            }
        }

        else{
            _sent_bytes = 0; // avoid overflow.
        }
    }

    else{
        n_bytes = write(_socket, data, size);
        _sent_bytes += n_bytes;
    }

	success = (n_bytes > 0);

	return success;
}

void SocketWriter::disable(){
	_send_data = false;
}

void SocketWriter::enable(){
	_send_data = true;
}

void SocketWriter::disconnect(){
	
	disable();
	
	if( shutdown(_socket, 2) < 0 ){
		qDebug() << "Cannot shutdown client socket";
		qDebug() << strerror(errno);
	}

	if( close(_socket) < 0 ){
		qDebug() << "Cannot close client socket";
		qDebug() << strerror(errno);
	}

	reset();
}
