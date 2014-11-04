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



class SocketWriter {

	public:
		SocketWriter(int socket, QString ip);
		virtual ~SocketWriter();


	private:
		int _socket;
		bool _send_data;
		bool _icy;

		quint64 _sent_bytes;
		QString _ip;

		QByteArray _icy_header;
		QByteArray _header;
		QByteArray _reject_header;
		QString _stream_title;

		void reset();
		void create_headers();
		void send_icy_data();

	public:
		QString get_ip();
		ip get_sd();
		void change_track(const MetaData& md);
		bool send_header(bool reject);		
		bool send_data(const uchar*, quint64 size);
		void disconnect();
		void enable();
		void disable();

};
