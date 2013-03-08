#include <libnotify/notify.h>

#include <QString>
#include <QtPlugin>
#include <QDebug>
#include <QFile>
#include <QPixmap>

#include "Notification/Notification.h"
#include "HelperStructs/CSettingsStorage.h"
#include "Notification/libnotify/LN_Notification.h"
#include "HelperStructs/Helper.h"
#include "Notification/libnotify/LN_Version.h"



LN_Notification::LN_Notification(){
	_initialized = notify_init("Sayonara"); 
    _not = 0;
}

LN_Notification::~LN_Notification(){

}

void LN_Notification::notification_show(const MetaData& md){



    if(!_initialized) return;

    not_close();

    QString text = md.artist + "\n" + md.album;
    text.replace("&", "&amp;");

    QString pixmap_path = Helper::get_cover_path(md.artist, md.album);
    if(!QFile::exists(pixmap_path)) pixmap_path = Helper::getIconPath() + "logo_small.png";
    else{

        QPixmap p(pixmap_path);
        p = p.scaled(35, 35, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        bool success = p.save(Helper::getSayonaraPath() + "not.jpg");
        if(success)
            pixmap_path = Helper::getSayonaraPath() + "not.jpg";
    }


#ifdef LIBNOTIFY_NEW
	NotifyNotification* n = notify_notification_new( md.title.toLocal8Bit().data(),
                                                 text.toLocal8Bit().data(),
                                                pixmap_path.toLocal8Bit().data());
#endif
#ifdef LIBNOTIFY_OLD
	
	NotifyNotification* n = notify_notification_new( md.title.toLocal8Bit().data(),
                                                 text.toLocal8Bit().data(),
                                                pixmap_path.toLocal8Bit().data(), NULL);
#endif

   _not = n;



    int timeout = CSettingsStorage::getInstance()->getNotificationTimeout();
    notify_notification_set_timeout     (n, timeout);
    notify_notification_show            (n, NULL);

}

void LN_Notification::notification_update(const MetaData& md){

   if(!_not) return;

   QString text = md.artist + "\n" + md.album;
   text.replace("&", "&amp;");
   QString pixmap_path = Helper::get_cover_path(md.artist, md.album);

   notify_notification_update( (NotifyNotification*) _not, 
				md.title.toLocal8Bit().data(),
				text.toLocal8Bit().data(),
				pixmap_path.toLocal8Bit().data());

				    

}

void LN_Notification::not_close(){

    if(!_not) return;

    NotifyNotification* n = (NotifyNotification*) _not;
    if(n)
       notify_notification_close(n,NULL);
    _not = NULL;

}


QString LN_Notification::get_name(){
	return "libnotify";
}


Q_EXPORT_PLUGIN2(sayonara_libnotify, LN_Notification);

