#include <QThread>
#include <QList>

#include "RadioStation/SocketWriter.h"

typedef struct sockaddr_in SocketAdress;
typedef struct sockaddr SocketAdress_t;

class StreamServer : public QThread{

	Q_OBJECT

	signals:
		void sig_new_connection(const QString&);
		void sig_connection_closed(const QString&);

	public:
		StreamServer(QObject* parent=0);
		~StreamServer();

	private:
		int _socket;
		int _port;
		bool _reject;
		bool _waiting;

		bool create_socket();
		QList<SocketWriter*> _lst_sw;

		SocketWriter* client_accept();
		MetaData _md;



	protected:
		void run();

	public slots:
		void new_data(uchar* data, quint64 size);
		void dismiss(int idx);
		void disconnect(SocketWriter* sw);
		void disconnect_all();
		void update_track(const MetaData&);
		void server_close();
		void stop();
};
