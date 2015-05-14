#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include "HelperStructs/globals.h"
#include "HelperStructs/SayonaraClass.h"
#include "PlayManager.h"

#include <QDBusInterface>
#include <QDBusConnection>
#include <QProcess>
#include <QTimer>
#include <QDebug>


class Shutdown : public QObject, private SayonaraClass {

	Q_OBJECT
	SINGLETON_QOBJECT(Shutdown)

signals:

	void sig_time_to_go(quint64);
	void sig_started(quint64);


private:
	QTimer*		_timer;
	QTimer*		_timer_countdown;
	PlayManager* _play_manager;

	quint64		_msecs2go;
	bool		_is_running;


private slots:
	void timeout();
	void countdown_timeout();
	void playlist_finished();

public:
	bool is_running();

	void stop();
	void shutdown(quint64 ms=0);
	void shutdown_after_end();
};



#endif // SHUTDOWN_H
