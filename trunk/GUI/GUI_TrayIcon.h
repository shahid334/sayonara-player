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

#include <QSystemTrayIcon>
#include <QAction>

/**
  * Small class to be used as tray icon
  */
class GUI_TrayIcon : public QSystemTrayIcon {
    Q_OBJECT
public:
    explicit GUI_TrayIcon(const QIcon & playIcon, const QIcon & pauseIcon, QObject *parent = 0);
    ~GUI_TrayIcon();

    void setupMenu (    QAction* closeAction,
                        QAction* playAction,
                        QAction* stopAction,
                        QAction* muteAction,
                        QAction* fwdAction,
                        QAction* bwdAction);

public slots:
    void playStateChanged (bool playing);

signals:


private:
    QIcon                   m_playIcon;
    QIcon                   m_pauseIcon;


};


#endif
