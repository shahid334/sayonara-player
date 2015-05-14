#include "HelperStructs/Shutdown/Shutdown.h"
#include "Notification/NotificationPluginLoader.h"


Shutdown::Shutdown(QObject* parent) :
	QObject(parent)
{
	_play_manager = PlayManager::getInstance();
	_is_running = false;

	_timer = new QTimer(this);
	_timer_countdown = new QTimer(this);

	_timer->setInterval(100);
	_timer_countdown->setInterval(50);

	_msecs2go = 0;

	connect(_timer, SIGNAL(timeout()), this, SLOT(timeout()));
	connect(_timer_countdown, SIGNAL(timeout()), this, SLOT(countdown_timeout()));
	connect(_play_manager, SIGNAL(sig_playlist_finished()), this, SLOT(playlist_finished()));
}


Shutdown::~Shutdown(){
	_timer->stop();
	_timer->deleteLater();
	_timer_countdown->stop();
	_timer_countdown->deleteLater();
}


void Shutdown::shutdown_after_end(){
	_is_running = true;
	NotificationPluginLoader* loader = NotificationPluginLoader::getInstance();
	Notification* n = loader->get_cur_plugin();
	if(n){
		n->notification_show(tr("Computer will shutdown after playlist has finished"));
	}
}

void Shutdown::shutdown(quint64 ms){

	if(ms == 0){
		timeout();
		return;
	}

	_is_running = true;
	_msecs2go = ms;
	_timer->start((int) ms);
	_timer_countdown->start(1000);
	emit sig_started(ms);

	NotificationPluginLoader* loader = NotificationPluginLoader::getInstance();
	Notification* n = loader->get_cur_plugin();
	if(n){
		n->notification_show(tr("Computer will shutdown in %1 minutes").arg(Helper::cvt_ms_to_string(ms, false, true, false)));
	}
}


bool Shutdown::is_running(){
	return _is_running;
}


void Shutdown::stop(){
	qDebug() << "Shutdown cancelled";
	_is_running = false;
	_timer->stop();
	_timer_countdown->stop();
	_msecs2go = 0;

}


void Shutdown::countdown_timeout(){

	_msecs2go -= 1000;
	_timer_countdown->start(1000);

	emit sig_time_to_go(_msecs2go);
	qDebug() << "Time to go: " << _msecs2go;

	if(_msecs2go % 60000 == 0){
		NotificationPluginLoader* loader = NotificationPluginLoader::getInstance();
		Notification* n = loader->get_cur_plugin();
		if(n){
			n->notification_show(tr("Computer will shutdown in %1 minutes").arg(Helper::cvt_ms_to_string(_msecs2go, false, true, false)));
		}
	}
}


void Shutdown::timeout()
{
	_is_running = false;
	CDatabaseConnector::getInstance()->store_settings();


	QDBusMessage response;

	QDBusInterface free_desktop_login(
				"org.freedesktop.login1",
				"/org/freedesktop/login1",
				"org.freedesktop.login1.Manager",
				 QDBusConnection::systemBus()
	);

	QDBusInterface free_desktop_console_kit(
				"org.freedesktop.ConsoleKit",
				"/org/freedesktop/ConsoleKit/Manager",
				"org.freedesktop.ConsoleKit.Manager",
				QDBusConnection::systemBus()
	);

	QDBusInterface gnome_session_manager(
				"org.gnome.SessionManager",
				"/org/gnome/SessionManager",
				"org.gnome.SessionManager",
				QDBusConnection::sessionBus()
	);

	QDBusInterface mate_session_manager(
				"org.mate.SessionManager",
				"/org/mate/SessionManager",
				"org.mate.SessionManager",
				QDBusConnection::sessionBus()
	);


	QDBusInterface kde_session_manager(
				"org.kde.ksmserver",
				"/KSMServer",
				"org.kde.KSMServerInterface",
				QDBusConnection::sessionBus()
	);


	if(QProcess::startDetached("/usr/bin/systemctl poweroff")){
		return;
	}

	bool g_pwr1 = QProcess::startDetached("gnome-power-cmd.sh shutdown");
	bool g_pwr2 = QProcess::startDetached("gnome-power-cmd shutdown");

	if(g_pwr1 || g_pwr2){
		return;
	}

	response = free_desktop_login.call("PowerOff", true);

	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = gnome_session_manager.call("RequestShutdown");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = gnome_session_manager.call("Shutdown");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = kde_session_manager.call("logout", 0, 2, 2);
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = kde_session_manager.call("Shutdown");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = mate_session_manager.call("RequestShutdown");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = mate_session_manager.call("Shutdown");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = free_desktop_console_kit.call("Stop");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	if(QProcess::startDetached("sudo shutdown -P now")){
		return;
	}

	if(QProcess::startDetached("sudo shutdown -h -P now")){
		return;
	}

	qDebug() << "Sorry, power off is not possible";
}


void Shutdown::playlist_finished(){
	if( _is_running ){
		timeout();
	}
}
