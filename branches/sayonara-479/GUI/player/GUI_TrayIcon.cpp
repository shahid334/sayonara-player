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



GUI_TrayIcon::GUI_TrayIcon (QObject *parent) : QSystemTrayIcon (parent) {

	m_settings = CSettingsStorage::getInstance();

	QString icon_path = Helper::getIconPath();
	QIcon play_icon = QIcon(icon_path + "/play.png");
	QIcon pause_icon = QIcon(icon_path + "/pause.png");

    QPixmap play_pixmap = play_icon.pixmap(24, 24);
    QPixmap pause_pixmap = pause_icon.pixmap(24, 24);

    m_playIcon = QIcon(play_pixmap);
    m_pauseIcon = QIcon(pause_pixmap);

    m_vol_step = 5;

    m_plugin_loader = NotificationPluginLoader::getInstance();
    m_notification_active = m_settings->getShowNotification();
    m_timeout = m_settings->getNotificationTimeout();


    m_playAction = new QAction(tr("Play"), this);
	m_playAction->setIcon(QIcon(icon_path + "play.png"));
	m_stopAction = new QAction(tr("Stop"), this);
	m_stopAction->setIcon(QIcon(icon_path + "stop.png"));
	m_bwdAction = new QAction(tr("Previous"), this);
	m_bwdAction->setIcon(QIcon(icon_path + "bwd.png"));
	m_fwdAction = new QAction(tr("Next"), this);
	m_fwdAction->setIcon(QIcon(icon_path + "fwd.png"));
	m_muteAction = new QAction(tr("Mute"), this);
	m_muteAction->setIcon(QIcon(icon_path + "vol_mute.png"));
	m_closeAction = new QAction(tr("Close"), this);
	m_closeAction->setIcon(QIcon(icon_path + "close.png"));
	m_showAction = new QAction(tr("Show"), this);

	QMenu* trayContextMenu = new QMenu();
	    trayContextMenu->addAction(m_playAction);
	    trayContextMenu->addAction(m_stopAction);
	    trayContextMenu->addSeparator();
	    trayContextMenu->addAction(m_fwdAction);
	    trayContextMenu->addAction(m_bwdAction);
	    trayContextMenu->addSeparator();
	    trayContextMenu->addAction(m_muteAction);
	    trayContextMenu->addSeparator();
	    trayContextMenu->addAction(m_showAction);
	    trayContextMenu->addAction(m_closeAction);
	this->setContextMenu(trayContextMenu);


    this->setToolTip("Sayonara Player");
    this->setIcon(m_playIcon);

    connect(m_playAction, SIGNAL(triggered()), this, SLOT(play_clicked()));
    connect(m_fwdAction, SIGNAL(triggered()), this, SLOT(fwd_clicked()));
    connect(m_bwdAction, SIGNAL(triggered()), this, SLOT(bwd_clicked()));
    connect(m_stopAction, SIGNAL(triggered()), this, SLOT(stop_clicked()));
    connect(m_showAction, SIGNAL(triggered()), this, SLOT(show_clicked()));
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(close_clicked()));
    connect(m_muteAction, SIGNAL(triggered()), this, SLOT(mute_clicked()));
}


GUI_TrayIcon::~GUI_TrayIcon() {
	delete m_playAction;
	delete m_stopAction;
	delete m_bwdAction;
	delete m_fwdAction;
	delete m_muteAction;
	delete m_closeAction;
}



bool GUI_TrayIcon::event ( QEvent * e ) {
    if (e->type() == QEvent::Wheel) {
        QWheelEvent * wheelEvent = dynamic_cast <QWheelEvent *> (e);
        emit onVolumeChangedByWheel (wheelEvent->delta());
    }
    return true;
}





void GUI_TrayIcon::songChangedMessage (const MetaData& md) {

    if(m_notification_active){
        Notification* n = m_plugin_loader->get_cur_plugin();

        if(n){
            qDebug() << "Notification: " << md.title << ", " << md.artist+ "," + md.album;
            QString text = md.artist+ "\n" + md.album;
            text.replace("&", "&amp;");
            n->notification_show(md.title, text);
        }

        else if (this -> isSystemTrayAvailable()) {

            this -> showMessage("Sayonara", md.title + " by " + md.artist,QSystemTrayIcon::Information, m_timeout);
        }
    }

    this -> setToolTip(md.title + " by " + md.artist);
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

void GUI_TrayIcon::set_enable_play(bool b){
	m_playAction->setEnabled(b);
}

void GUI_TrayIcon::set_enable_stop(bool b){
	m_stopAction->setEnabled(b);
}

void GUI_TrayIcon::set_enable_mute(bool b){

}

void GUI_TrayIcon::set_enable_fwd(bool b){
	m_fwdAction->setEnabled(b);
}

void GUI_TrayIcon::set_enable_bwd(bool b){
	m_bwdAction->setEnabled(b);
}

void GUI_TrayIcon::set_enable_show(bool b){
	m_showAction->setEnabled(b);
}


void GUI_TrayIcon::play_clicked(){
	if(!m_playAction->text().compare("play", Qt::CaseInsensitive)){
		emit sig_play_clicked();
	}

	else emit sig_pause_clicked();
}

void GUI_TrayIcon::stop_clicked(){
	emit sig_stop_clicked();
}

void GUI_TrayIcon::fwd_clicked(){
	emit sig_fwd_clicked();
}

void GUI_TrayIcon::bwd_clicked(){
	emit sig_bwd_clicked();
}

void GUI_TrayIcon::show_clicked(){
	emit sig_show_clicked();
}

void GUI_TrayIcon::close_clicked(){
	emit sig_close_clicked();
}

void GUI_TrayIcon::mute_clicked(){
	emit sig_mute_clicked();
}

void GUI_TrayIcon::switch_mute_unmute(bool mute){

	if(mute){
		m_muteAction->setIcon(QIcon(Helper::getIconPath() + "vol_mute.png"));
		m_muteAction->setText("Mute");
	}

	else {
		m_muteAction->setIcon(QIcon(Helper::getIconPath() + "vol_3.png"));
		m_muteAction->setText("Mute");
	}
}

void GUI_TrayIcon::switch_play_pause(bool play){

	if(play){
		setIcon(m_playIcon);
		m_playAction->setIcon(m_playIcon);
		m_playAction->setText("Play");
	}
	else {
		setIcon(m_pauseIcon);
		m_playAction->setIcon(m_pauseIcon);
		m_playAction->setText("Pause");
	}
}

int GUI_TrayIcon::get_vol_step(){
	return m_vol_step;

}
