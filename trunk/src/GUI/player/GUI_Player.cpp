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
#include "GUI/AlternativeCovers/GUI_AlternativeCovers.h"

#include "HelperStructs/globals.h"
#include "HelperStructs/Style/Style.h"
#include "HelperStructs/WebAccess/AsyncWebAccess.h"
#include "CoverLookup/CoverLookup.h"

#include "StreamPlugins/LastFM/LastFM.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QPalette>

GUI_Player* obj_ref = 0;

#ifdef Q_OS_UNIX
#include <csignal>

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
	SayonaraMainWindow(parent),
	Ui::Sayonara() {

	setupUi(this);

	initGUI();

	_translator = translator;
	_play_manager = PlayManager::getInstance();

	QString version = _settings->get(Set::Player_Version);

	_awa_version = new AsyncWebAccess(this);
	_awa_translators = new AsyncWebAccess(this);

	lab_sayonara->setText(tr("Sayonara Player"));
	lab_version->setText( version );
	lab_writtenby->setText(tr("Written by") + " Lucio Carreras");
	lab_copyright->setText(tr("Copyright") + " 2011-2014");

	_md_available = false;

	_mute = false;

	ui_playlist = 0;

	ui_streamrecorder = new GUI_StreamRecorder(this);
	ui_broadcasting = new GUI_BroadcastSetup(this);
    ui_notifications = new GUI_Notifications(this);
    ui_startup_dialog = new GUI_Startup_Dialog(this);
    ui_language_chooser= new GUI_LanguageChooser(this);

	_skin_suffix = "";

	_cov_lookup = new CoverLookup(this);
	_ui_alternative_covers = new GUI_AlternativeCovers(this->centralWidget());

	action_min2tray->setChecked(_settings->get(Set::Player_Min2Tray));

	bool one_instance = _settings->get(Set::Player_OneInstance);
	action_only_one_instance->setChecked(one_instance);

	bool show_small_items = _settings->get(Set::PL_SmallItems);
	action_smallPlaylistItems->setChecked(show_small_items);

	bool show_only_tracks = _settings->get(Set::Lib_OnlyTracks);
	action_showOnlyTracks->setChecked(show_only_tracks);

	QSizePolicy p = library_widget->sizePolicy();
	_library_stretch_factor = p.horizontalStretch();

	bool show_library = _settings->get(Set::Lib_Show);
	action_viewLibrary->setChecked(show_library);
    this->showLibrary(show_library);

	bool live_search = _settings->get(Set::Lib_LiveSearch);
	action_livesearch->setChecked(live_search);

	bool notify_new_version = _settings->get(Set::Player_NotifyNewVersion);
	action_notifyNewVersion->setChecked(notify_new_version);

	bool is_fullscreen = _settings->get(Set::Player_Fullscreen);
	bool is_maximized = _settings->get(Set::Player_Maximized);

	int volume = _settings->get(Set::Engine_Vol);
	volumeChanged(volume);

	action_Dark->setChecked(_settings->get(Set::Player_Style) == 1);
	skin_changed();

    if(!is_fullscreen & !is_maximized) {

		QSize size = _settings->get(Set::Player_Size);
		QPoint pos = _settings->get(Set::Player_Pos);
        QRect rect = this->geometry();

        rect.setX(pos.x());
        rect.setY(pos.y());
        rect.setWidth(size.width());
        rect.setHeight(size.height());
        this->setGeometry(rect);
    }

	_ui_library_width = 600;

	ui_libpath = new GUI_LibraryPath( library_widget );

	/* TRAY ACTIONS */
	this->setupTrayActions();

	/* SIGNALS AND SLOTS */
    this->setupConnections();
	_awa_version->set_url("http://sayonara.luciocarreras.de/current_version");
	_awa_translators->set_url("http://sayonara.luciocarreras.de/translators");

	if(_settings->get(Set::Player_NotifyNewVersion)){
		_awa_version->start();
	}

	_awa_translators->start();

	plugin_widget->resize(plugin_widget->width(), 0);

	stopped();

	REGISTER_LISTENER(Set::Lib_Path, _sl_libpath_changed);
	REGISTER_LISTENER(Set::Engine_SR_Active, _sl_sr_active_changed);
	REGISTER_LISTENER_NO_CALL(Set::Player_Fullscreen, _sl_fullscreen_toggled);
	REGISTER_LISTENER_NO_CALL(SetNoDB::Player_Quit, really_close);
}


GUI_Player::~GUI_Player() {
	qDebug() << "closing player...";
}


void GUI_Player::language_changed() {

	QString language = _settings->get(Set::Player_Language);
	_translator->load(language, Helper::getSharePath() + "translations/");

	retranslateUi(this);

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

	btn_mute->setIcon(Helper::getIcon("vol_1"));

	action_viewLibrary->setText(tr("&Library"));
	btn_rec->setVisible(false);

	action_Fullscreen->setShortcut(QKeySequence("F11"));
	action_Dark->setShortcut(QKeySequence("F10"));

	btn_correct->setVisible(false);
}


// new track
void GUI_Player::track_changed(const MetaData & md) {

	_md = md;
	_md_available = true;

	lab_sayonara->hide();
	lab_title->show();

	lab_version->hide();
	lab_artist->show();

	lab_writtenby->hide();
	lab_album->show();

	lab_copyright->hide();
	lab_rating->show();

	set_album_label();

	lab_artist->setText(Helper::elide_text(_md.artist, lab_artist, 1));
	lab_title->setText(Helper::elide_text(_md.title, lab_title, 2));

	total_time_changed(_md.length_ms);

	QString rating_text;

	rating_text = QString::number(_md.bitrate / 1000) + " kBit/s";
	if(_md.filesize > 0){
		rating_text += ", " + QString::number( (double) (_md.filesize / 1024) / 1024.0, 'f', 2) + " MB";
	}

	lab_rating->setText(rating_text);
	lab_rating->setToolTip(rating_text);

	btn_correct->setVisible(false);
	setRadioMode( _md.radio_mode() );

	this->setWindowTitle(QString("Sayonara - ") + md.title);
	this->repaint();

	fetch_cover();

	_tray_icon->show_notification(_md);
}

void GUI_Player::set_album_label(){

	QString str_year = QString::number(_md.year);
	QString album_name = Helper::get_album_w_disc(_md);

	if(_md.year > 1000 && !_md.album.contains(str_year)){
		album_name += " (" + str_year + ")";
	}

	lab_album->setText(Helper::elide_text(album_name,lab_album, 1));
}


void GUI_Player::psl_md_changed(const MetaData& md) {

	if(_md != md && md.title == _md.title){
		return;
	}

	_md = md;
	QString rating_text;

	rating_text = QString::number(_md.bitrate / 1000) + " kBit/s";
	if(_md.filesize > 0){
		rating_text += ", " + QString::number( (double) (_md.filesize / 1024) / 1024.0, 'f', 2) + " MB";
	}

	_md.title = md.title;

	lab_rating->setText(rating_text);
	lab_rating->setToolTip(rating_text);

	total_time_changed(_md.length_ms);
	lab_title->setText( md.title );
}


// public slot:
// id3 tags have changed
void GUI_Player::psl_id3_tags_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new) {

	QList<int> idxs = v_md_old.findTracks(_md.filepath());
	if(idxs.isEmpty()) return;

	int idx = idxs[0];

	_md = v_md_new[idx];

	set_album_label();
	lab_artist->setText(_md.artist);
	lab_title->setText(_md.title);
	btn_correct->setVisible(false);

	_tray_icon->show_notification(_md);

	setWindowTitle(QString("Sayonara - ") + _md.title);
	fetch_cover();
}


/** LAST FM **/
void GUI_Player::last_fm_logged_in(bool b) {

	bool active = _settings->get(Set::LFM_Active);

	if(!b && active){
		QMessageBox::warning(centralwidget, tr("Warning"), tr("Cannot login to Last.fm"));
	}
}


void GUI_Player::lfm_info_fetched(const MetaData& md, bool loved, bool corrected) {

	_md_corrected = md;

	bool radio_off = ( _md.radio_mode() == RadioModeOff );
	bool get_lfm_corrections = _settings->get(Set::LFM_Corrections);

	btn_correct->setVisible(corrected &&
							radio_off &&
							get_lfm_corrections);

	if(loved) {
		lab_title->setText(lab_title->text());
	}

	this->repaint();
}

void GUI_Player::correct_btn_clicked(bool b) {

	MetaData md = _md_corrected;
	_md_corrected = _md;

	bool same_artist = (_md.artist.compare(md.artist) == 0);
	bool same_album = (_md.album.compare(md.album) == 0);
	bool same_title = (_md.title.compare(md.title) == 0);

    if(!same_artist) {
		_md_corrected.artist = md.artist;
		_md_corrected.artist_id = -1;
    }

    if(!same_album) {
		_md_corrected.album = md.album;
		_md_corrected.album_id = -1;
    }

    if(!same_title) {
		_md_corrected.title = md.title;
    }

	MetaDataList lst;
	lst.push_back(_md_corrected);
}
/** LAST FM **/

void GUI_Player::skin_toggled(bool on){
	_settings->set(Set::Player_Style, (on ? 1 : 0) );
}

void GUI_Player::skin_changed() {

	bool dark = (_settings->get(Set::Player_Style) == 1);
    QString stylesheet = Style::get_style(dark);

	this->setStyleSheet(stylesheet);

	if (dark) 	_skin_suffix = QString("_dark");
	else 		_skin_suffix = QString("");

	setupVolButton(volumeSlider->value());
}



/** TRAY ICON **/
void GUI_Player::setupTrayActions() {


	_tray_icon = new GUI_TrayIcon(this);


	connect(_tray_icon, SIGNAL(sig_stop_clicked()), this, SLOT(stop_clicked()));
	connect(_tray_icon, SIGNAL(sig_bwd_clicked()), this, SLOT(prev_clicked()));
	connect(_tray_icon, SIGNAL(sig_fwd_clicked()), this, SLOT(next_clicked()));
	connect(_tray_icon, SIGNAL(sig_mute_clicked()), this, SLOT(muteButtonPressed()));
	connect(_tray_icon, SIGNAL(sig_close_clicked()), this, SLOT(really_close()));
	connect(_tray_icon, SIGNAL(sig_play_clicked()), this, SLOT(play_clicked()));
	connect(_tray_icon, SIGNAL(sig_pause_clicked()), this, SLOT(play_clicked()));
	connect(_tray_icon, SIGNAL(sig_show_clicked()), this, SLOT(showNormal()));

	connect(_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
      		this, 		SLOT(trayItemActivated(QSystemTrayIcon::ActivationReason)));

	connect(_tray_icon, SIGNAL(onVolumeChangedByWheel(int)),
   			this, 		SLOT(volumeChangedByTick(int)));


	_tray_icon->setPlaying(false);

	_tray_icon->show();

}


void GUI_Player::trayItemActivated (QSystemTrayIcon::ActivationReason reason) {

	bool min_to_tray = _settings->get(Set::Player_Min2Tray);
    switch (reason) {

    case QSystemTrayIcon::Trigger:

        if (this->isMinimized() || isHidden()) {
            this->setHidden(false);
            this->showNormal();
            this->activateWindow();
        }

		else if( min_to_tray ) {
            this->setHidden(true);
        }

        else this->showMinimized();


        break;
    case QSystemTrayIcon::MiddleClick:
		_tray_icon->show_notification(_md);
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

	if(!playlist) return;

	ui_playlist = playlist;

	QAction* action = createAction( QKeySequence(tr("Ctrl+P")) );
	connect(action, SIGNAL(triggered()), ui_playlist, SLOT(setFocus()));
}


void GUI_Player::setLibrary(GUI_AbstractLibrary* library) {

	if(!library) return;

	ui_library = library;

	QAction* action = createAction(QKeySequence( tr("Ctrl+L")) );
	connect(action, SIGNAL(triggered()), ui_library, SLOT(setFocus()));
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

void GUI_Player::psl_reload_library_allowed(bool b) {
	action_reloadLibrary->setEnabled(b);
}


/** LIBRARY AND PLAYLIST END **/


// prvt fct
void GUI_Player::setRadioMode(int radio) {

	bool stream_ripper = _settings->get(Set::Engine_SR_Active);

	if(stream_ripper) {

		bool btn_rec_visible = (radio != RadioModeOff);

        if(btn_rec_visible) {
			btn_play->setVisible(radio == RadioModeOff);
			btn_rec->setVisible(radio != RadioModeOff);
        }

        else {
			btn_rec->setVisible(radio != RadioModeOff);
			btn_play->setVisible(radio == RadioModeOff);
        }

		btn_play->setEnabled(radio == RadioModeOff);
	}

	else{

		btn_rec->setVisible(false);
		btn_play->setVisible(true);
	}

	_tray_icon->set_enable_fwd(true);
	songProgress->setEnabled( (_md.length_ms / 1000) > 0 );

    _play_manager->record(btn_rec->isChecked() && btn_rec->isVisible());

}


// public slot
void GUI_Player::_sl_sr_active_changed() {

	bool active = _settings->get(Set::Engine_SR_Active);

	if(active) {
		btn_play->setVisible(_md.radio_mode() == RadioModeOff);
		btn_rec->setVisible(_md.radio_mode() != RadioModeOff);
	}

	else{
		btn_play->setVisible(true);
		btn_play->setEnabled(_md.radio_mode() == RadioModeOff);
		btn_rec->setVisible(false);
	}

	btn_rec->setChecked(false);
}


void GUI_Player::ui_loaded() {


    #ifdef Q_OS_UNIX
		obj_ref = this;
        signal(SIGWINCH, signal_handler);
	#endif

	QString lib_path = _settings->get(Set::Lib_Path);
	if(ui_library){
		if(QFile::exists(lib_path)){
			ui_libpath->hide();
			ui_library->show();
		}

		else{
			ui_library->hide();
			ui_libpath->show();
		}
	}

	bool fullscreen = _settings->get(Set::Player_Fullscreen);
	action_Fullscreen->setChecked(fullscreen);

	if(ui_playlist){
		ui_playlist->resize(playlist_widget->size());
	}

	QString shown_plugin = _settings->get(Set::Player_ShownPlugin);
	PlayerPlugin* p  = _pph->find_plugin(shown_plugin);
	showPlugin(p);

}


void GUI_Player::moveEvent(QMoveEvent *e) {

    QMainWindow::moveEvent(e);

    QPoint p= this->pos();
	_settings->set(Set::Player_Pos, p);
}


void GUI_Player::resizeEvent(QResizeEvent* e) {

    QMainWindow::resizeEvent(e);

	ui_playlist->resize(playlist_widget->size());

	if(library_widget->isVisible()) {

		if( ui_libpath->isVisible() ){
			ui_libpath->resize(library_widget->size());
		}

		else{
			ui_library->resize(library_widget->size());
		}
    }

	QSize sz = plugin_widget->size();
    QSize target_size = this->size();

	bool maximized = _settings->get(Set::Player_Maximized);
	bool fullscreen = _settings->get(Set::Player_Fullscreen);

    // maybe we started the player maximized, then showNormal will deliver a strange size
	if( maximized || fullscreen ) {
        target_size = QSize(1024, 800);
    }

    bool is_maximized = this->isMaximized();
    bool is_fullscreen = this->isFullScreen();

	_settings->set(Set::Player_Maximized, is_maximized);
    if(is_maximized) {
		_settings->set(Set::Player_Fullscreen, false);
    }

    // maybe we started the player maximized, then showNormal will deliver a strange size
    if(target_size != this->size() && !is_maximized && !is_fullscreen) {
        resize(target_size);
        return;
    }

	set_album_label();
	lab_artist->setText(Helper::elide_text(_md.artist, lab_artist, 1));
	lab_title->setText(Helper::elide_text(_md.title, lab_title, 2));

    _pph->resize(sz);

	_settings->set(Set::Player_Size, this->size());

	update();
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

void GUI_Player::showEvent(QShowEvent *e){

	Q_UNUSED(e);

	bool maximized = _settings->get(Set::Player_Maximized);

	if(maximized){
		showMaximized();
	}

	else{
		showNormal();
	}

	ui_library->resize(library_widget->size());
	ui_playlist->resize(playlist_widget->size());
}

void GUI_Player::closeEvent(QCloseEvent* e) {

	bool min_to_tray = _settings->get(Set::Player_Min2Tray);

	if(min_to_tray) {
        e->ignore();
        this->hide();
    }
}

void GUI_Player::really_close(bool) {

	_settings->set(Set::Player_Min2Tray, false);
	this->close();
}


void GUI_Player::awa_version_finished() {


	QString new_version = *(_awa_version->get_data());
	QString cur_version = _settings->get(Set::Player_Version);
	bool notify_new_version = _settings->get(Set::Player_NotifyNewVersion);
	bool dark = (_settings->get(Set::Player_Style) == 1);

	new_version = new_version.trimmed();

	qDebug() << "Newest Version: " << new_version;
	qDebug() << "This Version:   " << cur_version;

	QString link;
	LINK("http://sayonara.luciocarreras.de", "http://sayonara.luciocarreras.de", dark, link);

	if(new_version > cur_version && notify_new_version) {
		QMessageBox::information(this, 
					tr("Info"), 
					tr("A new version is available!") + "<br />" +  link);
	}
}

void GUI_Player::awa_translators_finished() {

 QString data = QString::fromUtf8(_awa_translators->get_data()->toStdString().c_str());
 QStringList translators = data.split('\n');

 _translators.clear();

	foreach(QString str, translators) {
		if(str.trimmed().size() > 0) {
			_translators.push_back(str);
		}
	}
}

void GUI_Player::sl_notify_new_version(bool b) {
	_settings->set(Set::Player_NotifyNewVersion, b);
}


void GUI_Player::psl_set_status_bar_text(QString str, bool show) {
	Q_UNUSED(str);
	Q_UNUSED(show);
}
