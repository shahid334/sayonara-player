/* GUI_TrayIcon.cpp */

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
#include "Notification/NotificationPluginLoader.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "GUI/player/GUI_TrayIcon.h"
#include <QAction>
#include <QMenu>
#include <QEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <Notification/Notification.h>



GUI_TrayIcon::GUI_TrayIcon (const QIcon & playIcon, const QIcon & pauseIcon, QObject *parent) : QSystemTrayIcon (parent) {

    QPixmap play_pixmap = playIcon.pixmap(24, 24);
    m_playIcon = QIcon(play_pixmap);

    QPixmap pause_pixmap = pauseIcon.pixmap(24, 24);
    m_pauseIcon = QIcon(pause_pixmap);

    m_settings = CSettingsStorage::getInstance();

    m_plugin_loader = NotificationPluginLoader::getInstance();
    m_notification_active = m_settings->getShowNotification();
    m_timeout = m_settings->getNotificationTimeout();

    this -> setToolTip("Sayonara - Music - Player");
    this -> setIcon(playIcon);
}


GUI_TrayIcon::~GUI_TrayIcon() {

}


void GUI_TrayIcon::setupMenu (    QAction* closeAction,
                                  QAction* playAction,
                                  QAction* stopAction,
                                  QAction* muteAction,
                                  QAction* fwdAction,
                                  QAction* bwdAction,
                                  QAction* showAction) {
    QMenu* trayContextMenu = new QMenu();
    trayContextMenu->addAction(playAction);
    trayContextMenu->addAction(stopAction);
    trayContextMenu->addSeparator();
    trayContextMenu->addAction(fwdAction);
    trayContextMenu->addAction(bwdAction);
    trayContextMenu->addSeparator();
    trayContextMenu->addAction(muteAction);
    trayContextMenu->addSeparator();
    trayContextMenu->addAction(showAction);
    trayContextMenu->addAction(closeAction);
    this ->  setContextMenu(trayContextMenu);
}

bool GUI_TrayIcon::event ( QEvent * e ) {
    if (e->type ()== QEvent::Wheel) {
        QWheelEvent * wheelEvent = dynamic_cast <QWheelEvent *> (e);
        emit onVolumeChangedByWheel (wheelEvent->delta());
    }
    return true;
}

void GUI_TrayIcon::songChangedMessage (const MetaData& md) {

    if(m_notification_active){
        Notification* n = m_plugin_loader->get_cur_plugin();

        if(n){
            n->notification_show(md.title, md.artist+ "\n" + md.album);
        }

        else if (this -> isSystemTrayAvailable()) {

            this -> showMessage("Sayonara",md.title + " by " + md.artist,QSystemTrayIcon::Information, m_timeout);
        }
    }

    this -> setToolTip(md.title + " by " + md.artist);
}

void GUI_TrayIcon::playStateChanged (bool playing) {
    if(playing) {
        this -> setIcon(this->m_playIcon);
    }
    else {
        this -> setIcon(this -> m_pauseIcon);
    }
}

void GUI_TrayIcon::trackChanged(const MetaData& md){
	this->setToolTip(md.title + " by " + md.artist);
    songChangedMessage(md);
}

void  GUI_TrayIcon::set_timeout(int timeout_ms){
    m_timeout = timeout_ms;
}

void  GUI_TrayIcon::set_notification_active(bool active){
    m_notification_active = active;
}

