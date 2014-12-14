#include <QThread>
#include <QList>
#include <QTcpSocket>
#include <QTcpServer>
#include "RadioStation/StreamWriter.h"
#include "HelperStructs/SayonaraClass.h"


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

	public:
		StreamServer(QObject* parent=0);
		~StreamServer();

	private:

		MetaData _md;

		bool _prompt;
		bool _active;
		int _port;
		int _n_clients;

		QTcpServer* _server;
		QList<QTcpSocket*> _sockets;

		QMap<QString, QTcpSocket*> _queue_map;

		QList<StreamWriter*> _lst_sw;
		QList<QTcpSocket*> _queue;

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
		void update_track(const MetaData&);
		void server_close();
		void stop();

	private slots:
		void active_changed();
		void port_changed();
		void prompt_changed();
		void new_client_request();

};
