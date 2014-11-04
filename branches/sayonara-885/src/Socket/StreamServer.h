#include <QThread>
#include <QList>

#include "Sockets/SocketWriter.h"

typedef struct sockaddr_in SocketAdress;
typedef struct sockaddr SocketAdress_t;

class StreamServer : public QThread{

	Q_OBJECT

	public:
		StreamServer(QObject* parent=0);
		~StreamServer();

	private:
		int _socket;
		int _port;
		bool _reject;

		bool create_socket();
		QList<SocketWriter*> _lst_sw;

		bool accept();



	protected:
		void run();

	public slots:
		void new_data(uchar* data, quint64 size);
		void disconnect(const QString& ip);
		void disconnect_all();
		void close();
};
