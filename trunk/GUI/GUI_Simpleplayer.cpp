/* GUI_Simpleplayer.cpp */

/* Copyright (C) 2011  Lucio Carreras
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


#include "GUI/GUI_Simpleplayer.h"
#include "ui_GUI_Simpleplayer.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"


#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <GUI_TrayIcon.h>


GUI_SimplePlayer::GUI_SimplePlayer(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::GUI_SimplePlayer), VOLUME_STEP_SIZE_PERC (2){
	ui->setupUi(this);
	initGUI();

	CSettingsStorage* settings = CSettingsStorage::getInstance();

	this->ui->albumCover->setPixmap(
			QPixmap::fromImage(QImage(Helper::getIconPath() + "append.png")));

	this->m_playing = false;
	this->m_cur_searching = false;
	this->m_mute = false;
	this->m_radio_active = false;

	this -> setupActions();


	m_trayIcon = new GUI_TrayIcon(QIcon(Helper::getIconPath() + "play.png"), QIcon(Helper::getIconPath() + "pause.png"),this);
	m_trayIcon ->setupMenu(m_closeAction,m_playAction, m_stopAction,m_muteAction,m_fwdAction,m_bwdAction);
	m_trayIcon->show();


	m_minTriggerByTray = false;
	m_minimized2tray = false;

	m_isEqHidden = true;
	m_isPcHidden = true;

	QSize size = settings->getPlayerSize();
	QRect rect = this->geometry();
	rect.setWidth(size.width());
	rect.setHeight(size.height());
	this->setGeometry(rect);


	bool loadPlaylistChecked = settings->getLoadPlaylist();
	this->ui->action_load_playlist->setChecked(loadPlaylistChecked);

	bool loadShowNotifications = settings->getShowNotification();
	this->ui->action_notification->setChecked(loadShowNotifications);

	this->ui->action_ViewEqualizer->setText("Equalizer\t\tSTRG+e");
	this->ui->action_ViewPlaylistChooser->setText("Playlist Chooser\tSTRG+p");
	this->ui->action_ViewRadio->setText("Radio\t\tSTRG+r");



	connect(this->ui->btn_play, SIGNAL(clicked(bool)), this,
			SLOT(playClicked(bool)));
	connect(this->ui->btn_fw, SIGNAL(clicked(bool)), this,
			SLOT(forwardClicked(bool)));
	connect(this->ui->btn_bw, SIGNAL(clicked(bool)), this,
			SLOT(backwardClicked(bool)));
	connect(this->ui->btn_stop, SIGNAL(clicked(bool)), this,
			SLOT(stopClicked(bool)));
	connect(this->ui->btn_mute, SIGNAL(released()), this,
				SLOT(muteButtonPressed()));

	// file
	connect(this->ui->action_OpenFile, SIGNAL(triggered(bool)), this,
			SLOT(fileSelectedClicked(bool)));
	connect(this->ui->action_OpenFolder, SIGNAL(triggered(bool)), this,
			SLOT(folderSelectedClicked(bool)));
	connect(this->ui->action_ImportFolder, SIGNAL(triggered(bool)), this,
				SLOT(importFolderClicked()));
	connect(this->ui->action_reloadLibrary, SIGNAL(triggered(bool)), this,
				SLOT(reloadLibraryClicked(bool)));


	// view
	connect(this->ui->action_ViewEqualizer, SIGNAL(toggled(bool)), this,
				SLOT(showEqualizer(bool)));
	connect(this->ui->action_ViewRadio, SIGNAL(toggled(bool)), this,
				SLOT(showRadio(bool)));
	connect(this->ui->action_ViewPlaylistChooser, SIGNAL(toggled(bool)), this,
				SLOT(showPlaylistChooser(bool)));
	connect(this->ui->action_Dark, SIGNAL(toggled(bool)), this,
			SLOT(changeSkin(bool)));
	connect(this->ui->action_notification, SIGNAL(toggled(bool)), this,
			SLOT(show_notification_toggled(bool)));

	// preferences
	connect(this->ui->action_lastFM, SIGNAL(triggered(bool)), this,
			SLOT(lastFMClicked(bool)));
	connect(this->ui->action_setLibPath, SIGNAL(triggered(bool)), this,
			SLOT(setLibraryPathClicked(bool)));
	connect(this->ui->action_fetch_all_covers, SIGNAL(triggered(bool)), this,
			SLOT(fetch_all_covers_clicked(bool)));
	connect(this->ui->action_load_playlist, SIGNAL(toggled(bool)), this,
			SLOT(load_pl_on_startup_toggled(bool)));

	// about
	connect(this->ui->action_about, SIGNAL(triggered(bool)), this, SLOT(about(bool)));


    connect(this->ui->volumeSlider, SIGNAL(valueChanged(int)), this,
			SLOT(volumeChangedSlider(int)));
	    connect(this->ui->songProgress, SIGNAL(sliderPressed()), this,
			SLOT(searchSliderPressed()));
	connect(this->ui->songProgress, SIGNAL(sliderMoved(int)), this,
			SLOT(searchSliderMoved(int)));
	connect(this->ui->songProgress, SIGNAL(sliderReleased()), this,
			SLOT(searchSliderReleased()));





        this -> connectTrayIcon ();



	ui_playlist = 0;
	ui_playlist_chooser = 0;
	ui_radio = 0;
	ui_eq = 0;

	m_skinSuffix = "";
}

GUI_SimplePlayer::~GUI_SimplePlayer() {
	delete ui;
}



void GUI_SimplePlayer::initGUI() {

	this->ui->btn_mute->setIcon(QIcon(Helper::getIconPath() + "vol_1.png"));
	this->ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
	this->ui->btn_stop->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
	this->ui->btn_fw->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
	this->ui->btn_bw->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));

}


void GUI_SimplePlayer::setVolume(int vol) {
	this->ui->volumeSlider->setValue(vol);
	setupVolButton(vol);
	emit volumeChanged((qreal) vol);
}

void GUI_SimplePlayer::setStyle(int style){
	bool dark = (style == 1);
	changeSkin(dark);
	this->ui->action_Dark->setChecked(dark);
}

void GUI_SimplePlayer::changeSkin(bool dark) {

	if (dark) {

		this->ui->centralwidget->setStyleSheet(
				"background-color: " + Style::get_player_back_color() + "; color: #D8D8D8;");
		this->setStyleSheet(
				"background-color: " + Style::get_player_back_color() + "; color: #D8D8D8;");

		this->ui->menuView->setStyleSheet("background-color: " + Style::get_player_back_color() + "; #D8D8D8;");

		QString style = Style::get_btn_style(8);
		this->ui->btn_mute->setStyleSheet(style);
		this->ui->btn_play->setStyleSheet(style);
		this->ui->btn_fw->setStyleSheet(style);
		this->ui->btn_bw->setStyleSheet(style);
		this->ui->btn_stop->setStyleSheet(style);


		m_skinSuffix = QString("_dark");
	}

	else {
		this->ui->centralwidget->setStyleSheet("");
		this->ui->playlist_widget->setStyleSheet("");
		this->setStyleSheet("");
		this->ui->menuView->setStyleSheet("");
		m_skinSuffix = QString("");

		this->ui->btn_mute->setStyleSheet("");
		this->ui->btn_play->setStyleSheet("");
		this->ui->btn_fw->setStyleSheet("");
		this->ui->btn_bw->setStyleSheet("");
		this->ui->btn_stop->setStyleSheet("");

	}

	CSettingsStorage::getInstance()->setPlayerStyle(dark ? 1 : 0);

	setupVolButton(this->ui->volumeSlider->value());
	//emit skinChanged(dark);
	this->ui_library->change_skin(dark);
	this->ui_playlist->change_skin(dark);

}

QString GUI_SimplePlayer::getLengthString(quint32 length_ms) const {
	QString lengthString;
	int length_sec = length_ms / 1000;
	QString min = QString::number(length_sec / 60);
	QString sek = QString::number(length_sec % 60);

	if (min.length() < 2)
		min = QString('0') + min;

	if (sek.length() < 2)
		sek = QString('0') + sek;

	lengthString = min + QString(":") + sek;
	return lengthString;
}

void GUI_SimplePlayer::update_info(const MetaData& in) {


	this->m_metadata = in;
	qDebug() << m_metadata.toStringList();

	if (in.year < 1000 || in.album.contains(QString::number(in.year)))
		this->ui->album->setText(in.album);

	else
		this->ui->album->setText(
				in.album + " (" + QString::number(in.year) + ")");

	this->ui->artist->setText(in.artist);
	this->ui->title->setText(in.title);

	if( this->ui->action_notification->isChecked() ){
		m_trayIcon->songChangedMessage(QString ("Currently playing: \"" + in.title + "\" by " + in.artist));
	}


	this->setWindowTitle(QString("Sayonara - ") + in.title);

	emit wantCover(in);

}

void GUI_SimplePlayer::fillSimplePlayer(const MetaData & md) {

	this->m_metadata = md;

	// sometimes ignore the date
	if (md.year < 1000 || md.album.contains(QString::number(md.year)))
		this->ui->album->setText(md.album);

	else
		this->ui->album->setText(
				md.album + " (" + QString::number(md.year) + ")");

	this->ui->artist->setText(md.artist);
	this->ui->title->setText(md.title);

	if( this->ui->action_notification->isChecked() ){
        m_trayIcon->songChangedMessage(QString(
                        "Currently playing: \"" + md.title + "\" by " + md.artist));
	}

	QString lengthString = getLengthString(md.length_ms);

	this->ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
	this->m_playAction->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
	this->m_playAction->setText("Pause");

	this->ui->maxTime->setText(lengthString);

	// int tmpRating = (rand() % 4) + 1;

	QString tmp = QString("<font color=\"#FFAA00\" size=\"+10\">");
	if (md.bitrate < 96000)
		tmp += "*";
	else if (md.bitrate < 128000)
		tmp += "**";
	else if (md.bitrate < 160000)
		tmp += "***";
	else if (md.bitrate < 256000)
		tmp += "****";
	else
		tmp += "*****";
	tmp += "</font>";

	this->ui->rating->setText(tmp);
	this->ui->rating->setToolTip(
			QString("<font color=\"#000000\">") + QString::number(md.bitrate)
	+ "</font>");

	this->setWindowTitle(QString("Sayonara - ") + md.title);

	this->m_completeLength_ms = md.length_ms;
	this->m_playing = true;

}

void GUI_SimplePlayer::total_time_changed(qint64 total_time) {
	m_completeLength_ms = total_time;
	this->ui->maxTime->setText(getLengthString(total_time));

}

void GUI_SimplePlayer::setCurrentPosition(quint32 pos_sec) {

	if (m_completeLength_ms != 0) {

		double newSliderVal = (double) (pos_sec * 1000.0 * 100.0
				/ m_completeLength_ms);

		if (!m_cur_searching)
			this->ui->songProgress->setValue((int) newSliderVal);

		int min, sec;

		Helper::cvtSecs2MinAndSecs(pos_sec, &min, &sec);

		QString curPosString = Helper::cvtSomething2QString(min, 2)
		+ QString(':') + Helper::cvtSomething2QString(sec, 2);

		this->ui->curTime->setText(curPosString);

	}
}


void GUI_SimplePlayer::playClicked(bool) {

	if (this->m_playing == true) {
		this->ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
		m_playAction->setIcon(QIcon(Helper::getIconPath() + "play.png"));
		m_playAction->setText("Play");
		qDebug() << "Player: pause";
		emit pause();

		this->ui->albumCover->setFocus();

	}

	else {
		this->ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
		m_playAction->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
		m_playAction->setText("Pause");
		qDebug() << "Player: play";
		emit play();

		this->ui->albumCover->setFocus();

	}
	this->m_playing = !this->m_playing;
        this -> m_trayIcon->playStateChanged (this->m_playing);

}

void GUI_SimplePlayer::stopClicked(bool) {

	this->ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));

	m_playAction->setIcon(QIcon(Helper::getIconPath() + "play.png"));
	m_playAction->setText("Play");
	m_playing = false;

	this->ui->title->setText("Sayonara Player");
	this->ui->rating->setText("");
	this->ui->album->setText("Written by Lucio Carreras");
	this->ui->artist->setText("");
	this->setWindowTitle("Sayonara");
	this->ui->songProgress->setValue(0);
	this->ui->curTime->setText("00:00");
	this->ui->maxTime->setText("00:00");
	this->ui->albumCover->setPixmap(
			QPixmap::fromImage(QImage(Helper::getIconPath() + "append.png")));

	this->ui->albumCover->setFocus();
	emit stop();
}

void GUI_SimplePlayer::backwardClicked(bool) {
	this->ui->albumCover->setFocus();
	emit backward();
}

void GUI_SimplePlayer::forwardClicked(bool) {
	this->ui->albumCover->setFocus();
	emit forward();
}

void GUI_SimplePlayer::folderSelectedClicked(bool) {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			getenv("$HOME"),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir != "")
		emit baseDirSelected(dir);
}

void GUI_SimplePlayer::fileSelectedClicked(bool) {
	QStringList list =
			QFileDialog::getOpenFileNames(
					this,
					tr("Open Media files"),
					QDir::homePath(),
					tr(
							"Media files (*.mp3 *.wav *.flac *.aac *.wma *.avi *.mpg *.mpeg)"));

	if (list.size() > 0)
		emit fileSelected(list);
}

void GUI_SimplePlayer::searchSliderPressed() {
	m_cur_searching = true;
}

void GUI_SimplePlayer::searchSliderReleased() {
	m_cur_searching = false;
}

void GUI_SimplePlayer::searchSliderMoved(int search_percent, bool by_app) {
	m_cur_searching = true;

	if (!by_app)
		emit search(search_percent);
}

void GUI_SimplePlayer::volumeChangedSlider(int volume_percent) {
	setupVolButton(volume_percent);
        emit volumeChanged((double)volume_percent);
	CSettingsStorage::getInstance()->setVolume(volume_percent);
}

void GUI_SimplePlayer::volumeChangedByTick(int val) {
    int currentVolumeOrig_perc = this -> ui->volumeSlider->value();
    int currentVolume_perc = currentVolumeOrig_perc;
    if (val > 0) {
        //increase volume
        if (currentVolume_perc < this -> ui->volumeSlider->maximum()-VOLUME_STEP_SIZE_PERC) {
            currentVolume_perc+=VOLUME_STEP_SIZE_PERC;
        }
    }
    else if (val < 0) {
        //decrease volume
        if (currentVolume_perc > this -> ui->volumeSlider->minimum()+VOLUME_STEP_SIZE_PERC) {
            currentVolume_perc-=VOLUME_STEP_SIZE_PERC;
        }
    }
    else {
    }
    if (currentVolumeOrig_perc != currentVolume_perc) {
        this -> ui->volumeSlider->setValue(currentVolume_perc);
    }
}

void GUI_SimplePlayer::setupVolButton(int percent) {

	QString butFilename = Helper::getIconPath() + "vol_";

	if (percent == 0) {
		butFilename += QString("mute") + m_skinSuffix + ".png";
	}

	else if (percent < 40) {
		butFilename += QString("1") + m_skinSuffix + ".png";
	}

	else if (percent < 80) {
		butFilename += QString("2") + m_skinSuffix + ".png";
	}

	else {
		butFilename += QString("3") + m_skinSuffix + ".png";
	}

	this->ui->btn_mute->setIcon(QIcon(butFilename));

}

void GUI_SimplePlayer::muteButtonPressed() {

	if (m_mute) {
		m_mute = false;
		this->ui->volumeSlider->setEnabled(true);

		m_muteAction->setIcon(QIcon(Helper::getIconPath() + "vol_mute.png"));
		m_muteAction->setText("Mute");

		setupVolButton(this->ui->volumeSlider->value());

		emit volumeChanged((qreal) this->ui->volumeSlider->value());
	}

	else {
		m_mute = true;
		this->ui->volumeSlider->setEnabled(false);
		this->ui->btn_mute->setIcon(
				QIcon(Helper::getIconPath() + "vol_mute.png"));
		m_muteAction->setIcon(QIcon(Helper::getIconPath() + "vol_3.png"));
		m_muteAction->setText("Unmute");

		setupVolButton(0);

		emit volumeChanged(0);
	}

}

void GUI_SimplePlayer::cover_changed(bool success, QString cover_path) {

	if (!success){
		this->ui->albumCover->setPixmap(
				QPixmap::fromImage(
						QImage(Helper::getIconPath() + "append.png")));
		this->ui->albumCover->repaint();
		return;
	}



	if (!QFile::exists(cover_path)) {
		this->ui->albumCover->setPixmap(
				QPixmap::fromImage(
						QImage(Helper::getIconPath() + "append.png")));
		this->ui->albumCover->repaint();
		return;
	}

	QPixmap cover = QPixmap::fromImage(QImage(cover_path));
	this->ui->albumCover->setPixmap(cover);
	this->ui->albumCover->repaint();
}

void GUI_SimplePlayer::coverClicked(bool) {

	emit wantMoreCovers();
}

void GUI_SimplePlayer::hideAllPlugins(){
	showPlaylistChooser(false);
	showEqualizer(false);
	showRadio(false);
	this->ui->plugin_widget->hide();

}

void GUI_SimplePlayer::showPlaylistChooser(bool vis){

	QRect rect;
	if (vis){

		if(!m_isEqHidden || !m_isRadioHidden){
			ui->action_ViewEqualizer->setChecked(false);
			ui->action_ViewRadio->setChecked(false);
		}

		ui_eq->hide();
		ui_radio->hide();

		rect = ui->playlist_widget->geometry();

		rect.setTop(rect.top() + ui_playlist_chooser->height());
		rect.setHeight(ui_playlist->height() - ui_playlist_chooser->height());
		rect.setWidth(ui_playlist->width());

		ui->plugin_widget->setMinimumSize(rect.width(), ui_playlist_chooser->height());
		ui->plugin_widget->setMaximumSize(rect.width(), ui_playlist_chooser->height());
		ui_playlist_chooser->setMinimumSize(rect.width(), ui_playlist_chooser->height());
		ui_playlist_chooser->setMaximumSize(rect.width(), ui_playlist_chooser->height());

		ui_playlist_chooser->setVisible(true);
		ui->plugin_widget->show();
		m_isPcHidden = false;

	}

	else {
		rect = ui->playlist_widget->geometry();
		if(!ui_playlist_chooser->isHidden()){
			rect.setTop(rect.top() - ui_playlist_chooser->height());
			rect.setHeight(ui_playlist->height() + ui_playlist_chooser->height());
		}

		ui_playlist_chooser->hide();
		ui->plugin_widget->hide();
		m_isPcHidden = true;
	}
	ui->playlist_widget->setGeometry(rect);
	ui_playlist->resize(ui->playlist_widget->size());


}

void GUI_SimplePlayer::showEqualizer(bool vis) {

	QRect rect;
	if (vis) {

		if(!m_isPcHidden || !m_isRadioHidden){
			ui->action_ViewPlaylistChooser->setChecked(false);
			ui->action_ViewRadio->setChecked(false);
		}

		ui_playlist_chooser->hide();
		ui_radio->hide();

		rect = ui->playlist_widget->geometry();

		rect.setTop(rect.top() + ui_eq->height());
		rect.setHeight(ui_playlist->height() - ui_eq->height());
		rect.setWidth(ui_playlist->width());

		ui->plugin_widget->setMinimumSize(rect.width(), ui_eq->height());
		ui->plugin_widget->setMaximumSize(rect.width(), ui_eq->height());
		ui_eq->setMinimumSize(rect.width(), ui_eq->height());
		ui_eq->setMaximumSize(rect.width(), ui_eq->height());


		ui_eq->setVisible(true);
		ui->plugin_widget->show();
		m_isEqHidden = false;
	}


	else {
		rect = ui->playlist_widget->geometry();
		if(!m_isEqHidden){
			rect.setTop(rect.top() - ui_eq->height());
			rect.setHeight(ui_playlist->height() + ui_eq->height());
		}

		ui_eq->hide();
		ui->plugin_widget->hide();

		m_isEqHidden = true;
	}

	ui->playlist_widget->setGeometry(rect);
	ui_playlist->resize(ui->playlist_widget->size());

}

void GUI_SimplePlayer::showRadio(bool vis){

	QRect rect;
		if (vis) {

			if(!m_isPcHidden || !m_isEqHidden){
				ui->action_ViewPlaylistChooser->setChecked(false);
				ui->action_ViewEqualizer->setChecked(false);
			}

			ui_playlist_chooser->hide();
			ui_eq->hide();

			rect = ui->playlist_widget->geometry();

			rect.setTop(rect.top() + ui_radio->height());
			rect.setHeight(ui_playlist->height() - ui_radio->height());
			rect.setWidth(ui_playlist->width());

			ui->plugin_widget->setMinimumSize(rect.width(), ui_radio->height());
			ui->plugin_widget->setMaximumSize(rect.width(), ui_radio->height());
			ui_radio->setMinimumSize(rect.width(), ui_radio->height());
			ui_radio->setMaximumSize(rect.width(), ui_radio->height());


			ui_radio->setVisible(true);
			ui->plugin_widget->show();
			m_isRadioHidden = false;
		}


		else {
			rect = ui->playlist_widget->geometry();
			if(!m_isRadioHidden){
				rect.setTop(rect.top() - ui_radio->height());
				rect.setHeight(ui_playlist->height() + ui_radio->height());
			}

			ui_radio->hide();
			ui->plugin_widget->hide();
			m_isRadioHidden = true;
		}

		ui->playlist_widget->setGeometry(rect);
		ui_playlist->resize(ui->playlist_widget->size());
}

void GUI_SimplePlayer::close_playlist_chooser(){
	showPlaylistChooser(false);
	ui->action_ViewPlaylistChooser->setChecked(false);
}

void GUI_SimplePlayer::close_eq() {
	showEqualizer(false);
	ui->action_ViewEqualizer->setChecked(false);
}


void GUI_SimplePlayer::close_radio() {
	showRadio(false);
	ui->action_ViewRadio->setChecked(false);
}

void GUI_SimplePlayer::changeEvent(QEvent *event) {

	if (event->type() == QEvent::WindowStateChange) {

		if (isMinimized())
			hide();
		else
			show();
	}

}


void GUI_SimplePlayer::setupIcons() {

}

void GUI_SimplePlayer::setupActions() {
    m_playAction = new QAction(tr("Play"), this);
    m_playAction->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    m_stopAction = new QAction(tr("Stop"), this);
    m_stopAction->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
    m_bwdAction = new QAction(tr("Previous"), this);
    m_bwdAction->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));
    m_fwdAction = new QAction(tr("Next"), this);
    m_fwdAction->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
    m_muteAction = new QAction(tr("Mute"), this);
    m_muteAction->setIcon(QIcon(Helper::getIconPath() + "vol_mute.png"));
    m_closeAction = new QAction(tr("Close"), this);
    m_closeAction->setIcon(QIcon(Helper::getIconPath() + "close.png"));
    connect(m_stopAction, SIGNAL(triggered()), this, SLOT(stopClicked()));
    connect(m_bwdAction, SIGNAL(triggered()), this, SLOT(backwardClicked()));
    connect(m_fwdAction, SIGNAL(triggered()), this, SLOT(forwardClicked()));
    connect(m_muteAction, SIGNAL(triggered()), this, SLOT(muteButtonPressed()));
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_playAction, SIGNAL(triggered()), this, SLOT(playClicked()));

}

void GUI_SimplePlayer::connectTrayIcon() {
    connect(this->m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayItemActivated(QSystemTrayIcon::ActivationReason)));
    connect(this->m_trayIcon, SIGNAL(onShowNormal()), this, SLOT(showNormal()));
    connect(this->m_trayIcon, SIGNAL(onVolumeChangedByWheel(int)), this, SLOT(volumeChangedByTick(int)));

}


void GUI_SimplePlayer::trayItemActivated (QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        if (this->isMinimized() || isHidden())
            this->showNormal();
        if (!this->isActiveWindow())
            this->activateWindow();
        else {
            hide();
        }
        break;
    case QSystemTrayIcon::MiddleClick:
        this -> playClicked (false);
        break;
    default:
        break;
    }
}


void GUI_SimplePlayer::keyPressEvent(QKeyEvent* e) {
	e->accept();

	switch (e->key()) {


		case Qt::Key_MediaPlay:
			if(!m_radio_active)
				playClicked(true);
			break;


		case Qt::Key_MediaStop:
			stopClicked();
			break;


		case Qt::Key_MediaNext:
			forwardClicked(true);
			break;


		case Qt::Key_MediaPrevious:
			if(!m_radio_active)
				backwardClicked(true);
			break;


		case (Qt::Key_E):
			this->ui->action_ViewEqualizer->setChecked(!this->ui->action_ViewEqualizer->isChecked());
			break;


		case (Qt::Key_P):
			this->ui->action_ViewPlaylistChooser->setChecked(!this->ui->action_ViewPlaylistChooser->isChecked());
			break;

		case (Qt::Key_R):
			this->ui->action_ViewRadio->setChecked(!this->ui->action_ViewRadio->isChecked());
			break;

		default:
			break;

	}

}

QWidget* GUI_SimplePlayer::getParentOfPlaylist() {
	return this->ui->playlist_widget;
}

QWidget* GUI_SimplePlayer::getParentOfPlaylistChooser(){
	return this->ui->plugin_widget;
}

QWidget* GUI_SimplePlayer::getParentOfLibrary() {
	return this->ui->library_widget;
}

QWidget* GUI_SimplePlayer::getParentOfEqualizer() {
	return this->ui->plugin_widget;
}

QWidget* GUI_SimplePlayer::getParentOfRadio(){
	return this->ui->plugin_widget;
}


void GUI_SimplePlayer::setPlaylist(GUI_Playlist* playlist) {
	ui_playlist = playlist;


}

void GUI_SimplePlayer::setLibrary(GUI_Library_windowed* library) {

	ui_library = library;
}


void GUI_SimplePlayer::setEqualizer(GUI_Equalizer* eq) {
	ui_eq = eq;
}

void GUI_SimplePlayer::setPlaylistChooser(GUI_PlaylistChooser* playlist_chooser){
	ui_playlist_chooser = playlist_chooser;
}

void GUI_SimplePlayer::setRadio(GUI_RadioWidget* radio){
	ui_radio = radio;
}

void GUI_SimplePlayer::resizeEvent(QResizeEvent* e) {

	Q_UNUSED(e);
	this->ui_playlist->resize(this->ui->playlist_widget->size());
	this->ui_library->resize(this->ui->library_widget->size());

	QWidget* w;
	bool resize_plugin = false;
	if(!m_isEqHidden && ui_eq){
		w = ui_eq;
		resize_plugin = true;
	}

	else if(!m_isPcHidden && ui_playlist_chooser){
		w = ui_playlist_chooser;
		resize_plugin = true;
	}

	else if(!m_isRadioHidden && ui_radio){
		w = ui_radio;
		resize_plugin = true;
	}

	if(resize_plugin){
		ui->plugin_widget->setMinimumSize(ui_playlist->width(), w->height());
		ui->plugin_widget->setMaximumSize(ui_playlist->width(), w->height());
		w->setMinimumSize(ui_playlist->width(), w->height());
		w->setMaximumSize(ui_playlist->width(), w->height());
	}


	CSettingsStorage::getInstance()->setPlayerSize(this->size());
}

void GUI_SimplePlayer::lastFMClicked(bool b) {

	Q_UNUSED(b);
	emit setupLastFM();

}

void GUI_SimplePlayer::reloadLibraryClicked(bool b) {
	Q_UNUSED(b);
	emit reloadLibrary();
}

void GUI_SimplePlayer::importFolderClicked(bool b){
	Q_UNUSED(b);

	QString dir = QFileDialog::getExistingDirectory(this, "Open Directory",
				QDir::homePath(), QFileDialog::ShowDirsOnly);

	if(dir.size() > 0){
		qDebug() << "import " << dir;
		emit importDirectory(dir);
	}


}


void GUI_SimplePlayer::setLibraryPathClicked(bool b) {
	Q_UNUSED(b);

	QString start_dir = QDir::homePath();
	QString old_dir = CSettingsStorage::getInstance()->getLibraryPath();
	if (old_dir != "")
		start_dir = old_dir;

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			old_dir, QFileDialog::ShowDirsOnly);
	if (dir != "") {
		emit libpath_changed(dir);
		CSettingsStorage::getInstance()->setLibraryPath(dir);

		QMessageBox dialog;

		dialog.setFocus();
		dialog.setIcon(QMessageBox::Question);
		dialog.setText("<b>Library</b>");
		dialog.setInformativeText("Do you want to reload the Library?");
		dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		dialog.setDefaultButton(QMessageBox::Yes);

		int answer = dialog.exec();
		if(answer == QMessageBox::Yes)
			emit reloadLibrary();

		dialog.close();

	}

}

void GUI_SimplePlayer::fetch_all_covers_clicked(bool b) {
	Q_UNUSED(b);
	emit fetch_all_covers();
}

void GUI_SimplePlayer::album_cover_pressed() {
	emit fetch_alternate_covers(this->m_metadata);
}

void GUI_SimplePlayer::load_pl_on_startup_toggled(bool b){
	Q_UNUSED(b);
	CSettingsStorage::getInstance()->setLoadPlaylist(ui->action_load_playlist->isChecked());
}


void GUI_SimplePlayer::show_notification_toggled(bool active){

	CSettingsStorage::getInstance()->setShowNotifications(active);
}

void GUI_SimplePlayer::set_radio_active(bool b){
	m_radio_active = b;
	this->ui->btn_bw->setEnabled(!b);
	this->ui->btn_play->setEnabled(!b);
	this->ui->songProgress->setEnabled(!b);
	this->m_bwdAction->setEnabled(!b);
	this->m_playAction->setEnabled(!b);
}


void GUI_SimplePlayer::about(bool b){
	Q_UNUSED(b);

	QMessageBox::information(this, "About",
			"<b><font size=\"+2\">Sayonara Player 0.1.120107</font></b><br /><br />Written by Lucio Carreras<br /><br />License: GPL<br /><br />Copyright 2011-2012");
}
