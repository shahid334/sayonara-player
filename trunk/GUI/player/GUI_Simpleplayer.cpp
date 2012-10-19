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
#include "GUI/player/GUI_TrayIcon.h"
#include "GUI/alternate_covers/GUI_Alternate_Covers.h"
#include "ui_GUI_Simpleplayer.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"
#include "CoverLookup/CoverLookup.h"
#include "Engine/Engine.h"
#include "StreamPlugins/LastFM/LastFM.h"


#include <QList>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QPalette>


GUI_SimplePlayer::GUI_SimplePlayer(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::SimplePlayer), VOLUME_STEP_SIZE_PERC (5){
	ui->setupUi(this);
	initGUI();

	CSettingsStorage* settings = CSettingsStorage::getInstance();

	this->ui->albumCover->setIcon(QIcon(Helper::getIconPath() + "append.png"));


	m_playing = false;
	m_mute = false;


    m_min2tray = settings->getMinimizeToTray();

	ui_playlist = 0;
	ui_playlist_chooser = 0;
	ui_lfm_radio = 0;
	ui_eq = 0;
	ui_stream = 0;

	m_skinSuffix = "";
	m_class_name = "Player";

	QSize size = settings->getPlayerSize();
	QRect rect = this->geometry();
	rect.setWidth(size.width());
	rect.setHeight(size.height());
	this->setGeometry(rect);

	m_cov_lookup = new CoverLookup(m_class_name);
	m_alternate_covers = new GUI_Alternate_Covers(this->centralWidget(), m_class_name);


	ui->action_ViewLFMRadio->setVisible(settings->getLastFMActive());


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
	qDebug() << "closing player...";
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

	this->ui->btn_rec->setIcon(QIcon(Helper::getIconPath() + "rec.png"));
	this->ui->btn_rec->setVisible(false);

	this->ui->btn_stop->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
	this->ui->btn_fw->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
	this->ui->btn_bw->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));
	this->ui->btn_correct->setIcon(QIcon(Helper::getIconPath() + "edit.png"));
	//this->ui->btn_correct->setText(QString("(c)"));
	this->ui->btn_correct->setToolTip("Correct ID3 Tag");

	this->ui->action_ViewEqualizer->setText("Equalizer\t\tSTRG+e");
	this->ui->action_ViewStream->setText("Stream\t\tSTRG+s");
	this->ui->action_ViewPlaylistChooser->setText("Playlist Chooser\tSTRG+p");
	this->ui->action_ViewLFMRadio->setText("Last.fm Radio\t\tSTRG+r");
	this->ui->action_viewLibrary->setText("Library\t\tSTRG+l");
	this->ui->btn_correct->setVisible(false);
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
	connect(this->ui->btn_rec, SIGNAL(toggled(bool)), this,
				SLOT(sl_rec_button_toggled(bool)));
	connect(this->ui->btn_correct, SIGNAL(clicked(bool)), this,
			SLOT(correct_btn_clicked(bool)));
	connect(this->ui->albumCover, SIGNAL(clicked()), this, SLOT(coverClicked()));

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
	connect(this->ui->action_viewLibrary, SIGNAL(toggled(bool)), this,
			SLOT(showLibrary(bool)));
	connect(this->ui->action_ViewEqualizer, SIGNAL(toggled(bool)), this,
				SLOT(show_eq(bool)));
	connect(this->ui->action_ViewLFMRadio, SIGNAL(toggled(bool)), this,
				SLOT(show_lfm_radio(bool)));


	connect(this->ui->action_ViewStream, SIGNAL(toggled(bool)), this,
					SLOT(show_stream(bool)));
	connect(this->ui->action_ViewPlaylistChooser, SIGNAL(toggled(bool)), this,
				SLOT(show_playlist_chooser(bool)));
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
	connect(this->ui->action_streamrecorder, SIGNAL(triggered(bool)), this,
			SLOT(sl_action_streamripper_toggled(bool)));
	connect(this->ui->action_SocketConnection, SIGNAL(triggered(bool)), this,
			SLOT(sl_action_socket_connection_triggered(bool)));


	// about
	connect(this->ui->action_about, SIGNAL(triggered(bool)), this, SLOT(about(bool)));

    connect(this->m_trayIcon, SIGNAL(onVolumeChangedByWheel(int)), this, SLOT(volumeChangedByTick(int)));


    connect(this->ui->volumeSlider, SIGNAL(searchSliderMoved(int)), this,
			SLOT(volumeChanged(int)));
    connect(this->ui->volumeSlider, SIGNAL(searchSliderReleased(int)), this,
    		SLOT(volumeChanged(int)));
    connect(this->ui->volumeSlider, SIGNAL(searchSliderPressed(int)), this,
    		SLOT(volumeChanged(int)));


    connect(this->ui->songProgress, SIGNAL(searchSliderReleased(int)), this,
    		SLOT(setProgressJump(int)));
	connect(this->ui->songProgress, SIGNAL(searchSliderPressed(int)), this,
			SLOT(setProgressJump(int)));
	connect(this->ui->songProgress, SIGNAL(searchSliderMoved(int)), this,
			SLOT(setProgressJump(int)));



	// cover lookup
	connect(this->m_cov_lookup, SIGNAL(sig_cover_found(QString, QString)),
			this, 				SLOT(cover_changed(QString, QString)));

	connect(this, 				SIGNAL(sig_want_cover(const MetaData&)),
			this->m_cov_lookup, SLOT(search_cover(const MetaData&)));

	connect(this,				SIGNAL(sig_fetch_all_covers()),
			this->m_cov_lookup, SLOT(search_all_covers()));

	connect(this->m_alternate_covers, SIGNAL(sig_covers_changed(QString)),
			this,				SLOT(sl_alternate_cover_available(QString)));

}


// new track
void GUI_SimplePlayer::update_track(const MetaData & md) {

    this->ui->songProgress->setValue(0);
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
	this->ui->maxTime->setText(lengthString);
	this->ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));

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

	m_playAction->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
	m_playAction->setText("Pause");

	m_completeLength_ms = md.length_ms;
	m_playing = true;
	m_trayIcon->playStateChanged(this->m_playing);

	QString cover_path = Helper::get_cover_path(md.artist, md.album);
	if(! QFile::exists(cover_path) ){
		cover_path = Helper::getIconPath() + "append.png";
		emit sig_want_cover(md);
	}


	this->ui->albumCover->setIcon(QIcon(cover_path));

	setCurrentPosition(0);
	this->ui->btn_correct->setVisible(false);
	this->ui->albumCover->repaint();
	setRadioMode(md.radio_mode);
	this->repaint();
}


// public slot:
// id3 tags have changed
void GUI_SimplePlayer::psl_id3_tags_changed(MetaDataList& v_md) {

	MetaData md_new;
	bool found = false;

	for(uint i=0; i<v_md.size(); i++){
		if(m_metadata.id == v_md[i].id){
			m_metadata = v_md[i];
			found = true;
			break;
		}
	}

	if(!found) return;

	if (m_metadata.year < 1000 || m_metadata.album.contains(QString::number(m_metadata.year)))
		this->ui->album->setText(m_metadata.album);

	else
		this->ui->album->setText(
				m_metadata.album + " (" + QString::number(m_metadata.year) + ")");

	this->ui->artist->setText(m_metadata.artist);
	this->ui->title->setText(m_metadata.title);

	if( this->ui->action_notification->isChecked() ){
		m_trayIcon->songChangedMessage(QString ("Currently playing: \"" + m_metadata.title + "\" by " + m_metadata.artist));
	}

	this->setWindowTitle(QString("Sayonara - ") + m_metadata.title);

	emit sig_want_cover(m_metadata);
	this->ui->btn_correct->setVisible(false);
    //this->repaint();
}




/** LAST FM **/
void GUI_SimplePlayer::last_fm_logged_in(bool b){

	if(!b && CSettingsStorage::getInstance()->getLastFMActive() && !m_suppress_warning)
		QMessageBox::warning(this->ui->centralwidget, "Warning", "You are not logged in to LastFM!");

	show_lfm_radio(false);
	ui->action_ViewLFMRadio->setChecked(false);

	this->ui->action_ViewLFMRadio->setVisible(b);
}

void GUI_SimplePlayer::psl_lfm_activated(bool b){

	show_lfm_radio(false);
	ui->action_ViewLFMRadio->setChecked(false);

	this->ui->action_ViewLFMRadio->setVisible(b);
}

void GUI_SimplePlayer::lfm_info_fetched(const MetaData& md, bool loved, bool corrected){

	m_metadata_corrected = md;
	this->ui->btn_correct->setVisible(corrected && CSettingsStorage::getInstance()->getLastFMCorrections());

	if(loved){
		this->ui->title->setText(this->ui->title->text());
	}

	this->repaint();
}

void GUI_SimplePlayer::correct_btn_clicked(bool b){
	emit sig_correct_id3(m_metadata_corrected);

}
/** LAST FM **/


/** PROGRESS BAR **/
void GUI_SimplePlayer::total_time_changed(qint64 total_time) {

    m_completeLength_ms = total_time;
    this->ui->maxTime->setText(getLengthString(total_time));
}

void GUI_SimplePlayer::setProgressJump(int percent){

    if(percent > 100 || percent < 0) {
        percent = 0;
    }
    long cur_pos_ms = (percent * m_metadata.length_ms) / 100;
    QString curPosString = Helper::cvtMsecs2TitleLengthString(cur_pos_ms);
    this->ui->curTime->setText(curPosString);

    emit search(percent);
}

void GUI_SimplePlayer::setCurrentPosition(quint32 pos_sec) {

    if (m_completeLength_ms != 0) {

		int newSliderVal = (pos_sec * 100000) / (m_completeLength_ms);

        if (!this->ui->songProgress->isSearching() && newSliderVal < ui->songProgress->maximum()){
            this->ui->songProgress->setValue(newSliderVal);
		}
	}

    else if(pos_sec > m_completeLength_ms / 1000){
    	this->ui->songProgress->setValue(0);
    }


    if(!this->ui->songProgress->isSearching()){
        int min, sec;
        if(m_completeLength_ms != 0 && pos_sec > m_completeLength_ms) pos_sec = 0;
        Helper::cvtSecs2MinAndSecs(pos_sec, &min, &sec);

        QString curPosString = Helper::cvtSomething2QString(min, 2)
        + QString(':') + Helper::cvtSomething2QString(sec, 2);

        this->ui->curTime->setText(curPosString);
    }

}

/** PROGRESS BAR END **/


void GUI_SimplePlayer::setStyle(int style){
	bool dark = (style == 1);
	changeSkin(dark);
	this->ui->action_Dark->setChecked(dark);
}

void GUI_SimplePlayer::changeSkin(bool dark) {


	QString menu_style = Style::get_menu_style(dark);
	QString btn_style;

	this->ui->menubar->setStyleSheet(Style::get_menubar_style(dark));
	this->ui->menuFle->setStyleSheet(menu_style);
	this->ui->menuPreferences->setStyleSheet(menu_style);
	this->ui->menuView->setStyleSheet(menu_style);
	this->ui->menuAbout->setStyleSheet(menu_style);

	if (dark) {

		this->ui->centralwidget->setStyleSheet(
				"background-color: " + Style::get_player_back_color() + "; color: #D8D8D8;");
		this->setStyleSheet(
				"background-color: " + Style::get_player_back_color() + "; color: #D8D8D8;");


		QString style = Style::get_btn_style(8);
		this->ui->btn_mute->setStyleSheet(style);
		this->ui->btn_play->setStyleSheet(style);
		this->ui->btn_fw->setStyleSheet(style);
		this->ui->btn_bw->setStyleSheet(style);
		this->ui->btn_stop->setStyleSheet(style);
		this->ui->btn_rec->setStyleSheet(style);

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
		this->ui->btn_rec->setStyleSheet(Style::get_btn_style(8));
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

void GUI_SimplePlayer::sl_rec_button_toggled(bool b){

	emit sig_rec_button_toggled(b);
}


/** PLAYER BUTTONS **/
void GUI_SimplePlayer::playClicked(bool) {

	bool play_tmp = false;
	if (this->m_playing == true) {
		this->ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
		m_playAction->setIcon(QIcon(Helper::getIconPath() + "play.png"));
		m_playAction->setText("Play");

		emit pause();

	}

	else {
		this->ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
		m_playAction->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
		m_playAction->setText("Pause");

		emit play();
		play_tmp = true;
	}

	this->ui->albumCover->setFocus();
	this->m_playing = play_tmp;

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
    this->ui->curTime->setText("0:00");
	this->ui->maxTime->setText("00:00");


	this->ui->albumCover->setIcon(QIcon(Helper::getIconPath() + "append.png"));

	this->ui->albumCover->setFocus();
	this -> m_trayIcon->playStateChanged (this->m_playing);
	emit stop();
}

void GUI_SimplePlayer::backwardClicked(bool) {

    this->ui->albumCover->setFocus();
    int cur_pos_sec =  (m_completeLength_ms * this->ui->songProgress->value()) / 100000;
    if(cur_pos_sec > 3){
        setProgressJump(0);

    }

    else{
        emit backward();
    }
}

void GUI_SimplePlayer::forwardClicked(bool) {
	this->ui->albumCover->setFocus();
	emit forward();
}
/** PLAYER BUTTONS END **/




/** VOLUME **/
void GUI_SimplePlayer::setVolume(int vol) {
	this->ui->volumeSlider->setValue(vol);
	setupVolButton(vol);
	emit sig_volume_changed(vol);
}

void GUI_SimplePlayer::volumeChanged(int volume_percent) {
	setupVolButton(volume_percent);
    this->ui->volumeSlider->setValue(volume_percent);
	emit sig_volume_changed(volume_percent);

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



    if (currentVolumeOrig_perc != currentVolume_perc) {

        volumeChanged(currentVolume_perc);
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

		emit sig_volume_changed(this->ui->volumeSlider->value());
	}

	else {
		m_mute = true;
		this->ui->volumeSlider->setEnabled(false);
		this->ui->btn_mute->setIcon(
				QIcon(Helper::getIconPath() + "vol_mute.png"));
		m_muteAction->setIcon(QIcon(Helper::getIconPath() + "vol_3.png"));
		m_muteAction->setText("Unmute");

		setupVolButton(0);
		emit sig_volume_changed(0);
	}

}

/** VOLUME END **/



/** COVERS **/
void GUI_SimplePlayer::coverClicked() {

	if(m_metadata.album_id >= 0)
		m_alternate_covers->start(m_metadata.album_id, true);

   else if(m_metadata.radio_mode == RADIO_STATION){
        QString searchstring = QString("Radio ") + m_metadata.title;
        QString targetpath = Helper::get_cover_path(m_metadata.artist, m_metadata.album);

        m_alternate_covers->start(searchstring, targetpath);
    }

    else {

        QString searchstring;
        if(m_metadata.album.size() != 0 || m_metadata.artist != 0){
            searchstring = m_metadata.album + " " + m_metadata.artist;
        }

        else {
            searchstring = m_metadata.title + " " + m_metadata.artist;
        }

        searchstring = searchstring.trimmed();

        QString targetpath = Helper::get_cover_path(m_metadata.artist, m_metadata.album);

        m_alternate_covers->start(searchstring, targetpath);
    }



	this->setFocus();
}

void GUI_SimplePlayer::sl_alternate_cover_available(QString target_class){

	Q_UNUSED(target_class);

	QString coverpath = Helper::get_cover_path(m_metadata.artist, m_metadata.album);
	this->ui->albumCover->setIcon(QIcon(coverpath));

}


// public slot
// cover was found by CoverLookup
void GUI_SimplePlayer::cover_changed(QString caller_class, QString cover_path) {

	if(m_class_name != caller_class) return;

	// found cover is not for the player but for sth else
	QString our_coverpath = Helper::get_cover_path(m_metadata.artist, m_metadata.album);

	if(	our_coverpath.toLower() != cover_path.toLower() ||
		!QFile::exists(cover_path) ){

		cover_path = Helper::getIconPath() + "append.png";
	}

	this->ui->albumCover->setIcon(QIcon(cover_path));
	this->ui->albumCover->repaint();
}


/** COVER END **/





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
    m_showAction = new QAction(tr("Show"), this);


    connect(m_stopAction, SIGNAL(triggered()), this, SLOT(stopClicked()));
    connect(m_bwdAction, SIGNAL(triggered()), this, SLOT(backwardClicked()));
    connect(m_fwdAction, SIGNAL(triggered()), this, SLOT(forwardClicked()));
    connect(m_muteAction, SIGNAL(triggered()), this, SLOT(muteButtonPressed()));
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(really_close()));
    connect(m_playAction, SIGNAL(triggered()), this, SLOT(playClicked()));
    connect(m_showAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    m_trayIcon = new GUI_TrayIcon(QIcon(Helper::getIconPath() + "play.png"), QIcon(Helper::getIconPath() + "pause.png"),this);
    m_trayIcon ->setupMenu(m_closeAction,m_playAction, m_stopAction,m_muteAction,m_fwdAction,m_bwdAction, m_showAction);
    m_trayIcon->playStateChanged(true);

    connect(this->m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayItemActivated(QSystemTrayIcon::ActivationReason)));
   	connect(this->m_trayIcon, SIGNAL(onVolumeChangedByWheel(int)), this, SLOT(volumeChangedByTick(int)));
    m_trayIcon->show();

}

void GUI_SimplePlayer::trayItemActivated (QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {

    case QSystemTrayIcon::Trigger:

        if (this->isMinimized() || isHidden()){
            this->setHidden(false);
            this->showNormal();
            this->activateWindow();
        }

        else if(m_min2tray){
            this->setHidden(true);
        }


        break;
    case QSystemTrayIcon::MiddleClick:
        this -> playClicked (false);
        break;
    default:
        break;
    }
}

/** TRAY ICON END **/





// public fct
QWidget* GUI_SimplePlayer::getParentOfPlaylist() {
	return this->ui->playlist_widget;
}

QWidget* GUI_SimplePlayer::getParentOfLibrary() {
	return this->ui->library_widget;
}


// public fct
void GUI_SimplePlayer::setPlaylist(GUI_Playlist* playlist) {
	ui_playlist = playlist;
    if(ui_playlist){
        ui_playlist->show();
        ui_playlist->resize(this->ui->playlist_widget->size());
    }
}

// public fct
void GUI_SimplePlayer::setLibrary(GUI_Library_windowed* library) {
	ui_library = library;
    if(ui_library){
        ui_library->show();
        ui_library->resize(this->ui->library_widget->size());
    }

}



/** OVERLOADED EVENTS **/
// protected fct
void GUI_SimplePlayer::resizeEvent(QResizeEvent* e) {

	Q_UNUSED(e);
	this->ui_playlist->resize(this->ui->playlist_widget->size());
	this->ui_library->resize(this->ui->library_widget->size());

	QSize sz = this->ui->plugin_widget->size();

    if(ui_eq && !ui_eq->isHidden())
		ui_eq->resize(sz);

    if(ui_stream && !ui_stream->isHidden())
		ui_stream->resize(sz);

    if(ui_lfm_radio && !ui_lfm_radio->isHidden())
		ui_lfm_radio->resize(sz);

    if(ui_playlist_chooser && !ui_playlist_chooser->isHidden())
		ui_playlist_chooser->resize(sz);

	CSettingsStorage::getInstance()->setPlayerSize(this->size());
}


void GUI_SimplePlayer::keyPressEvent(QKeyEvent* e) {
	e->accept();

	switch (e->key()) {

		case Qt::Key_MediaPlay:
            if(m_metadata.radio_mode == RADIO_OFF)
				playClicked(true);
			break;

		case Qt::Key_MediaStop:
			stopClicked();
			break;

		case Qt::Key_MediaNext:
			forwardClicked(true);
			break;

		case Qt::Key_MediaPrevious:
            if(m_metadata.radio_mode == RADIO_OFF)
				backwardClicked(true);
			break;

		case (Qt::Key_E):
			this->ui->action_ViewEqualizer->setChecked(!this->ui->action_ViewEqualizer->isChecked());
			break;

		case (Qt::Key_P):
			this->ui->action_ViewPlaylistChooser->setChecked(!this->ui->action_ViewPlaylistChooser->isChecked());
			break;

		case (Qt::Key_R):
			this->ui->action_ViewLFMRadio->setChecked(!this->ui->action_ViewLFMRadio->isChecked());
			break;

		case (Qt::Key_S):
					this->ui->action_ViewStream->setChecked(!this->ui->action_ViewStream->isChecked());
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


void GUI_SimplePlayer::closeEvent(QCloseEvent* e){

    if(m_min2tray){
        e->ignore();
        this->hide();
    }
}

/** OVERLOADED EVENTS END **/


void GUI_SimplePlayer::really_close(){
    m_min2tray = false;
    this->close();
}




// prvt fct
void GUI_SimplePlayer::setRadioMode(int radio){


	bool stream_ripper = CSettingsStorage::getInstance()->getStreamRipper();
	this->ui->btn_bw->setEnabled(radio == RADIO_OFF);
	this->ui->btn_fw->setEnabled(radio != RADIO_STATION);

	if(stream_ripper){
		this->ui->btn_play->setVisible(radio == RADIO_OFF);
		this->ui->btn_rec->setVisible(radio != RADIO_OFF);
	}

	else{
		this->ui->btn_play->setEnabled(radio == RADIO_OFF);
		this->ui->btn_rec->setVisible(false);
	}

    this->ui->songProgress->setEnabled(radio == RADIO_OFF);
    this->m_bwdAction->setEnabled(radio == RADIO_OFF);
	this->m_fwdAction->setEnabled(radio != RADIO_STATION);
    this->m_playAction->setEnabled(radio == RADIO_OFF);

}

// public slot
void GUI_SimplePlayer::psl_strrip_set_active(bool b){

	if(b){
        this->ui->btn_play->setVisible(m_metadata.radio_mode == RADIO_OFF);
        this->ui->btn_rec->setVisible(m_metadata.radio_mode != RADIO_OFF);
	}

	else{
		this->ui->btn_play->setVisible(true);
        this->ui->btn_play->setEnabled(m_metadata.radio_mode == RADIO_OFF);
		this->ui->btn_rec->setVisible(false);
	}
}


void GUI_SimplePlayer::suppress_warning(bool b){
	 m_suppress_warning = b;
}
