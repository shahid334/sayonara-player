/* StreamWriter.cpp */

/* Copyright (C) 2011-2014  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "RadioStation/StreamWriter.h"
#include "HelperStructs/Helper.h"

static char padding[256];

StreamWriter::StreamWriter(QTcpSocket* socket, const MetaData& md) : SayonaraClass()
{

	create_headers();
	reset();

	memset(padding, 0, 256);

	_stream_title = md.title + " by " + md.artist;
	_socket = socket;

	connect(socket, SIGNAL(disconnected()), this, SLOT(socket_disconnected()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(data_available()));

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
				"connection:keep-alive\r\n"
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
				"connection:keep-alive\r\n"
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
	bool get_bg = false;
	bool get_favicon=false;
	bool get_metadata=false;
	bool icy=false;
	bool is_browser=false;


	QString qmsg;
	QStringList lst;
	QByteArray msg;

	msg = _socket->readAll();


	if(msg.size() == 0) {
		qDebug() << "Fail.. Cannot read from socket";
		return HttpAnswerFail;
	}

	qmsg = QString(msg);
	lst = qmsg.split("\r\n");
	_icy = false;

	foreach(QString str, lst){

		qDebug() << str;

		QRegExp regex("(GET|HEAD)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_pl("(GET)(\\s|/)*(playlist.m3u)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_mp3("(GET)(\\s|/)*(track.mp3)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_bg("(GET)(\\s|/)*(bg-checker.png)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_favicon("(GET)(\\s|/)*(favicon.ico)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_metadata("(GET)(\\s|/)*(metadata)(\\s|/)*HTTP", Qt::CaseInsensitive);



		if(str.contains(regex)){
			get_received = true;
			continue;
		}

		if(str.contains(regex_metadata)){
			get_metadata = true;
		}

		if(str.contains(regex_favicon)){
			get_favicon = true;
			continue;
		}

		if(str.contains(regex_pl)){
			get_playlist = true;
			continue;
		}

		if(str.contains(regex_mp3)){
			get_mp3 = true;
			continue;
		}

		if(str.contains(regex_bg)){
			get_bg = true;
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
					_user_agent.contains("mozilla", Qt::CaseInsensitive) ||
					_user_agent.contains("safari", Qt::CaseInsensitive) ||
					_user_agent.contains("internet explorer", Qt::CaseInsensitive) ||
					_user_agent.contains("opera", Qt::CaseInsensitive) ||
					_user_agent.contains("chrom", Qt::CaseInsensitive))
				{
						is_browser = true;
				}
			}
		}
	}

	if(is_browser && get_favicon && !_host.isEmpty()){
		return HttpAnswerFavicon;
	}

	if(is_browser && get_bg && !_host.isEmpty()){
		return HttpAnswerBG;
	}

	if(is_browser && get_metadata && !_host.isEmpty()){
		return HttpAnswerMetaData;
	}

	if(is_browser && !get_mp3 && !_host.isEmpty()){
		return HttpAnswerHTML5;
	}

	if(is_browser && get_mp3 && !_host.isEmpty()){
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
									"Connection: close\r\n\r\n" +
									pl
								 );

	n_bytes = _socket->write(data);

	return (n_bytes > 0);
}

bool StreamWriter::send_favicon(){
	int n_bytes;
	bool success;
	QByteArray arr;
	success = Helper::read_file_into_byte_arr( Helper::getIconPath() + "favicon.ico", arr );

	qDebug() << "Read favicon " << Helper::getIconPath() << "favicon.ico into arr: " << success << ": " << arr.size();

	if(!success){
		return send_header(true);
	}

	QByteArray data = QByteArray("HTTP/1.1 200 OK\r\n"
								 "content-type: image/x-icon\r\n"
								 "content-length: " + QString::number(arr.size()).toLocal8Bit() +
								 "\r\nConnection: close\r\n\r\n" +
								 arr
								 );


	n_bytes = _socket->write(data);

	qDebug() << "Send favicon";

	return (n_bytes > 0);
}

bool StreamWriter::send_metadata(){

	int n_bytes;

	QByteArray html = _stream_title.toLocal8Bit();
	QByteArray data = QByteArray("HTTP/1.1 200 OK\r\n"
								 "content-type: text/plain\r\n"
								 "content-length: " + QString::number(html.size()).toLocal8Bit() +
								 "\r\nConnection: close\r\n\r\n" +
									html
								 );

	n_bytes = _socket->write(data);

	qDebug() << "Send metadata";

	return (n_bytes > 0);
}


bool StreamWriter::send_bg(){

	int n_bytes;
	QByteArray html;

	bool success = Helper::read_file_into_byte_arr( Helper::getIconPath() + "bg-checker.png", html );
	if(!success){
		return false;
	}

	QByteArray data = QByteArray("HTTP/1.1 200 OK\r\n"
								 "content-type: image/png\r\n"
								 "content-length: " + QString::number(html.size()).toLocal8Bit() +
								 "\r\nConnection: close\r\n\r\n" +
								 html
								 );

	n_bytes = _socket->write(data);

	qDebug() << "Send background";

	return (n_bytes > 0);

}

bool StreamWriter::send_html5(){

	int n_bytes;

	QByteArray html = QByteArray(
			"<!DOCTYPE html>"
			"<html>"
				"<head>"
				"<link rel=\"icon\" href=\"favicon.ico\" type=\"image/x-icon\" />"
				"<title>Sayonara Player Radio</title>"

				"<script>\n"

				"function loadXMLDoc()\n"
				"{\n"
				"var xmlhttp;\n"
				"if (window.XMLHttpRequest)\n"
"				  {// code for IE7+, Firefox, Chrome, Opera, Safari\n"
				  "xmlhttp=new XMLHttpRequest();\n"
				  "}\n"
				"else\n"
"				  {// code for IE6, IE5\n"
				  "xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");\n"
				  "}\n"
				"xmlhttp.onreadystatechange=function()\n"
"				  {\n"
				  "if (xmlhttp.readyState==4 && xmlhttp.status==200)\n"
"				    {\n"
					"document.getElementById(\"metadata\").innerHTML=xmlhttp.responseText;\n"
					"}\n"
				"}\n"
				"xmlhttp.open(\"GET\",\"metadata\",true);\n"
				"xmlhttp.send();\n"
				"}\n"

				"function start(){\n"
				//"setInterval(function(){loadXMLDoc(); }, 3000);\n"
				"}"
				"</script>"


				"</head>"
				"<body background=\"bg-checker.png\" onload=\"start()\" >"

					"<h1 style=\"color: #f3841a; font-family: Fredoka One, lucida grande, tahoma, sans-serif; font-weight: 400;\">Sayonara Player Radio</h1>"
					"<audio id=\"player\" autoplay controls>"
						"<source src=\"track.mp3\" type=\"audio/mpeg\">"
						"Your browser does not support the audio element."
					"</audio><br /><br />"
					"<table><tr><td>"
					"<div id=\"metadata\" style=\"color: white;\">") +
						_stream_title.toLocal8Bit() +
			QByteArray("</div></td>"

					"<td><div style=\"color: white;\">"
						"<button type=\"button\" onclick=\"loadXMLDoc()\">Refresh</button><br /><br />"
					"</div></td></tr></table><br />"
					"<div style=\"color: white;\">"
					"Stream by Lucio Carreras"
					"</div>"
				"</body>"
			"</html>");

	QByteArray data = QByteArray("HTTP/1.1 200 OK\r\n"
								   "content-type: text/html\r\n"
								   "content-length: " + QString::number(html.size()).toLocal8Bit() +
								   "\r\nConnection: keep-alive\r\n\r\n" +
								   html
								   );

	qDebug() << "send html5 ";

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

	_md = md;
	_stream_title = md.title + " by " + md.artist;
	qDebug() << "Update stream title " << _stream_title;
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


void StreamWriter::socket_disconnected(){
	emit sig_disconnected(this);
}


void StreamWriter::data_available(){

	qDebug() << "***New data available***";

	HttpAnswer answer = parse_message();
	QString ip = get_ip();

	switch(answer){

		case HttpAnswerFail:
		case HttpAnswerReject:
			qDebug() << "Rejected: " << get_user_agent() << ": " << get_ip();
			send_header(true);
			break;

		case HttpAnswerIgnore:
			qDebug() << "ignore...";
			break;

		case HttpAnswerPlaylist:
			qDebug() << "Asked for playlist";
			send_playlist(_md);
			break;

		case HttpAnswerHTML5:
			qDebug() << "Asked for html5";
			send_html5();
			break;

		case HttpAnswerBG:
			qDebug() << "Asked for background";
			send_bg();
			break;

		case HttpAnswerFavicon:
			qDebug() << "Asked for favicon";
			send_favicon();
			break;

		case HttpAnswerMetaData:
			qDebug() << "Asked for metadata";
			send_metadata();
			break;

		default:
			qDebug() << "Accepted: " << get_user_agent() << ": " << ip;
			send_header(false);
			emit sig_new_connection(ip);
			break;
	}
}
