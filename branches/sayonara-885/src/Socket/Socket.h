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

#define BUFFER_SIZE 1024

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
	void sig_new_fd(int);

public:
	Socket();
	virtual ~Socket();

public slots:
	void new_data(uchar* data, quint64 size);

private:
	int		_srv_socket;
	int 	_client_socket;
	int		_port;
	int		_port_to;
	bool 	_connected;
	struct sockaddr_in _srv_info;
	struct sockaddr_in _cli_info;

	bool sock_connect();
	void sock_disconnect();


	uchar buffer[BUFFER_SIZE];
	int _idx;

};

#endif /* SOCKET_H_ */
