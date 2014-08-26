/* GUI_Player.cpp */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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

#include "GUI/ui_GUI_Player.h"
#include "GUI/player/GUI_Player.h"
#include "GUI/player/GUI_TrayIcon.h"
#include "GUI/stream/GUI_Stream.h"
#include "GUI/Podcasts/GUI_Podcasts.h"
#include "GUI/alternate_covers/GUI_Alternate_Covers.h"

#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/AsyncWebAccess.h"
#include "CoverLookup/CoverLookup.h"

#include "StreamPlugins/LastFM/LastFM.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QPalette>

GUI_Player* obj_ref = 0;

#ifdef Q_OS_UNIX
#include <signal.h>

void signal_handler(int sig) {

    if(sig == SIGWINCH && obj_ref) {
        qDebug() << "show everything";
        obj_ref->setHidden(false);
        obj_ref->showNormal();
        obj_ref->activateWindow();
    }

    else if(sig == 15 && obj_ref) {
        qDebug() << "extern close event";
        obj_ref->really_close();
    }
    else {

        qDebug() << "signal " << sig << " received";
    }
}

#endif

GUI_Player::GUI_Player(QTranslator* translator, QWidget *parent) :
	QMainWindow(parent),
	Ui::Sayonara() {

	setupUi(this);

	initGUI();

    m_translator = translator;
    m_settings = CSettingsStorage::getInstance();

    m_awa_version = new AsyncWebAccess(this);
    m_awa_translators = new AsyncWebAccess(this);

	lab_sayonara->setText(tr("Sayonara Player"));
	lab_version->setText(m_settings->getVersion());
	lab_writtenby->setText(tr("Written by") + " Lucio Carreras");
	lab_copyright->setText(tr("Copyright") + " 2011-2014");

    m_metadata_available = false;
	m_playing = false;
	m_mute = false;

	ui_playlist = 0;

    ui_notifications = new GUI_Notifications(this);
    ui_startup_dialog = new GUI_Startup_Dialog(this);
    ui_language_chooser= new GUI_LanguageChooser(this);

	m_skinSuffix = "";
	m_class_name = "Player";
	m_converter_active = false;

	m_cov_lookup = new CoverLookup(this);
	m_alternate_covers = new GUI_Alternate_Covers(this->centralWidget(), m_class_name);

	action_ViewLFMRadio->setVisible(m_settings->getLastFMActive());

    m_min2tray = m_settings->getMinimizeToTray();
	action_min2tray->setChecked(m_min2tray);
	action_only_one_instance->setChecked(m_settings->getAllowOnlyOneInstance());

    bool showSmallPlaylistItems = m_settings->getShowSmallPlaylist();
	action_smallPlaylistItems->setChecked(showSmallPlaylistItems);

    bool showOnlyTracks = m_settings->getLibShowOnlyTracks();
	action_showOnlyTracks->setChecked(showOnlyTracks);

	QSizePolicy p = library_widget->sizePolicy();
	m_library_stretch_factor = p.horizontalStretch();

    bool show_library = m_settings->getShowLibrary();
	action_viewLibrary->setChecked(show_library);
    this->showLibrary(show_library);

    bool live_search = m_settings->getLibLiveSheach();
	action_livesearch->setChecked(live_search);

    bool notify_new_version = m_settings->getNotifyNewVersion();
	action_notifyNewVersion->setChecked(notify_new_version);


    bool is_fullscreen = m_settings->getPlayerFullscreen();
    bool is_maximized = m_settings->getPlayerMaximized();
    if(!is_fullscreen & !is_maximized) {
        QSize size = m_settings->getPlayerSize();
        QPoint pos = m_settings->getPlayerPos();

        QRect rect = this->geometry();
        rect.setX(pos.x());
        rect.setY(pos.y());
        rect.setWidth(size.width());
        rect.setHeight(size.height());
        this->setGeometry(rect);
    }

    m_library_width = 600;

    QString lib_path = m_settings->getLibraryPath();
    ui_libpath = 0;
    if(lib_path.size() == 0) {
		ui_libpath = new GUI_LibraryPath( library_widget );
    }


	/* TRAY ACTIONS */
	this->setupTrayActions();

	/* SIGNALS AND SLOTS */
    this->setupConnections();
    m_awa_version->set_url("http://sayonara.luciocarreras.de/current_version");
    m_awa_translators->set_url("http://sayonara.luciocarreras.de/translators");
    m_awa_version->start();
    m_awa_translators->start();

	plugin_widget->resize(plugin_widget->width(), 0);
    ui_info_dialog = 0;

    changeSkin(m_settings->getPlayerStyle() == 1);

	stopClicked(false);
}


GUI_Player::~GUI_Player() {
	qDebug() << "closing player...";

}


void GUI_Player::language_changed(QString language) {

    m_translator->load(language, Helper::getSharePath() + "translations/");

	retranslateUi(this);

    ui_notifications->language_changed();
    ui_startup_dialog->language_changed();
    ui_language_chooser->language_changed(true);
    m_alternate_covers->language_changed();

    if(ui_libpath)
        ui_libpath->language_changed();

    QList<PlayerPlugin*> all_plugins = _pph->get_all_plugins();
	QList<QAction*> actions = menuView->actions();

    foreach(QAction* action, actions) {
        if(!action->data().isNull()) {
			menuView->removeAction(action);
        }
    }

    actions.clear();

    foreach(PlayerPlugin* p, all_plugins) {
        QAction* action = p->getAction();
        action->setData(p->getName());
        actions << action;
    }

	menuView->insertActions(action_Dark, actions);
	menuView->insertSeparator(action_Dark);

    emit sig_language_changed();
}


QAction* GUI_Player::createAction(QList<QKeySequence>& seq_list) {
    QAction* action = new QAction(this);

    action->setShortcuts(seq_list);
    action->setShortcutContext(Qt::ApplicationShortcut);
    this->addAction(action);
    connect(this, SIGNAL(destroyed()), action, SLOT(deleteLater()));

    return action;
}

QAction* GUI_Player::createAction(QKeySequence seq) {
    QList<QKeySequence> seq_list;
    seq_list << seq;
    return createAction(seq_list);
}


void GUI_Player::initGUI() {

	btn_mute->setIcon(QIcon(Helper::getIconPath() + "vol_1.png"));
	btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
	btn_rec->setIcon(QIcon(Helper::getIconPath() + "rec.png"));
	btn_stop->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
	btn_fw->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
	btn_bw->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));
	btn_correct->setIcon(QIcon(Helper::getIconPath() + "edit.png"));

	action_viewLibrary->setText(tr("&Library"));
	btn_rec->setVisible(false);

	action_Fullscreen->setShortcut(QKeySequence("F11"));
	action_Dark->setShortcut(QKeySequence("F10"));

	btn_correct->setVisible(false);
}



// new track
void GUI_Player::update_track(const MetaData & md, int pos_sec, bool playing) {

	QString rating_text;
	QString length_text;

    m_metadata = md;

    m_completeLength_ms = md.length_ms;
    m_playing = playing;
    m_trayIcon->setPlaying(playing);

    setCurrentPosition(pos_sec);

	lab_sayonara->hide();
	lab_title->show();

	lab_version->hide();
	lab_artist->show();

	lab_writtenby->hide();
	lab_album->show();

	lab_copyright->hide();
	lab_rating->show();


    if (md.year < 1000 || md.album.contains(QString::number(md.year)))
		 lab_album->setText(Helper::get_album_w_disc(md));

     else
		 lab_album->setText(
                 Helper::get_album_w_disc(md) + " (" + QString::number(md.year) + ")");

	lab_artist->setText(md.artist);
	lab_title->setText(md.title);

    m_trayIcon->songChangedMessage(md);

	length_text = Helper::cvtMsecs2TitleLengthString(md.length_ms, true);
	maxTime->setText(length_text);
	songProgress->setEnabled( md.length_ms > 0 );


    if(m_playing)
		btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
    else
		btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));



	rating_text += QString::number(md.bitrate / 1000) + " kBit/s";
	rating_text += ", " + QString::number( (double) (md.filesize / 1024) / 1024.0, 'f', 2) + " MB";

	lab_rating->setText(rating_text);
	lab_rating->setToolTip(rating_text);

    this->setWindowTitle(QString("Sayonara - ") + md.title);


	btn_correct->setVisible(false);

    fetch_cover();

	setRadioMode(md.radio_mode);

    m_metadata_available = true;

    this->repaint();
}



void GUI_Player::psl_bitrate_changed(qint32 bitrate) {

	QString tmp = QString("<font color=\"#FFAA00\" size=\"+10\">");
	if (bitrate <= 96000)
		tmp += "*";
	else if (bitrate <= 128000)
		tmp += "**";
	else if (bitrate <= 160000)
		tmp += "***";
	else if (bitrate <= 256000)
		tmp += "****";
	else
		tmp += "*****";
	tmp += "</font>";

	//lab_rating->setText(tmp);
	lab_rating->setToolTip(
			QString("<font color=\"#000000\">") +
			QString::number(bitrate / 1000) +
			QString(" kBit/s") +
			QString("</font>"));

	lab_rating->setText(
			//QString("<font color=\"#000000\">") +
			QString::number(bitrate / 1000) +
			QString(" kBit/s"));
		//	QString("</font>"));

}


void GUI_Player::psl_track_time_changed(MetaData& md) {
    m_completeLength_ms = md.length_ms;
    QString lengthString = Helper::cvtMsecs2TitleLengthString(md.length_ms, true);
	maxTime->setText(lengthString);
}


// public slot:
// id3 tags have changed
void GUI_Player::psl_id3_tags_changed(MetaDataList& v_md) {

	MetaData md_new;
	bool found = false;

	for(uint i=0; i<v_md.size(); i++) {
		if(m_metadata.id == v_md[i].id) {
			m_metadata = v_md[i];
			found = true;
			break;
		}
	}

	if(!found) return;

	btn_correct->setVisible(false);

	if (m_metadata.year < 1000 || m_metadata.album.contains(QString::number(m_metadata.year)))
		lab_album->setText(m_metadata.album);

	else
		lab_album->setText(
				m_metadata.album + " (" + QString::number(m_metadata.year) + ")");

	lab_artist->setText(m_metadata.artist);
	lab_title->setText(m_metadata.title);

    m_trayIcon->songChangedMessage(m_metadata);

	this->setWindowTitle(QString("Sayonara - ") + m_metadata.title);

	emit sig_want_cover(m_metadata);
}




/** LAST FM **/
void GUI_Player::last_fm_logged_in(bool b) {

    if(!b && m_settings->getLastFMActive())
		QMessageBox::warning(centralwidget, tr("Warning"), tr("Cannot login to Last.fm"));

    if(!b) {
		action_ViewLFMRadio->setChecked(false);
    }

	action_ViewLFMRadio->setVisible(b);
}


void GUI_Player::psl_lfm_activated(bool b) {

	action_ViewLFMRadio->setChecked(false);
	action_ViewLFMRadio->setVisible(b);
}


void GUI_Player::lfm_info_fetched(const MetaData& md, bool loved, bool corrected) {

    m_metadata_corrected = md;

    bool radio_off = (m_metadata.radio_mode == RADIO_OFF);
    bool get_lfm_corrections = m_settings->getLastFMCorrections();

	btn_correct->setVisible(corrected &&
    							radio_off &&
    							get_lfm_corrections);

	if(loved) {
		lab_title->setText(lab_title->text());
	}

	this->repaint();
}

void GUI_Player::correct_btn_clicked(bool b) {

    if(!ui_info_dialog)
        return;

    MetaData md = m_metadata_corrected;
    m_metadata_corrected = m_metadata;

    bool same_artist = (m_metadata.artist.compare(md.artist) == 0);
    bool same_album = (m_metadata.album.compare(md.album) == 0);
    bool same_title = (m_metadata.title.compare(md.title) == 0);

    if(!same_artist) {
        m_metadata_corrected.artist = md.artist;
        m_metadata_corrected.artist_id = -1;
    }

    if(!same_album) {
        m_metadata_corrected.album = md.album;
        m_metadata_corrected.album_id = -1;
    }

    if(!same_title) {
        m_metadata_corrected.title = md.title;
    }

	MetaDataList lst;
	lst.push_back(m_metadata_corrected);
    ui_info_dialog->setMetaData(lst);

    ui_info_dialog->show(TAB_EDIT);

}
/** LAST FM **/




void GUI_Player::setStyle(int style) {

	bool dark = (style == 1);
	changeSkin(dark);
	action_Dark->setChecked(dark);
}

void GUI_Player::changeSkin(bool dark) {

    QString stylesheet = Style::get_style(dark);

	this->setStyleSheet(stylesheet);

	if (dark) 	m_skinSuffix = QString("_dark");
	else 		m_skinSuffix = QString("");

    m_settings->setPlayerStyle(dark ? 1 : 0);
    this->m_trayIcon->change_skin(stylesheet);

	setupVolButton(volumeSlider->value());
    emit sig_skin_changed(dark);
}



/** TRAY ICON **/
void GUI_Player::setupTrayActions() {


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


void GUI_Player::trayItemActivated (QSystemTrayIcon::ActivationReason reason) {

    switch (reason) {

    case QSystemTrayIcon::Trigger:

        if (this->isMinimized() || isHidden()) {
            this->setHidden(false);
            this->showNormal();
            this->activateWindow();
        }

        else if(m_min2tray) {
            this->setHidden(true);
        }

        else this->showMinimized();


        break;
    case QSystemTrayIcon::MiddleClick:
    	m_trayIcon->songChangedMessage(m_metadata);
        break;
    default:
        break;
    }
}

/** TRAY ICON END **/



/** LIBRARY AND PLAYLIST **/

QWidget* GUI_Player::getParentOfPlaylist() {
	return playlist_widget;
}

QWidget* GUI_Player::getParentOfLibrary() {
	return library_widget;
}


void GUI_Player::setPlaylist(GUI_Playlist* playlist) {
	ui_playlist = playlist;
    if(ui_playlist) {
        ui_playlist->show();
		ui_playlist->resize(playlist_widget->size());
        QAction* action = createAction( QKeySequence(tr("Ctrl+P")) );
        connect(action, SIGNAL(triggered()), ui_playlist, SLOT(setFocus()));
    }
}


void GUI_Player::setLibrary(GUI_Library_windowed* library) {
    ui_library = library;
    if(ui_library && !ui_libpath) {
        ui_library->show();
		ui_library->resize(library_widget->size());
        QAction* action = createAction(QKeySequence( tr("Ctrl+L")) );
        connect(action, SIGNAL(triggered()), ui_library, SLOT(setFocus()));
    }

    else if(ui_libpath) {
        ui_library->hide();
        ui_libpath->show();
		ui_libpath->resize(library_widget->size());
    }
}

void GUI_Player::setPlayerPluginHandler(PlayerPluginHandler* pph) {
	_pph = pph;

	QList<PlayerPlugin*> lst = _pph->get_all_plugins();
    QList<QAction*> actions;

    foreach(PlayerPlugin* p, lst) {
		QAction* action = p->getAction();
        action->setData(p->getName());
		// action is connected in Plugin itself
        actions << action;
	}


	menuView->insertActions(action_Dark, actions);
	menuView->insertSeparator(action_Dark);

	connect(_pph, SIGNAL(sig_show_plugin(PlayerPlugin*)), this, SLOT(showPlugin(PlayerPlugin*)));
    connect(_pph, SIGNAL(sig_hide_all_plugins()), this, SLOT(hideAllPlugins()));

}

void GUI_Player::stopped() {
    m_metadata_available = false;
	stopClicked(false);
}

void GUI_Player::psl_reload_library_allowed(bool b) {
	action_reloadLibrary->setEnabled(b);
}


/** LIBRARY AND PLAYLIST END **/


// prvt fct
void GUI_Player::setRadioMode(int radio) {

    bool stream_ripper = m_settings->getStreamRipper();
	btn_bw->setEnabled(radio != RADIO_LFM);
	btn_fw->setEnabled(true);

	if(stream_ripper) {

        bool btn_rec_visible = (radio != RADIO_OFF);

        if(btn_rec_visible) {
			btn_play->setVisible(radio == RADIO_OFF);
			btn_rec->setVisible(radio != RADIO_OFF);
        }

        else {
			btn_rec->setVisible(radio != RADIO_OFF);
			btn_play->setVisible(radio == RADIO_OFF);
        }

		btn_play->setEnabled(radio == RADIO_OFF);
	}

	else{

		btn_rec->setVisible(false);
		btn_play->setVisible(true);
		btn_play->setEnabled(radio != RADIO_LFM);
	}

	m_trayIcon->set_enable_play(radio != RADIO_LFM);
	m_trayIcon->set_enable_fwd(true);
	m_trayIcon->set_enable_bwd(radio != RADIO_LFM);

	songProgress->setEnabled( (radio != RADIO_LFM) && (m_metadata.length_ms / 1000) > 0);

	emit sig_rec_button_toggled(btn_rec->isChecked() && btn_rec->isVisible());
}


// public slot
void GUI_Player::psl_strrip_set_active(bool b) {

	if(b) {
		btn_play->setVisible(m_metadata.radio_mode == RADIO_OFF);
		btn_rec->setVisible(m_metadata.radio_mode != RADIO_OFF);
	}

	else{
		btn_play->setVisible(true);
		btn_play->setEnabled(m_metadata.radio_mode == RADIO_OFF);
		btn_rec->setVisible(false);
	}

	emit sig_rec_button_toggled(btn_rec->isChecked() && btn_rec->isVisible());
}


void GUI_Player::ui_loaded() {

    #ifdef Q_OS_UNIX
		obj_ref = this;

        signal(SIGWINCH, signal_handler);

	#endif
    if(ui_libpath)
		ui_libpath->resize(library_widget->size());

    changeSkin(m_settings->getPlayerStyle() == 1);

    bool fullscreen = m_settings->getPlayerFullscreen();
	action_Fullscreen->setChecked(fullscreen);

	ui_playlist->resize(playlist_widget->size());
}


void GUI_Player::notification_changed(bool active, int timeout_ms) {

    m_trayIcon->set_timeout(timeout_ms);
    m_trayIcon->set_notification_active(active);
}


void GUI_Player::moveEvent(QMoveEvent *e) {

    QMainWindow::moveEvent(e);

    QPoint p= this->pos();
    m_settings->setPlayerPos(p);

}


void GUI_Player::resizeEvent(QResizeEvent* e) {

    QMainWindow::resizeEvent(e);

	ui_playlist->resize(playlist_widget->size());

	if(library_widget->isVisible()) {

        if(ui_libpath)
			ui_libpath->resize(library_widget->size());
        else
			ui_library->resize(library_widget->size());
    }

	QSize sz = plugin_widget->size();
    QSize target_size = this->size();

    // maybe we started the player maximized, then showNormal will deliver a strange size
    if(m_settings->getPlayerMaximized() || m_settings->getPlayerFullscreen()) {
        target_size = QSize(1024, 800);
    }

    bool is_maximized = this->isMaximized();
    bool is_fullscreen = this->isFullScreen();
    m_settings->setPlayerMaximized(is_maximized);
    if(is_maximized) {
        m_settings->setPlayerFullscreen(false);
    }

    // maybe we started the player maximized, then showNormal will deliver a strange size
    if(target_size != this->size() && !is_maximized && !is_fullscreen) {
        resize(target_size);
        return;
    }

    _pph->resize(sz);
    m_settings->setPlayerSize(this->size());
    this->update();

}


void GUI_Player::keyPressEvent(QKeyEvent* e) {

    e->accept();

	switch (e->key()) {

    case (Qt::Key_F9):
        if(!isMaximized())
            this->showMaximized();
        else this->showNormal();
            break;

        case (Qt::Key_F10):
			action_Dark->setChecked(!action_Dark->isChecked());
            break;

		case (Qt::Key_F11):
			show_fullscreen_toggled(!this->isFullScreen());
            break;

		default:
			break;
	}
}


void GUI_Player::closeEvent(QCloseEvent* e) {

    if(m_min2tray) {
        e->ignore();
        this->hide();
    }
}


void GUI_Player::really_close(bool b) {

	m_min2tray = false;
	this->close();
}


void GUI_Player::awa_version_finished() {

    QString new_version = *(m_awa_version->get_data());
	QString cur_version = m_settings->getVersion();
	new_version = new_version.trimmed();

	qDebug() << "Newest Version: " << new_version;
	qDebug() << "This Version:   " << cur_version;

	if(new_version > cur_version && m_settings->getNotifyNewVersion()) {
		QMessageBox::information(this, 
					tr("Info"), 
					tr("A new version is available!"));
	}
}

void GUI_Player::awa_translators_finished() {

 QString data = QString::fromUtf8(m_awa_translators->get_data()->toStdString().c_str());
 QStringList translators = data.split('\n');

 m_translators.clear();

 foreach(QString str, translators) {
     if(str.trimmed().size() > 0) {
        m_translators.push_back(str);
     }
 }
}

void GUI_Player::sl_notify_new_version(bool b) {
	m_settings->setNotifyNewVersion(b);
}


void GUI_Player::psl_set_status_bar_text(QString str, bool show) {
	Q_UNUSED(str);
	Q_UNUSED(show);
}
