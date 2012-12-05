#include "Notification/Notification.h"
#include <QList>

class NotificationPluginLoader;

class NotificationPluginLoader{
	
public:
	
	NotificationPluginLoader* getInstance();

	NotificationPluginLoader(NotificationPluginLoader&);

	virtual ~NotificationPluginLoader();
	
	void get_plugins();

private:

	NotificationPluginLoader();
	void search_plugins();

	QList<Notification> _notification_plugins;

};
