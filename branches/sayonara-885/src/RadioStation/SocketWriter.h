#include "HelperStructs/MetaData.h"
#include <QByteArray>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

enum HttpAnswer {
	HttpAnswerFail=0,
	HttpAnswerOK=1,
	HttpAnswerReject=2,
	HttpAnswerIgnore=3,
	HttpAnswerPlaylist=4
};


class SocketWriter {

	public:
		SocketWriter(int socket, QString ip);
		virtual ~SocketWriter();


	private:
		QString _host;
		int _socket;
		bool _dismissed;
		bool _send_data;
		bool _icy;


		quint64 _sent_bytes;
		QString _ip;

		QByteArray _icy_header;
		QByteArray _header;
		QByteArray _reject_header;
		QString _stream_title;
		QString _user_agent;


		void reset();
		void create_headers();
		bool send_icy_data();

	public:
		QString get_ip();
		int get_sd();
		QString get_user_agent();
		void change_track(const MetaData& md);
		bool send_header(bool reject);
		bool send_playlist(int port, const MetaData& md);
		HttpAnswer parse_message();
		bool send_data(const uchar*, quint64 size);
		void disconnect();
		void enable();
		void disable();

};
