/* LN_Notification.cpp */

/* Copyright (C) 2011-2014  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libnotify/notify.h>

#include <QtPlugin>
#include <QFile>
#include <QPixmap>

#include "CoverLookup/CoverLocation.h"
#include "Notification/Notification.h"
#include "Settings/Settings.h"
#include "Notification/libnotify/LN_Notification.h"


LN_Notification::LN_Notification() {
	_initialized = notify_init("Sayonara"); 
    _not = 0;
}

LN_Notification::~LN_Notification() {

}

static void notification_closed(gpointer notification, void* data){

    gpointer* pnot = &notification;
    *pnot = 0;

}

void LN_Notification::notification_show(const MetaData& md) {

	if(!_initialized) {
		return;
	}

	CoverLocation cl = CoverLocation::get_cover_location(md);

    QString pixmap_path;

    not_close();

    QString text = md.artist + "\n" + md.album;
    text.replace("&", "&amp;");

	if( !QFile::exists(cl.cover_path) ) {
        pixmap_path = Helper::getIconPath() + "logo_small.png";
	}

    else{

		bool success;
        QPixmap p(cl.cover_path);

		int scale = _settings->get(Set::Notification_Scale);

		if(scale > 0) {
			p = p.scaled(scale, scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		}

		success = p.save(Helper::getSayonaraPath() + "not.jpg");

		if(success) {
            pixmap_path = Helper::getSayonaraPath() + "not.jpg";
		}
    }


#if (NOTIFY_VERSION_MINOR > 6 && NOTIFY_VERSION_MAJOR >= 0)
	NotifyNotification* n = notify_notification_new( md.title.toLocal8Bit().data(),
                                                 text.toLocal8Bit().data(),
                                                pixmap_path.toLocal8Bit().data());
#else
	 NotifyNotification* n = notify_notification_new( md.title.toLocal8Bit().data(),
                                                 text.toLocal8Bit().data(),
                                                pixmap_path.toLocal8Bit().data(), NULL);
#endif

     g_signal_connect(n, "closed", G_CALLBACK(notification_closed), NULL);


   _not = n;

	int timeout = _settings->get(Set::Notification_Timeout);

    notify_notification_set_timeout     (n, timeout);
    notify_notification_show            (n, NULL);

}

void LN_Notification::notification_update(const MetaData& md) {

   if(!_not) return;

   QString pixmap_path = CoverLocation::get_cover_location(md).cover_path;

   QString text = md.artist + "\n" + md.album;
   text.replace("&", "&amp;");

   notify_notification_update( (NotifyNotification*) _not, 
				md.title.toLocal8Bit().data(),
				text.toLocal8Bit().data(),
				pixmap_path.toLocal8Bit().data());
}

void LN_Notification::not_close() {

    if(!_not) return;

    NotifyNotification* n = (NotifyNotification*) _not;
    if(n){
       notify_notification_close(n,NULL);
    }

    _not = NULL;
}


QString LN_Notification::get_name() {
	return "libnotify";
}


Q_EXPORT_PLUGIN2(sayonara_libnotify, LN_Notification);

