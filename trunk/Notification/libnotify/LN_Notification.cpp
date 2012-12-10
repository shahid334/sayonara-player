#include <libnotify/notify.h>

#include <QString>
#include <QtPlugin>

#include "Notification/Notification.h"
#include "HelperStructs/CSettingsStorage.h"
#include "Notification/libnotify/LN_Notification.h"
#include "HelperStructs/Helper.h"


LN_Notification::LN_Notification(){
	_initialized = notify_init("Sayonara"); 
}

LN_Notification::~LN_Notification(){

}

void LN_Notification::notification_show(QString title, QString text){

	if(!_initialized) return;

    QString pixmap_path = Helper::getIconPath() + "/logo_small.png";

    NotifyNotification* n = notify_notification_new( title.toLocal8Bit().data(),
                                                     text.toLocal8Bit().data(),
                                                    pixmap_path.toLocal8Bit().data());
    int timeout = CSettingsStorage::getInstance()->getNotificationTimeout();
    notify_notification_set_timeout     (n, timeout);
	notify_notification_show            (n, NULL);
}

QString LN_Notification::get_name(){
	return "libnotify";
}


Q_EXPORT_PLUGIN2(sayonara_libnotify, LN_Notification);

