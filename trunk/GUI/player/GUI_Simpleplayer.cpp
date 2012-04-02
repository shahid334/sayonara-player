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


#include "GUI/player/GUI_Simpleplayer.h"
#include "GUI/stream/GUI_Stream.h"
#include "GUI/GUI_TrayIcon.h"
#include "ui_GUI_Simpleplayer.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>


GUI_SimplePlayer::GUI_SimplePlayer(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::GUI_SimplePlayer), VOLUME_STEP_SIZE_PERC (2){
	ui->setupUi(this);
	initGUI();

	CSettingsStorage* settings = CSettingsStorage::getInstance();

	this->ui->albumCover->setPixmap(
			QPixmap::fromImage(QImage(Helper::getIconPath() + "append.png")));


	m_playing = false;
	m_cur_searching = false;
	m_mute = false;
	m_radio_active = RADIO_OFF;

	m_min2tray = settings->getMinimizeToTray();

	ui_playlist = 0;
	ui_playlist_chooser = 0;
	ui_radio = 0;
	ui_eq = 0;

	m_skinSuffix = "";

	ui_stream_dialog = new GUI_Stream();
	ui_stream_dialog->setModal(true);
	ui_stream_dialog->hide();

	QSize size = settings->getPlayerSize();
	QRect rect = this->geometry();
	rect.setWidth(size.width());
	rect.setHeight(size.height());
	this->setGeometry(rect);


	this->ui->action_min2tray->setChecked(m_min2tray);

	bool loadPlaylistChecked = settings->getLoadPlaylist();
	this->ui->action_load_playlist->setChecked(loadPlaylistChecked);

	bool loadShowNotifications = settings->getShowNotification();
	this->ui->action_notification->setChecked(loadShowNotifications);

	bool showSmallPlaylistItems = settings->getShowSmallPlaylist();
	this->ui->action_smallPlaylistItems->setChecked(showSmallPlaylistItems);

	QSizePolicy p = this->ui->library_widget->sizePolicy();
	m_library_stretch_factor = p.horizontalStretch();

	bool show_library = settings->getShowLibrary();
	if(!show_library){
		p.setHorizontalStretch(0);
		this->ui->library_widget->setSizePolicy(p);
		m_library_width = 300;
	}

	this->ui->action_viewLibrary->setChecked(show_library);

	/* TRAY ACTIONS */
	this->setupTrayActions();

	/* SIGNALS AND SLOTS */
	this->setupConnections();

	this->ui->plugin_widget->resize(this->ui->plugin_widget->width(), 0);

}


GUI_SimplePlayer::~GUI_SimplePlayer() {
	delete ui;
	delete m_playAction;
	delete m_stopAction;

	delete m_bwdAction;
	delete m_fwdAction;
	delete m_muteAction;
	delete m_closeAction;
}



void GUI_SimplePlayer::initGUI() {

	this->ui->btn_mute->setIcon(QIcon(Helper::getIconPath() + "vol_1.png"));
	this->ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
	this->ui->btn_stop->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
	this->ui->btn_fw->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
	this->ui->btn_bw->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));

	this->ui->action_ViewEqualizer->setText("Equalizer\t\tSTRG+e");
	this->ui->action_ViewPlaylistChooser->setText("Playlist Chooser\tSTRG+p");
	this->ui->action_ViewRadio->setText("Last.fm Radio\t\tSTRG+r");
	this->ui->action_viewLibrary->setText("Library\t\tSTRG+l");
}

void GUI_SimplePlayer::setupConnections(){

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
	connect(this->ui->action_OpenStream, SIGNAL(triggered(bool)), this,
			SLOT(streamDialogClicked(bool)));
	connect(this->ui->action_OpenFolder, SIGNAL(triggered(bool)), this,
			SLOT(folderSelectedClicked(bool)));
	connect(this->ui->action_ImportFolder, SIGNAL(triggered(bool)), this,
				SLOT(importFolderClicked()));
	connect(this->ui->action_reloadLibrary, SIGNAL(triggered(bool)), this,
				SLOT(reloadLibraryClicked(bool)));


	// view
	connect(this->ui->action_viewLibrary, SIGNAL(toggled(bool)), this,
			SLOT(showLibrary(bool)));
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
	connect(this->ui->action_smallPlaylistItems, SIGNAL(toggled(bool)), this,
			SLOT(small_playlist_items_toggled(bool)));

	// preferences
	connect(this->ui->action_lastFM, SIGNAL(triggered(bool)), this,
			SLOT(lastFMClicked(bool)));
	connect(this->ui->action_setLibPath, SIGNAL(triggered(bool)), this,
			SLOT(setLibraryPathClicked(bool)));
	connect(this->ui->action_fetch_all_covers, SIGNAL(triggered(bool)), this,
			SLOT(fetch_all_covers_clicked(bool)));
	connect(this->ui->action_load_playlist, SIGNAL(toggled(bool)), this,
			SLOT(load_pl_on_startup_toggled(bool)));
	connect(this->ui->action_min2tray, SIGNAL(toggled(bool)), this,
			SLOT(min2tray_toggled(bool)));

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

	connect(this->ui_stream_dialog, SIGNAL(sig_play_stream(const QString&, const QString&)), this,
				SLOT(play_stream_selected(const QString&, const QString&)));

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

	m_trayIcon->setToolTip(QString("\"") +
			md.title +
			"\" by \"" +
			md.artist +
			QString("\""));

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
			QString("<font color=\"#000000\">") +
			QString::number(md.bitrate / 1000) +
			QString(" kBit/s") +
			QString("</font>"));

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
	}

	else{
		this->ui->maxTime->setText("");
	}

	int min, sec;

	Helper::cvtSecs2MinAndSecs(pos_sec, &min, &sec);

	QString curPosString = Helper::cvtSomething2QString(min, 2)
	+ QString(':') + Helper::cvtSomething2QString(sec, 2);

	this->ui->curTime->setText(curPosString);
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


void GUI_SimplePlayer::setVolume(int vol) {
	this->ui->volumeSlider->setValue(vol);
	setupVolButton(vol);
	emit volumeChanged((qreal) vol);
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
							"Media files (*.mp3 *.wav *.flac *.aac *.wma *.avi *.mpg *.mpeg *.m3u *.pls *.asx)"));

	if (list.size() > 0)
		emit fileSelected(list);
}

void GUI_SimplePlayer::streamDialogClicked(bool) {

	QRect geo = ui_stream_dialog->geometry();
	geo.setX(this->geometry().x() + 20);
	geo.setY(this->geometry().y() + 100);
	//geo.setWidth(200);
	geo.setHeight(80);
	ui_stream_dialog->setGeometry(geo);
	ui_stream_dialog->show();
}

void GUI_SimplePlayer::play_stream_selected(const QString& url, const QString& name){
	emit sig_stream_selected(url, name);
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



void GUI_SimplePlayer::showLibrary(bool b){

	CSettingsStorage* settings = CSettingsStorage::getInstance();

	settings->setShowLibrary(b);

	if(!b){

		QSizePolicy p = this->ui->library_widget->sizePolicy();

		m_library_width = this->ui->library_widget->width();
		m_library_stretch_factor = p.horizontalStretch();

		p.setHorizontalStretch(0);
		this->ui->library_widget->setSizePolicy(p);

		this->resize(this->width() - m_library_width, this->height());
	}

	else{
		QSizePolicy p = this->ui->library_widget->sizePolicy();
		p.setHorizontalStretch(m_library_stretch_factor);
		this->ui->library_widget->setSizePolicy(p);

		this->resize(this->width() + m_library_width, this->height());
	}
}

void GUI_SimplePlayer::hideUnneededPlugins(QWidget* wannashow){
	if(ui_eq != wannashow)
		this->ui->action_ViewEqualizer->setChecked(false);

	if(ui_radio != wannashow)
		this->ui->action_ViewRadio->setChecked(false);

	if(ui_playlist_chooser != wannashow)
		this->ui->action_ViewPlaylistChooser->setChecked(false);
}

void GUI_SimplePlayer::hideAllPlugins(){

	if(ui_eq)
		this->ui_eq->hide();

	if(ui_radio)
		this->ui_radio->hide();

	if(ui_playlist_chooser)
		this->ui_playlist_chooser->hide();

	this->ui->plugin_widget->setMinimumHeight(0);
}


void GUI_SimplePlayer::showPlugin(QWidget* widget, bool v){

	if(!widget) return;

	int old_h = this->ui->plugin_widget->minimumHeight();
	int h = widget->height();

	QSize pl_size;
	if(ui_playlist)
		pl_size = ui_playlist->size();
	QSize widget_size = widget->size();

	if (v){
		hideUnneededPlugins(widget);
		this->ui->plugin_widget->show();
		widget->show();
		widget_size.setWidth(this->ui->plugin_widget->width());
		pl_size.setHeight(pl_size.height() - h + old_h);

		this->ui->plugin_widget->setMinimumHeight(widget->height());
		widget->resize(widget_size);
	}

	else{
		widget->hide();
		pl_size.setHeight(pl_size.height() + old_h);
		this->ui->plugin_widget->setMinimumHeight(0);

	}

	if(ui_playlist)
		ui_playlist->resize(pl_size);


}

void GUI_SimplePlayer::showPlaylistChooser(bool vis){

	if(vis) CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_PLAYLIST_CHOOSER);
	else CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_NONE);

	showPlugin(ui_playlist_chooser, vis);

}

void GUI_SimplePlayer::showEqualizer(bool vis) {

	if(vis) CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_EQUALIZER);
	else CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_NONE);

	showPlugin(ui_eq, vis);
}

void GUI_SimplePlayer::showRadio(bool vis){

	if(vis) CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_RADIO);
	else CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_NONE);

	showPlugin(ui_radio, vis);
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

	if(!m_min2tray) return;

	if (event->type() == QEvent::WindowStateChange) {

		if (isMinimized())
			hide();
		else
			show();
	}
}


void GUI_SimplePlayer::setupIcons() {

}


/** TRAY ICON **/


void GUI_SimplePlayer::setupTrayActions() {
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

    m_trayIcon = new GUI_TrayIcon(QIcon(Helper::getIconPath() + "play.png"), QIcon(Helper::getIconPath() + "pause.png"),this);
    m_trayIcon ->setupMenu(m_closeAction,m_playAction, m_stopAction,m_muteAction,m_fwdAction,m_bwdAction);

    connect(this->m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayItemActivated(QSystemTrayIcon::ActivationReason)));
   	connect(this->m_trayIcon, SIGNAL(onVolumeChangedByWheel(int)), this, SLOT(volumeChangedByTick(int)));
    m_trayIcon->show();
}


void GUI_SimplePlayer::trayItemActivated (QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        if (this->isMinimized() || isHidden())
            this->showNormal();
        if (!this->isActiveWindow())
            this->activateWindow();
        else if(m_min2tray){
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
	qDebug() << "Key = " << e->key();

	switch (e->key()) {

		case Qt::Key_MediaPlay:
			if(m_radio_active == RADIO_OFF)
				playClicked(true);
			break;

		case Qt::Key_MediaStop:
			stopClicked();
			break;

		case Qt::Key_MediaNext:
			forwardClicked(true);
			break;

		case Qt::Key_MediaPrevious:
			if(m_radio_active == RADIO_OFF)
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

		case (Qt::Key_L):
			this->ui->action_viewLibrary->setChecked(!this->ui->action_viewLibrary->isChecked());
			break;

		case (Qt::Key_F11):
			if(!this->isFullScreen())
				this->showFullScreen();
			else this->showNormal();
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
	ui_playlist->resize(this->ui->playlist_widget->size());
}

void GUI_SimplePlayer::setLibrary(GUI_Library_windowed* library) {
	ui_library = library;
	ui_library->resize(this->ui->library_widget->size());
}

void GUI_SimplePlayer::check_show_plugins(){

	int shown_plugin = CSettingsStorage::getInstance()->getShownPlugin();

	switch(shown_plugin){

		case PLUGIN_EQUALIZER:
			ui->action_ViewEqualizer->setChecked(true);
			showEqualizer(true);
			break;

		case PLUGIN_RADIO:
			ui->action_ViewRadio->setChecked(true);
			showRadio(true);
			break;

		case PLUGIN_PLAYLIST_CHOOSER:
			ui->action_ViewPlaylistChooser->setChecked(true);
			showPlaylistChooser(true);
			break;

		case PLUGIN_NONE:
		default:
			break;
	}


}


void GUI_SimplePlayer::setEqualizer(GUI_Equalizer* eq) {
	ui_eq = eq;
	ui_eq->resize(this->ui->plugin_widget->size());
	//check_show_plugins();
}

void GUI_SimplePlayer::setPlaylistChooser(GUI_PlaylistChooser* playlist_chooser){
	ui_playlist_chooser = playlist_chooser;
	ui_playlist_chooser->resize(this->ui->plugin_widget->size());
	//check_show_plugins();
}

void GUI_SimplePlayer::setRadio(GUI_RadioWidget* radio){
	ui_radio = radio;
	ui_radio->resize(this->ui->plugin_widget->size());
	//check_show_plugins();
}

void GUI_SimplePlayer::resizeEvent(QResizeEvent* e) {

	Q_UNUSED(e);
	this->ui_playlist->resize(this->ui->playlist_widget->size());
	this->ui_library->resize(this->ui->library_widget->size());

	if(!ui_eq->isHidden())
		ui_eq->resize(this->ui->plugin_widget->size());
	if(!ui_radio->isHidden())
		ui_radio->resize(this->ui->plugin_widget->size());
	if(!ui_playlist_chooser->isHidden())
		ui_playlist_chooser->resize(this->ui->plugin_widget->size());

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

void GUI_SimplePlayer::min2tray_toggled(bool b){
	CSettingsStorage::getInstance()->setMinimizeToTray(b);
	m_min2tray = b;
}

void GUI_SimplePlayer::small_playlist_items_toggled(bool b){
	CSettingsStorage::getInstance()->setShowSmallPlaylist(b);
	emit show_small_playlist_items(b);
}

void GUI_SimplePlayer::set_radio_active(int radio){

	m_radio_active = radio;
	this->ui->btn_bw->setEnabled(radio == RADIO_OFF);
	this->ui->btn_fw->setEnabled(radio != RADIO_STATION);
	this->ui->btn_play->setEnabled(radio == RADIO_OFF);
	this->ui->songProgress->setEnabled(radio == RADIO_OFF);
	this->m_bwdAction->setEnabled(radio == RADIO_OFF);
	this->m_fwdAction->setEnabled(radio != RADIO_STATION);
	this->m_playAction->setEnabled(radio == RADIO_OFF);
}


void GUI_SimplePlayer::about(bool b){
	Q_UNUSED(b);

	QMessageBox::information(this, "About",
			"<b><font size=\"+2\">Sayonara Player 0.1.120205</font></b><br /><br />Written by Lucio Carreras<br /><br />License: GPL<br /><br />Copyright 2011-2012");
}
