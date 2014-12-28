/* StreamWriter.h */

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



#include "HelperStructs/MetaData.h"
#include "HelperStructs/SayonaraClass.h"

#include <QByteArray>
#include <QTcpSocket>
#include <QHostAddress>


enum HttpAnswer {
	HttpAnswerFail=0,
	HttpAnswerOK,
	HttpAnswerReject,
	HttpAnswerIgnore,
	HttpAnswerPlaylist,
	HttpAnswerMP3
};


class StreamWriter : protected SayonaraClass {

	public:
		StreamWriter(QTcpSocket* socket);
		virtual ~StreamWriter();


	private:

		QTcpSocket* _socket;

		bool _dismissed;
		bool _send_data;
		bool _icy;


		QByteArray _icy_header;
		QByteArray _header;
		QByteArray _reject_header;
		QString _stream_title;
		QString _user_agent;
		QString _host;
		quint64 _sent_bytes;


		void reset();
		void create_headers();
		bool send_icy_data();

	public:
		QString get_ip();
		QString get_user_agent();
		int get_sd();

		void change_track(const MetaData& md);
		bool send_header(bool reject);
		bool send_playlist(const MetaData& md);
		bool send_html5();

		HttpAnswer parse_message();

		bool send_data(const uchar*, quint64 size);
		void disconnect();
		void enable();
		void disable();

};
