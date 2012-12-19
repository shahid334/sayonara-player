#ifndef NOTIFICATION_PLUGIN_LOADER_H_
#define NOTIFICATION_PLUGIN_LOADER_H_

#include "Notification/Notification.h"
#include <QList>
#include <QObject>
#include <QString>

class NotificationPluginLoader;

class NotificationPluginLoader : public QObject {

    Q_OBJECT

public:
	
    static NotificationPluginLoader* getInstance();
    NotificationPluginLoader(NotificationPluginLoader&);
    QList<Notification*> get_plugins();
    Notification* get_cur_plugin();
    void set_cur_plugin(Notification* n);
    void set_cur_plugin(QString name);

private:

    NotificationPluginLoader();

    QString app_dir;

    QList<Notification*> _notification_plugins;
    Notification*       _cur_notification;
    int                 _cur_idx;

};

#endif

