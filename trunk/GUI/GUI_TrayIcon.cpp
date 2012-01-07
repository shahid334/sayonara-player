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

#include <GUI_TrayIcon.h>
#include <QAction>
#include <QMenu>
#include <Helper.h>

GUI_TrayIcon::GUI_TrayIcon (const QIcon & playIcon, const QIcon & pauseIcon, QObject *parent) : QSystemTrayIcon (parent) {
    this -> setToolTip("Sayonara - Music - Player");
    this -> setIcon(playIcon);
    this -> m_playIcon = playIcon;
    this -> m_pauseIcon = pauseIcon;
}

void GUI_TrayIcon::setupMenu (    QAction* closeAction,
                                  QAction* playAction,
                                  QAction* stopAction,
                                  QAction* muteAction,
                                  QAction* fwdAction,
                                  QAction* bwdAction) {
    QMenu* trayContextMenu = new QMenu();
    trayContextMenu->addAction(playAction);
    trayContextMenu->addAction(stopAction);
    trayContextMenu->addSeparator();
    trayContextMenu->addAction(fwdAction);
    trayContextMenu->addAction(bwdAction);
    trayContextMenu->addSeparator();
    trayContextMenu->addAction(muteAction);
    trayContextMenu->addSeparator();
    trayContextMenu->addAction(closeAction);
    this ->  setContextMenu(trayContextMenu);
}

void GUI_TrayIcon::playStateChanged (bool playing) {
    if(playing) {
        this -> setIcon(this->m_playIcon);
    }
    else {
        this -> setIcon(this -> m_pauseIcon);
    }
}

GUI_TrayIcon::~GUI_TrayIcon() {

}
