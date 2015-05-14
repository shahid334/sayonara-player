/*
 * GUI_PlayerMenubar.cpp
 *
 *  Created on: 10.10.2012
 *      Author: luke
 */

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

#include "GUI/player/GUI_Player.h"
#include "CoverLookup/CoverLookupAll.h"
#include "GUI/StreamRecorder/GUI_StreamRecorder.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>


/** FILE **/
void GUI_Player::fileSelectedClicked(bool) {

	QStringList filetypes = Helper::get_soundfile_extensions();
    filetypes.append(Helper::get_playlistfile_extensions());
	QString filetypes_str = QString(tr("Media files") + " (");
	for(const QString& filetype : filetypes) {
		filetypes_str += filetype;
		if(filetype != filetypes.last()) {
			filetypes_str += " ";
		}
	}

	filetypes_str += ")";



	QStringList list =
			QFileDialog::getOpenFileNames(
					this,
					tr("Open Media files"),
					QDir::homePath(),
					filetypes_str);

	if (list.size() > 0)
		emit sig_file_selected(list);
}

void GUI_Player::folderSelectedClicked(bool) {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			getenv("$HOME"),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir != "")
		emit sig_basedir_selected(dir);
}


// TODO: not ok
// -> base
void GUI_Player::importFolderClicked(bool b) {
	Q_UNUSED(b);

	QString lib_path = _settings->get(Set::Lib_Path);

	if(lib_path.size() == 0 || !QFile::exists(lib_path)) {

        int ret = QMessageBox::warning(this, tr("No library path"), tr("Please select library path first"), QMessageBox::Ok, QMessageBox::Cancel);
        if(ret == QMessageBox::Cancel) return;

		sl_libpath_clicked();
        return;
    }

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
				QDir::homePath(), QFileDialog::ShowDirsOnly);

	if(dir.size() > 0) {
		emit sig_import_dir(dir);
	}
}

void GUI_Player::importFilesClicked( bool b ) {
   Q_UNUSED(b);

	QString lib_path = _settings->get(Set::Lib_Path);

	if(lib_path.size() == 0 || !QFile::exists(lib_path)) {

        int ret = QMessageBox::warning(this, 
                                       tr("No library path"), 
                                       tr("Please select library path first"), 
                                       QMessageBox::Ok, QMessageBox::Cancel);
        if(ret == QMessageBox::Cancel) return;

		sl_libpath_clicked();
        return;
    }

    QStringList extensions = Helper::get_soundfile_extensions();
    QString filter = QString("Soundfiles (") + extensions.join(" ") + ")";
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Import Files"),
					QDir::homePath(), filter);

    if(files.size() > 0) emit sig_import_files(files);

}


void GUI_Player::reloadLibraryClicked(bool b) {
	Q_UNUSED(b);
    emit sig_reload_library(false);
}


/** FILE END **/


/** VIEW **/

void GUI_Player::showLibrary(bool b, bool resize) {

	_settings->set(Set::Lib_Show, b);
	int old_width = width();
	int lib_width = library_widget->width();
    int new_width = old_width;
	library_widget->setVisible(b);

    // invisble
	if(!b) {

		QSizePolicy p = library_widget->sizePolicy();
		_library_stretch_factor = p.horizontalStretch();

        p.setHorizontalStretch(0);
		library_widget->setSizePolicy(p);

		_ui_library_width = lib_width;
        new_width = old_width - lib_width;
		setMinimumSize(300, 500);
	}

    // visible
	else{
		QSizePolicy p = library_widget->sizePolicy();
		p.setHorizontalStretch(_library_stretch_factor);
		library_widget->setSizePolicy(p);
		new_width = old_width + _ui_library_width;
		setMinimumSize(850, 500);
    }

	if(resize) {
		QRect rect = geometry();
        rect.setWidth(new_width);
		rect.setHeight(height());
		setGeometry(rect);
    }
}

void GUI_Player::_sl_fullscreen_toggled(){
	show_fullscreen_toggled(_settings->get(Set::Player_Fullscreen));
}

void GUI_Player::show_fullscreen_toggled(bool b) {
	// may happend because of F11 too
	action_Fullscreen->setChecked(b);
	if(b)
		showFullScreen();
	else showNormal();

	_settings->set(Set::Player_Fullscreen, b);
}

void GUI_Player::sl_show_only_tracks(bool b) {

	_settings->set(Set::Lib_OnlyTracks, b);
}

/** VIEW END **/



/** PREFERENCES **/

// TODO: not ok
// -> base
void GUI_Player::sl_libpath_clicked() {

	QString start_dir = QDir::homePath();
	QString old_dir = _settings->get(Set::Lib_Path);

	if (old_dir.size() > 0 && QFile::exists(old_dir)) {
		start_dir = old_dir;
    }

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			old_dir, QFileDialog::ShowDirsOnly);

    if (dir.size() > 0 && (old_dir.compare(dir) != 0)) {

		_settings->set(Set::Lib_Path, dir);

		if( ui_libpath->isVisible() ) {

			ui_libpath->hide();

			bool show_library = _settings->get(Set::Lib_Show);

			if(show_library) {
                ui_library->show();
				ui_library->resize(library_widget->size());
            }
        }

        QMessageBox dialog(this);

		dialog.setFocus();
		dialog.setIcon(QMessageBox::Question);
		dialog.setText("<b>" + tr("Library") + "</b>");
		dialog.setInformativeText(tr("Do you want to reload the Library?"));
		dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		dialog.setDefaultButton(QMessageBox::Yes);

		int answer = dialog.exec();
		if(answer == QMessageBox::Yes){
			emit sig_reload_library(true);
		}

		dialog.close();
	}
}

void GUI_Player::_sl_libpath_changed() {

	QString dir = _settings->get(Set::Lib_Path);

	if (QFile::exists(dir)) {

		if( library_widget->isVisible() && ui_library ){
            ui_libpath->hide();
            ui_library->show();
			ui_library->resize(library_widget->size());
        }
    }

	else{

		if( library_widget->isVisible() && ui_library ){
			ui_library->hide();
			ui_libpath->show();
			ui_libpath->resize(library_widget->size());
		}
	}
}


// prvt slot
void GUI_Player::sl_action_socket_connection_triggered(bool b) {
	emit sig_show_socket();
}

// prvt slot
void GUI_Player::load_pl_on_startup_toggled(bool b) {

	_settings->set(Set::PL_Load, b);
}

// prvt slot
void GUI_Player::show_notification_toggled(bool active) {

	_settings->set(Set::Notification_Show, active);
}

// prvt slot
void GUI_Player::min2tray_toggled(bool b) {
	_settings->set(Set::Player_Min2Tray, b);
}

void GUI_Player::only_one_instance_toggled(bool b) {
	_settings->set(Set::Player_OneInstance, b);
}

// prvt slot
void GUI_Player::small_playlist_items_toggled(bool b) {
	_settings->set(Set::PL_SmallItems, b);
}

// private slot
void GUI_Player::sl_streamrecorder_toggled() {
	ui_streamrecorder->show();
}

void GUI_Player::sl_broadcasting_clicked(){
	ui_broadcasting->show();
}




// prvt slot
void GUI_Player::lastFMClicked() {

	emit sig_setup_LastFM();
}

void GUI_Player::sl_live_search(bool b) {
	_settings->set(Set::Lib_LiveSearch, b);
}

void GUI_Player::sl_action_language_toggled() {
    ui_language_chooser->show();
}

/** PREFERENCES END **/

void GUI_Player::help(bool b) {
	Q_UNUSED(b);
	QString link = Helper::createLink("http://sayonara.luciocarreras.de/Forum/xmb");
	
	QMessageBox::information(this, tr("Help"), tr("Please visit the forum at") + "<br />" + link);

}

// private slot
void GUI_Player::about(bool b) {
	Q_UNUSED(b);

	QString version = _settings->get(Set::Player_Version);
	QString revision;
#ifdef SAYONARA_REVISION
	revision = SAYONARA_REVISION;
	version += "-" + revision;
#endif

	QString link = Helper::createLink("http://sayonara.luciocarreras.de");

	QMessageBox infobox(this);
    infobox.setParent(this);
	infobox.setIconPixmap(Helper::getPixmap("logo.png", QSize(150, 150), true));
    infobox.setWindowFlags(Qt::Dialog);
    infobox.setModal(true);

    QString first_translators;
    QString last_translator;
    QString translator_str = "";

	if(_translators.size() > 2) {

		for (int i=0; i<_translators.size() - 1; i++) {

			first_translators += "<b>" + _translators[i] + "</b>";
			if(i < _translators.size() - 2) first_translators += ", ";
        }

		last_translator = QString("<b>") + _translators[_translators.size() - 1] + "</b>";
        translator_str = QString("<br /><br /><br />") +
                tr("Special thanks to %1 and %2 for translating")
                .arg(first_translators)
                .arg(last_translator);
    }

    infobox.setWindowTitle(tr("About Sayonara"));
    infobox.setText("<b><font size=\"+2\">Sayonara Player " + version + "</font></b>");
    infobox.setInformativeText( QString("") +
				tr("Written by Lucio Carreras") + "<br /><br />" +
                tr("License") + ": GPLv3<br /><br />" +
                "Copyright 2011-2015<br /><br />" + link + translator_str
                                );

    infobox.setStandardButtons(QMessageBox::Ok);

    infobox.exec();
}


