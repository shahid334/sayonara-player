#include <QThread>
#include <QList>

#include "RadioStation/SocketWriter.h"
#include "Settings/Settings.h"

typedef struct sockaddr_in SocketAdress;
typedef struct sockaddr SocketAdress_t;

enum AcceptState{
	AcceptStateNone=-1,
	AcceptStateWaiting=0,
	AcceptStateRejected=1,
	AcceptStateAccepted=2,
	AcceptStateTimeout=3
};

class StreamServer : public QThread{

	Q_OBJECT

	signals:
		void sig_new_connection_request(const QString&);
		void sig_new_connection(const QString&);
		void sig_connection_closed(const QString&);

	public:
		StreamServer(QObject* parent=0);
		~StreamServer();

	private:
		Settings* settings;
		int _socket;
		int _port;

		QMap<QString, AcceptState> _accept_map;
		AcceptState _accepted;

		bool create_socket();
		QList<SocketWriter*> _lst_sw;

		SocketWriter* client_accept();
		MetaData _md;

	protected:
		void run();

	public slots:
		void new_data(uchar* data, quint64 size);
		void accept_client();
		void reject_client();
		void dismiss(int idx);
		void disconnect(SocketWriter* sw);
		void disconnect_all();
		void update_track(const MetaData&);
		void server_close();
		void stop();
};
