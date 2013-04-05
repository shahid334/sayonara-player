/* GUI_Simpleplayer.cpp */

/* Copyright (C) 2012  Lucio Carreras
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



#include "GUI/ui_GUI_Simpleplayer.h"
#include "GUI/player/GUI_Simpleplayer.h"
#include "GUI/player/GUI_TrayIcon.h"
#include "GUI/stream/GUI_Stream.h"
#include "GUI/Podcasts/GUI_Podcasts.h"
#include "GUI/alternate_covers/GUI_Alternate_Covers.h"

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

GUI_SimplePlayer* obj_ref = 0;
void signal_handler(int sig);

#ifdef Q_OS_UNIX
#include <signal.h>


void signal_handler(int sig){
	if(sig == SIGWINCH && obj_ref){
		qDebug() << "show everything";
		obj_ref->setHidden(false);
		obj_ref->showNormal();
		obj_ref->activateWindow();
	}

	else if(sig == 15 && obj_ref){
		qDebug() << "extern close event";
		obj_ref->really_close();
	}
}

#endif


GUI_SimplePlayer::GUI_SimplePlayer(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::SimplePlayer) {
	ui->setupUi(this);
	initGUI();

    m_settings = CSettingsStorage::getInstance();

    ui->albumCover->setIcon(QIcon(Helper::getIconPath() + "logo.png"));

    ui->artist->setText(m_settings->getVersion());
	ui->album->setText(tr("Written by") + " Lucio Carreras");

    m_metadata_available = false;
	m_playing = false;
	m_mute = false;

	ui_playlist = 0;

    ui_notifications = new GUI_Notifications(this);
    ui_startup_dialog = new GUI_Startup_Dialog(this);

	m_skinSuffix = "";
	m_class_name = "Player";



	m_cov_lookup = new CoverLookup(m_class_name);
	m_alternate_covers = new GUI_Alternate_Covers(this->centralWidget(), m_class_name);

    ui->action_ViewLFMRadio->setVisible(m_settings->getLastFMActive());

    m_min2tray = m_settings->getMinimizeToTray();
    ui->action_min2tray->setChecked(m_min2tray);
    ui->action_only_one_instance->setChecked(m_settings->getAllowOnlyOneInstance());

    bool showSmallPlaylistItems = m_settings->getShowSmallPlaylist();
	ui->action_smallPlaylistItems->setChecked(showSmallPlaylistItems);

    bool showOnlyTracks = m_settings->getLibShowOnlyTracks();
	ui->action_showOnlyTracks->setChecked(showOnlyTracks);

	QSizePolicy p = ui->library_widget->sizePolicy();
	m_library_stretch_factor = p.horizontalStretch();

    bool show_library = m_settings->getShowLibrary();
    ui->action_viewLibrary->setChecked(show_library);
    this->showLibrary(show_library);

    bool live_search = m_settings->getLibLiveSheach();
    this->ui->action_livesearch->setChecked(live_search);


    bool is_fullscreen = m_settings->getPlayerFullscreen();
    if(!is_fullscreen){
        QSize size = m_settings->getPlayerSize();
        QRect rect = this->geometry();
        rect.setWidth(size.width());
        rect.setHeight(size.height());
        this->setGeometry(rect);
    }

    m_library_width = 600;

	/* TRAY ACTIONS */
	this->setupTrayActions();

	/* SIGNALS AND SLOTS */
    this->setupConnections();

	ui->plugin_widget->resize(ui->plugin_widget->width(), 0);
    ui_info_dialog = 0;

    changeSkin(m_settings->getPlayerStyle() == 1);
}


GUI_SimplePlayer::~GUI_SimplePlayer() {
	qDebug() << "closing player...";
	delete ui;
}


void GUI_SimplePlayer::initGUI() {

	QString ctrl = tr("Ctrl");

	ui->btn_mute->setIcon(QIcon(Helper::getIconPath() + "vol_1.png"));
	ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    ui->btn_rec->setIcon(QIcon(Helper::getIconPath() + "rec.png"));
	ui->btn_rec->setVisible(false);

	ui->btn_stop->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
	ui->btn_fw->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
	ui->btn_bw->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));
	ui->btn_correct->setIcon(QIcon(Helper::getIconPath() + "edit.png"));
    ui->btn_correct->setToolTip(tr("Correct ID3 Tag"));

    ui->action_viewLibrary->setText(tr("&Library"));
    ui->action_viewLibrary->setShortcut(QKeySequence(ctrl+"+l"));

    ui->action_Fullscreen->setShortcut(QKeySequence("F11"));
    ui->action_Dark->setShortcut(QKeySequence("F10"));


    ui->btn_correct->setVisible(false);
}

void GUI_SimplePlayer::setupConnections(){

	connect(ui->btn_play, SIGNAL(clicked(bool)), this,
			SLOT(playClicked(bool)));
	connect(ui->btn_fw, SIGNAL(clicked(bool)), this,
			SLOT(forwardClicked(bool)));
	connect(ui->btn_bw, SIGNAL(clicked(bool)), this,
			SLOT(backwardClicked(bool)));
    connect(ui->btn_stop, SIGNAL(clicked()), this,
            SLOT(stopClicked()));
	connect(ui->btn_mute, SIGNAL(released()), this,
			SLOT(muteButtonPressed()));
	connect(ui->btn_rec, SIGNAL(toggled(bool)), this,
				SLOT(sl_rec_button_toggled(bool)));
	connect(ui->btn_correct, SIGNAL(clicked(bool)), this,
			SLOT(correct_btn_clicked(bool)));
	connect(ui->albumCover, SIGNAL(clicked()), this, SLOT(coverClicked()));

	// file
	connect(ui->action_OpenFile, SIGNAL(triggered(bool)), this,
			SLOT(fileSelectedClicked(bool)));

	connect(ui->action_OpenFolder, SIGNAL(triggered(bool)), this,
			SLOT(folderSelectedClicked(bool)));
	connect(ui->action_ImportFolder, SIGNAL(triggered(bool)), this,
				SLOT(importFolderClicked()));
	connect(ui->action_reloadLibrary, SIGNAL(triggered(bool)), this,
				SLOT(reloadLibraryClicked(bool)));
        connect(ui->action_clearLibrary, SIGNAL(triggered(bool)), this,
				SLOT(clearLibraryClicked(bool)));

	connect(ui->action_Close, SIGNAL(triggered(bool)), this,
				SLOT(really_close(bool)));


	// view
	connect(ui->action_viewLibrary, SIGNAL(toggled(bool)), this,
			SLOT(showLibrary(bool)));
	connect(ui->action_Dark, SIGNAL(toggled(bool)), this,
			SLOT(changeSkin(bool)));

	connect(ui->action_smallPlaylistItems, SIGNAL(toggled(bool)), this,
			SLOT(small_playlist_items_toggled(bool)));
	connect(ui->action_showOnlyTracks, SIGNAL(toggled(bool)), this,
			SLOT(sl_show_only_tracks(bool)));
	connect(ui->action_Fullscreen, SIGNAL(toggled(bool)), this,
			SLOT(show_fullscreen_toggled(bool)));


	// preferencesF
	connect(ui->action_lastFM, SIGNAL(triggered(bool)), this,
			SLOT(lastFMClicked(bool)));
	connect(ui->action_setLibPath, SIGNAL(triggered(bool)), this,
			SLOT(setLibraryPathClicked(bool)));
	connect(ui->action_fetch_all_covers, SIGNAL(triggered(bool)), this,
			SLOT(fetch_all_covers_clicked(bool)));
    connect(ui->action_startup, SIGNAL(triggered(bool)), ui_startup_dialog,
            SLOT(show()));
	connect(ui->action_min2tray, SIGNAL(toggled(bool)), this,
			SLOT(min2tray_toggled(bool)));
	connect(ui->action_only_one_instance, SIGNAL(toggled(bool)), this,
				SLOT(only_one_instance_toggled(bool)));

	connect(ui->action_streamrecorder, SIGNAL(triggered(bool)), this,
			SLOT(sl_action_streamripper_toggled(bool)));
    connect(ui->action_notifications, SIGNAL(triggered(bool)), ui_notifications,
            SLOT(show()));
	connect(ui->action_SocketConnection, SIGNAL(triggered(bool)), this,
			SLOT(sl_action_socket_connection_triggered(bool)));

    connect(ui->action_livesearch, SIGNAL(triggered(bool)), this,
            SLOT(sl_live_search(bool)));


	// about
	connect(ui->action_about, SIGNAL(triggered(bool)), this, SLOT(about(bool)));

	connect(ui->action_help, SIGNAL(triggered(bool)), this, SLOT(help(bool)));
    connect(m_trayIcon, SIGNAL(onVolumeChangedByWheel(int)), this, SLOT(volumeChangedByTick(int)));


    connect(ui->volumeSlider, SIGNAL(searchSliderMoved(int)), this,
			SLOT(volumeChanged(int)));
    connect(ui->volumeSlider, SIGNAL(searchSliderReleased(int)), this,
    		SLOT(volumeChanged(int)));
    connect(ui->volumeSlider, SIGNAL(searchSliderPressed(int)), this,
    		SLOT(volumeChanged(int)));


    connect(ui->songProgress, SIGNAL(searchSliderReleased(int)), this,
    		SLOT(setProgressJump(int)));
	connect(ui->songProgress, SIGNAL(searchSliderPressed(int)), this,
			SLOT(setProgressJump(int)));
	connect(ui->songProgress, SIGNAL(searchSliderMoved(int)), this,
			SLOT(setProgressJump(int)));



	// cover lookup
	connect(m_cov_lookup, SIGNAL(sig_cover_found(QString, QString)),
			this, 				SLOT(cover_changed(QString, QString)));

	connect(this, 				SIGNAL(sig_want_cover(const MetaData&)),
			m_cov_lookup, SLOT(search_cover(const MetaData&)));

	connect(this,				SIGNAL(sig_fetch_all_covers()),
			m_cov_lookup, 		SLOT(search_all_covers()));

    connect(m_alternate_covers, SIGNAL(sig_covers_changed(QString, QString)),
            this,				SLOT(sl_alternate_cover_available(QString, QString)));

    connect(m_alternate_covers, SIGNAL(sig_no_cover()),
            this,				SLOT(sl_no_cover_available()));


    // notifications
    connect(ui_notifications, SIGNAL(sig_settings_changed(bool,int)),
            this, SLOT(notification_changed(bool,int)));



    qDebug() << "connections done";
}


// new track
void GUI_SimplePlayer::update_track(const MetaData & md, int pos_sec, bool playing) {

    m_metadata = md;

    m_completeLength_ms = md.length_ms;
    m_playing = playing;
    m_trayIcon->setPlaying(playing);

    setCurrentPosition(pos_sec);

	// sometimes ignore the date
	if (md.year < 1000 || md.album.contains(QString::number(md.year)))
        ui->album->setText(Helper::get_album_w_disc(md));

	else
		ui->album->setText(
                Helper::get_album_w_disc(md) + " (" + QString::number(md.year) + ")");

	ui->artist->setText(md.artist);
	ui->title->setText(md.title);


    m_trayIcon->songChangedMessage(md);
	/*m_trayIcon->setToolTip(QString("\"") +
			md.title +
			"\" by \"" +
			md.artist +
			QString("\""));*/

	QString lengthString = Helper::cvtMsecs2TitleLengthString(md.length_ms, true);
	ui->maxTime->setText(lengthString);

    if(m_playing)
        ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
    else
        ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));

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

	ui->rating->setText(tmp);
	ui->rating->setToolTip(
			QString("<font color=\"#000000\">") +
			QString::number(md.bitrate / 1000) +
			QString(" kBit/s") +
			QString("</font>"));

	this->setWindowTitle(QString("Sayonara - ") + md.title);

	QString cover_path = Helper::get_cover_path(md.artist, md.album);
	if(! QFile::exists(cover_path) ){
        if(md.radio_mode != RADIO_STATION){
            cover_path = Helper::getIconPath() + "logo.png";
            emit sig_want_cover(md);
        }

        else
        	cover_path = Helper::getIconPath() + "radio.png";
	}

	ui->btn_correct->setVisible(false);

    ui->albumCover->setIcon(QIcon(cover_path));
	ui->albumCover->repaint();

	setRadioMode(md.radio_mode);
    m_metadata_available = true;
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

	ui->btn_correct->setVisible(false);

	if (m_metadata.year < 1000 || m_metadata.album.contains(QString::number(m_metadata.year)))
		ui->album->setText(m_metadata.album);

	else
		ui->album->setText(
				m_metadata.album + " (" + QString::number(m_metadata.year) + ")");

	ui->artist->setText(m_metadata.artist);
	ui->title->setText(m_metadata.title);

    m_trayIcon->songChangedMessage(m_metadata);

	this->setWindowTitle(QString("Sayonara - ") + m_metadata.title);

	emit sig_want_cover(m_metadata);
}




/** LAST FM **/
void GUI_SimplePlayer::last_fm_logged_in(bool b){

    if(!b && m_settings->getLastFMActive())
        QMessageBox::warning(ui->centralwidget, tr("Warning"), tr("Cannot login to Last.fm"));

    if(!b){
	/// TODO
//        show_lfm_radio(false);
        ui->action_ViewLFMRadio->setChecked(false);
    }

	ui->action_ViewLFMRadio->setVisible(b);
}


void GUI_SimplePlayer::psl_lfm_activated(bool b){
/// TODO
//    show_lfm_radio(false);
	ui->action_ViewLFMRadio->setChecked(false);

	ui->action_ViewLFMRadio->setVisible(b);
}


void GUI_SimplePlayer::lfm_info_fetched(const MetaData& md, bool loved, bool corrected){

    m_metadata_corrected = md;

    bool radio_off = (m_metadata.radio_mode == RADIO_OFF);
    bool get_lfm_corrections = m_settings->getLastFMCorrections();

    ui->btn_correct->setVisible(corrected &&
    							radio_off &&
    							get_lfm_corrections);

	if(loved){
		ui->title->setText(ui->title->text());
	}

	this->repaint();
}

void GUI_SimplePlayer::correct_btn_clicked(bool b){

    if(!ui_info_dialog)
        return;

    MetaData md = m_metadata_corrected;
    m_metadata_corrected = m_metadata;

    bool same_artist = (m_metadata.artist.compare(md.artist) == 0);
    bool same_album = (m_metadata.album.compare(md.album) == 0);
    bool same_title = (m_metadata.title.compare(md.title) == 0);

    if(!same_artist){
        m_metadata_corrected.artist = md.artist;
        m_metadata_corrected.artist_id = -1;
    }

    if(!same_album){
        m_metadata_corrected.album = md.album;
        m_metadata_corrected.album_id = -1;
    }

    if(!same_title){
        m_metadata_corrected.title = md.title;
    }

    MetaDataList lst;
    lst.push_back(m_metadata_corrected);
    ui_info_dialog->setMetaData(lst);
    ui_info_dialog->setMode(INFO_MODE_TRACKS);

    ui_info_dialog->show(TAB_EDIT);

}
/** LAST FM **/


void GUI_SimplePlayer::setStyle(int style){

	bool dark = (style == 1);
	changeSkin(dark);
	ui->action_Dark->setChecked(dark);
}

void GUI_SimplePlayer::changeSkin(bool dark) {

    QString stylesheet = Style::get_style(dark);

	this->setStyleSheet(stylesheet);

	if (dark) 	m_skinSuffix = QString("_dark");
	else 		m_skinSuffix = QString("");

    m_settings->setPlayerStyle(dark ? 1 : 0);
    this->m_trayIcon->change_skin(stylesheet);

    setupVolButton(ui->volumeSlider->value());
    emit sig_skin_changed(dark);
}



/** TRAY ICON **/
void GUI_SimplePlayer::setupTrayActions() {


	m_trayIcon = new GUI_TrayIcon(this);


    connect(m_trayIcon, SIGNAL(sig_stop_clicked()), this, SLOT(stopClicked()));
    connect(m_trayIcon, SIGNAL(sig_bwd_clicked()), this, SLOT(backwardClicked()));
    connect(m_trayIcon, SIGNAL(sig_fwd_clicked()), this, SLOT(forwardClicked()));
    connect(m_trayIcon, SIGNAL(sig_mute_clicked()), this, SLOT(muteButtonPressed()));
    connect(m_trayIcon, SIGNAL(sig_close_clicked()), this, SLOT(really_close()));
    connect(m_trayIcon, SIGNAL(sig_play_clicked()), this, SLOT(playClicked()));
    connect(m_trayIcon, SIGNAL(sig_pause_clicked()), this, SLOT(playClicked()));
    connect(m_trayIcon, SIGNAL(sig_show_clicked()), this, SLOT(showNormal()));

    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
      		this, 		SLOT(trayItemActivated(QSystemTrayIcon::ActivationReason)));

    connect(m_trayIcon, SIGNAL(onVolumeChangedByWheel(int)),
   			this, 		SLOT(volumeChangedByTick(int)));


    m_trayIcon->setPlaying(false);

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

        else this->showMinimized();


        break;
    case QSystemTrayIcon::MiddleClick:
        this -> playClicked (false);
        break;
    default:
        break;
    }
}

/** TRAY ICON END **/



/** LIBRARY AND PLAYLIST **/

QWidget* GUI_SimplePlayer::getParentOfPlaylist() {
	return ui->playlist_widget;
}

QWidget* GUI_SimplePlayer::getParentOfLibrary() {
	return ui->library_widget;
}


void GUI_SimplePlayer::setPlaylist(GUI_Playlist* playlist) {
	ui_playlist = playlist;
    if(ui_playlist){
        ui_playlist->show();
        ui_playlist->resize(ui->playlist_widget->size());
    }
}


void GUI_SimplePlayer::setLibrary(GUI_Library_windowed* library) {
    ui_library = library;
    if(ui_library){
        ui_library->show();
        ui_library->resize(ui->library_widget->size());
    }
}

void GUI_SimplePlayer::setPlayerPluginHandler(PlayerPluginHandler* pph){
	_pph = pph;

	QList<PlayerPlugin*> lst = _pph->get_all_plugins();
    QList<QAction*> actions;

    foreach(PlayerPlugin* p, lst){
		QAction* action = p->getAction();
		// action is connected in Plugin itself
        actions << action;
	}


    this->ui->menuView->insertActions(this->ui->action_Dark, actions);
    this->ui->menuView->insertSeparator(this->ui->action_Dark);

	connect(_pph, SIGNAL(sig_show_plugin(PlayerPlugin*)), this, SLOT(showPlugin(PlayerPlugin*)));
    connect(_pph, SIGNAL(sig_hide_all_plugins()), this, SLOT(hideAllPlugins()));

}

void GUI_SimplePlayer::stopped(){
    m_metadata_available = false;
    stopClicked(false);
}


/** LIBRARY AND PLAYLIST END **/


// prvt fct
void GUI_SimplePlayer::setRadioMode(int radio){

    bool stream_ripper = m_settings->getStreamRipper();
	ui->btn_bw->setEnabled(radio == RADIO_OFF);
	ui->btn_fw->setEnabled(radio != RADIO_STATION);

	if(stream_ripper){

        bool btn_rec_visible = (radio != RADIO_OFF);

        if(btn_rec_visible){
            ui->btn_play->setVisible(radio == RADIO_OFF);
            ui->btn_rec->setVisible(radio != RADIO_OFF);
        }

        else {
            ui->btn_rec->setVisible(radio != RADIO_OFF);
            ui->btn_play->setVisible(radio == RADIO_OFF);
        }

        ui->btn_play->setEnabled(radio == RADIO_OFF);
	}

	else{

		ui->btn_rec->setVisible(false);
        ui->btn_play->setVisible(true);
		ui->btn_play->setEnabled(radio == RADIO_OFF);
	}

    m_trayIcon->set_enable_play(radio == RADIO_OFF);
    m_trayIcon->set_enable_fwd(radio != RADIO_STATION);
    m_trayIcon->set_enable_bwd(radio == RADIO_OFF);

    ui->songProgress->setEnabled(radio == RADIO_OFF);

    emit sig_rec_button_toggled(ui->btn_rec->isChecked() && ui->btn_rec->isVisible());
}


// public slot
void GUI_SimplePlayer::psl_strrip_set_active(bool b){

	if(b){
        ui->btn_play->setVisible(m_metadata.radio_mode == RADIO_OFF);
        ui->btn_rec->setVisible(m_metadata.radio_mode != RADIO_OFF);
	}

	else{
		ui->btn_play->setVisible(true);
        ui->btn_play->setEnabled(m_metadata.radio_mode == RADIO_OFF);
		ui->btn_rec->setVisible(false);
	}

    emit sig_rec_button_toggled(ui->btn_rec->isChecked() && ui->btn_rec->isVisible());
}


void GUI_SimplePlayer::ui_loaded(){

    #ifdef Q_OS_UNIX
		obj_ref = this;
		signal(SIGWINCH, signal_handler);
		signal(15, signal_handler);
	#endif

    changeSkin(m_settings->getPlayerStyle() == 1);
    this->ui->action_Fullscreen->setChecked(m_settings->getPlayerFullscreen());
}


void GUI_SimplePlayer::notification_changed(bool active, int timeout_ms){

    m_trayIcon->set_timeout(timeout_ms);
    m_trayIcon->set_notification_active(active);
}


void GUI_SimplePlayer::resizeEvent(QResizeEvent* e) {

    QMainWindow::resizeEvent(e);

    ui_playlist->resize(ui->playlist_widget->size());

    if(ui->library_widget->isVisible())
        ui_library->resize(ui->library_widget->size());

	QSize sz = ui->plugin_widget->size();


    _pph->resize(sz);
    m_settings->setPlayerSize(this->size());
    this->update();
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
		case (Qt::Key_P):
		case (Qt::Key_R):
		case (Qt::Key_S):
	        case (Qt::Key_O):
			break;

		case (Qt::Key_L):
			ui->action_viewLibrary->setChecked(!ui->action_viewLibrary->isChecked());
			break;

        case (Qt::Key_F10):
            ui->action_Dark->setChecked(!ui->action_Dark->isChecked());
            break;

		case (Qt::Key_F11):
			show_fullscreen_toggled(!this->isFullScreen());
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


void GUI_SimplePlayer::really_close(bool b){

	m_min2tray = false;
	this->close();
}


