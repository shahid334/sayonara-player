/* GUI_TrayIcon.h */

/* Copyright (C) 2012  gleugner
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


#ifndef GUI_TRAYICON_H
#define GUI_TRAYICON_H

#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/MetaData.h"

#include "Notification/Notification.h"
#include "Notification/NotificationPluginLoader.h"

#include <QSystemTrayIcon>
#include <QAction>



/**
  * Small class to be used as tray icon
  */
class GUI_TrayIcon : public QSystemTrayIcon {
    Q_OBJECT
public:
    explicit GUI_TrayIcon(const QIcon & playIcon,const QIcon & pauseIcon, QObject *parent = 0);
    ~GUI_TrayIcon();

    void setupMenu (    QAction* closeAction,
                        QAction* playAction,
                        QAction* stopAction,
                        QAction* muteAction,
                        QAction* fwdAction,
                        QAction* bwdAction,
                        QAction* showAction);

    virtual bool event ( QEvent * e );
    void set_timeout(int timeout_ms);
    void set_notification_active(bool active);

public slots:
    void playStateChanged (bool playing);
    void trackChanged(const MetaData& md);
    void songChangedMessage (const MetaData& md);


signals:

    /**
      * this event is fired, if we have a mouse wheel event
      * @param delta bigger then 0 when mouse wheel has moved forward smaller when moved backwards
      */
    void onVolumeChangedByWheel (int delta);


private:
    QIcon                   m_playIcon;
    QIcon                   m_pauseIcon;
    int                     m_timeout;

    NotificationPluginLoader* m_plugin_loader;

    bool                    m_notification_active;
    CSettingsStorage*       m_settings;
};


#endif
