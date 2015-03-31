/* StreamServer.h */

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

#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include <QThread>
#include <QList>
#include <QTcpSocket>
#include <QTcpServer>
#include "RadioStation/StreamWriter.h"
#include "HelperStructs/SayonaraClass.h"
#include "PlayManager.h"


enum AcceptState{
	AcceptStateNone=-1,
	AcceptStateWaiting=0,
	AcceptStateRejected=1,
	AcceptStateAccepted=2,
	AcceptStateTimeout=3
};

class StreamServer : public QThread, protected SayonaraClass {

	Q_OBJECT

	signals:
		void sig_new_connection_request(const QString&);
		void sig_new_connection(const QString&);
		void sig_connection_closed(const QString&);
		void sig_can_listen(bool);

	public:
		StreamServer(QObject* parent=0);
		~StreamServer();

	private:

		MetaData	_md;

		bool		_prompt;
		bool		_active;
		int			_port;
		int			_n_clients;

		QTcpServer*					_server;
		QTcpSocket*					_pending_socket;
		QList<QTcpSocket*>			_sockets;

		QMap<QString, QTcpSocket*>	_queue_map;

		QList<StreamWriter*>		_lst_sw;
		QStringList					_allowed_ips;
		PlayManager*				_play_manager;

		bool listen_for_connection();


	protected:
		void run();

	public slots:
		void new_data(uchar* data, quint64 size);
		void accept_client();
		void reject_client();
		void dismiss(int idx);

		void disconnect(StreamWriter* sw);
		void disconnect_all();

		void server_close();
		void stop();
		void retry();


	private slots:
		void track_changed(const MetaData&);
		void server_destroyed();
		void active_changed();
		void port_changed();
		void prompt_changed();
		void mp3_enc_found();
		void new_client_request();
		void disconnected(StreamWriter* sw);
		void new_connection(const QString& ip);

};

#endif
