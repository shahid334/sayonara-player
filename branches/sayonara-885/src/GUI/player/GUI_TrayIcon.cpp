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
#include "HelperStructs/Helper.h"
#include "GUI/player/GUI_TrayIcon.h"
#include "HelperStructs/Style.h"

#include <QAction>
#include <QMenu>
#include <QEvent>
#include <QWheelEvent>
#include <QHoverEvent>
#include <QIcon>
#include <QPixmap>
#include <QTimer>
#include <QFont>


GUI_TrayIcon::GUI_TrayIcon (QObject *parent) :
	QSystemTrayIcon (parent),
	SayonaraClass()
{

    m_playing = false;
    m_mute = false;

    m_vol_step = 5;

    m_plugin_loader = NotificationPluginLoader::getInstance();

    _md_set = false;
    
	m_playAction = new QAction(tr("Play"), this);
	m_playAction->setIcon(Helper::getIcon("play.png"));
	m_stopAction = new QAction(tr("Stop"), this);
	m_stopAction->setIcon(Helper::getIcon("stop.png"));
	m_bwdAction = new QAction(tr("Previous"), this);
	m_bwdAction->setIcon(Helper::getIcon("bwd.png"));
	m_fwdAction = new QAction(tr("Next"), this);
	m_fwdAction->setIcon(Helper::getIcon("fwd.png"));
	m_muteAction = new QAction(tr("Mute"), this);
	m_muteAction->setIcon(Helper::getIcon("vol_mute.png"));
	m_closeAction = new QAction(tr("Close"), this);
	m_closeAction->setIcon(Helper::getIcon("close.png"));
	m_showAction = new QAction(tr("Show"), this);

    m_trayContextMenu = new QMenu();
	m_trayContextMenu->addAction(m_playAction);
	m_trayContextMenu->addAction(m_stopAction);
	m_trayContextMenu->addSeparator();
	m_trayContextMenu->addAction(m_fwdAction);
	m_trayContextMenu->addAction(m_bwdAction);
	m_trayContextMenu->addSeparator();
	m_trayContextMenu->addAction(m_muteAction);
	m_trayContextMenu->addSeparator();
	m_trayContextMenu->addAction(m_showAction);
	m_trayContextMenu->addAction(m_closeAction);
	QFont f = m_trayContextMenu->font();
	f.setFamily("DejaVu Sans");
	m_trayContextMenu->setFont(f);

	this->setContextMenu(m_trayContextMenu);
	this->setIcon(Helper::getIcon("play.png"));

    connect(m_playAction, SIGNAL(triggered()), this, SLOT(play_clicked()));
    connect(m_fwdAction, SIGNAL(triggered()), this, SLOT(fwd_clicked()));
    connect(m_bwdAction, SIGNAL(triggered()), this, SLOT(bwd_clicked()));
    connect(m_stopAction, SIGNAL(triggered()), this, SLOT(stop_clicked()));
    connect(m_showAction, SIGNAL(triggered()), this, SLOT(show_clicked()));
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(close_clicked()));
    connect(m_muteAction, SIGNAL(triggered()), this, SLOT(mute_clicked()));

	REGISTER_LISTENER(Set::Player_Style, skin_changed);
}


GUI_TrayIcon::~GUI_TrayIcon() {
	delete m_playAction;
	delete m_stopAction;
	delete m_bwdAction;
	delete m_fwdAction;
	delete m_muteAction;
	delete m_closeAction;
}


void GUI_TrayIcon::language_changed() {
  
	m_playAction->setText(tr("Play"));
	m_fwdAction->setText(tr("Next"));
	m_bwdAction->setText(tr("Previous"));
	m_stopAction->setText(tr("Stop"));
	m_muteAction->setText(tr("Mute"));
	m_closeAction->setText(tr("Close"));
	m_showAction->setText(tr("Show"));
}
        

void GUI_TrayIcon::skin_changed() {
	bool dark = (_settings->get(Set::Player_Style) == 1);
	QString stylesheet = Style::get_style(dark);
    this->m_trayContextMenu->setStyleSheet(stylesheet);
    this->setMute(_mute);
}

bool GUI_TrayIcon::event ( QEvent * e ) {

    if (e->type() == QEvent::Wheel) {
        QWheelEvent* wheelEvent = (QWheelEvent*) e;
        emit onVolumeChangedByWheel (wheelEvent->delta());
    }

    return true;
}


void GUI_TrayIcon::show_notification (const MetaData& md) {

    _md = md;
    _md_set = true;

	bool active = _settings->get(Set::Notification_Show);
	if(!active) return;

	Notification* n = m_plugin_loader->get_cur_plugin();

	if(n) {
		n->notification_show(md);
	}

	else if ( isSystemTrayAvailable() ) {

		QString msg = md.title + tr(" by ") + md.artist;
		int timeout = _settings->get(Set::Notification_Timeout);

		showMessage("Sayonara", msg, QSystemTrayIcon::Information, timeout);
	}
}



void GUI_TrayIcon::set_enable_play(bool b) {
	m_playAction->setEnabled(b);
}

void GUI_TrayIcon::set_enable_stop(bool b) {
	m_stopAction->setEnabled(b);
}

void GUI_TrayIcon::set_enable_mute(bool b) {

}

void GUI_TrayIcon::set_enable_fwd(bool b) {
	m_fwdAction->setEnabled(b);
}

void GUI_TrayIcon::set_enable_bwd(bool b) {
	m_bwdAction->setEnabled(b);
}

void GUI_TrayIcon::set_enable_show(bool b) {
	m_showAction->setEnabled(b);
}


void GUI_TrayIcon::play_clicked() {

	if( !m_playing ) {
		emit sig_play_clicked();
	}

	else {
		emit sig_pause_clicked();
	}
}

void GUI_TrayIcon::stop_clicked() {
	emit sig_stop_clicked();
}

void GUI_TrayIcon::stop() {
	_md_set = false;
}

void GUI_TrayIcon::fwd_clicked() {
	emit sig_fwd_clicked();
}

void GUI_TrayIcon::bwd_clicked() {
	emit sig_bwd_clicked();
}

void GUI_TrayIcon::show_clicked() {
	emit sig_show_clicked();
}

void GUI_TrayIcon::close_clicked() {
	emit sig_close_clicked();
}

void GUI_TrayIcon::mute_clicked() {
	emit sig_mute_clicked();
}

void GUI_TrayIcon::setMute(bool mute) {

    _mute = mute;

    QString suffix = "";
	int style = _settings->get(Set::Player_Style);

    if(style == 1) {
        suffix = "_dark";
    }

	if(!mute) {
        m_muteAction->setIcon(Helper::getIcon("vol_mute" + suffix + ".png"));
        m_muteAction->setText(tr("Mute"));
	}

	else {
        m_muteAction->setIcon(Helper::getIcon("vol_3" + suffix + ".png"));
        m_muteAction->setText(tr("Unmute"));
	}
}

void GUI_TrayIcon::setPlaying(bool play) {

    m_playing = play;

	if(play) {
		setIcon(Helper::getIcon("play.png"));
		m_playAction->setIcon(Helper::getIcon("pause.png"));
		m_playAction->setText(tr("Pause"));
	}

	else {
		setIcon(Helper::getIcon("pause.png"));
		m_playAction->setIcon(Helper::getIcon("play.png"));
		m_playAction->setText(tr("Play"));
	}
}

int GUI_TrayIcon::get_vol_step() {
	return m_vol_step;
}
