/* Socket.h

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

#ifndef SOCKET_H_
#define SOCKET_H_

#include <QObject>
#include <QThread>
#include <netinet/in.h>
#include "HelperStructs/MetaData.h"

#define BUFFER_SIZE 1024

enum HttpAnswer{
	HttpAnswerFail=0,
	HttpAnswerOK=1,
	HttpAnswerReject=2
};


class Socket : public QThread {

	Q_OBJECT

protected:
	void run();

signals:
	void sig_play();
	void sig_pause();
	void sig_stop();
	void sig_prev();
	void sig_next();
	void sig_setVolume(int);

	void sig_new_connection_req(const QString&);
	void sig_new_connection(const QString&);
	void sig_new_connection();
	void sig_connection_closed(const QString&);

public:
	Socket(QObject* parent=0);
	virtual ~Socket();

public slots:
	void new_data(uchar* data, quint64 size);
	void connection_valid(bool);
	void stop();
	void psl_update_track(const MetaData&);

private:

	int		_srv_socket;
	int		_client_socket;

	int		_port;
	bool 	_connected;
	bool	_send_data;
	bool	_icy;

	ssize_t _bytes_written;

	QString _stream_title;
	QString _user_agent;

	QByteArray _header;
	QByteArray _icy_header;

	QString _ip;
	struct sockaddr_in _srv_info;
	struct sockaddr_in _cli_info;


	bool client_connect();
	void client_disconnect();
	void client_reset();
	bool check_for_new_data();

	bool init_server_socket();
	void server_disconnect();

	bool send_header();
	void send_reject_header();
	bool send_icy_data();

	qint64 send_stream_data(uchar* data , quint64 size);



	HttpAnswer parse_message();

};

#endif /* SOCKET_H_ */
