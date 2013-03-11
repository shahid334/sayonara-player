/*
 * GUI_SimplePlayerMenubar.cpp
 *
 *  Created on: 10.10.2012
 *      Author: luke
 */

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

#include "GUI/player/GUI_Simpleplayer.h"
#include "HelperStructs/CSettingsStorage.h"
#include <HelperStructs/Style.h>

#include <QWidget>
#include <QDir>
#include <QStringList>
#include <QFileDialog>


#include <cstdio>
#include <cstdlib>



/** FILE **/


void GUI_SimplePlayer::fileSelectedClicked(bool) {

	QStringList filetypes = Helper::get_soundfile_extensions();
    filetypes.append(Helper::get_playlistfile_extensions());
	QString filetypes_str = QString("Media files (");
	foreach(QString filetype, filetypes){
		filetypes_str += filetype;
		if(filetype != filetypes.last()){
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
		emit fileSelected(list);
}

void GUI_SimplePlayer::folderSelectedClicked(bool) {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			getenv("$HOME"),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir != "")
		emit baseDirSelected(dir);
}


// TODO: not ok
// -> base
void GUI_SimplePlayer::importFolderClicked(bool b){
	Q_UNUSED(b);

    CSettingsStorage* settings = CSettingsStorage::getInstance();
    QString lib_path = settings->getLibraryPath();

    if(lib_path.size() == 0 || !QFile::exists(lib_path)){

        int ret = QMessageBox::warning(this, "No library path", "Please select library path first", QMessageBox::Ok, QMessageBox::Cancel);
        if(ret == QMessageBox::Cancel) return;

        setLibraryPathClicked();
        return;
    }

	QString dir = QFileDialog::getExistingDirectory(this, "Open Directory",
				QDir::homePath(), QFileDialog::ShowDirsOnly);

	if(dir.size() > 0){
		emit importDirectory(dir);
	}
}


void GUI_SimplePlayer::reloadLibraryClicked(bool b) {
	Q_UNUSED(b);
    emit reloadLibrary(false);
}


// prvt slot
void GUI_SimplePlayer::fetch_all_covers_clicked(bool b) {
	Q_UNUSED(b);
	emit sig_fetch_all_covers();
}
/** FILE END **/


/** VIEW **/

void GUI_SimplePlayer::showLibrary(bool b, bool resize){

    CSettingsStorage* settings = CSettingsStorage::getInstance();

    settings->setShowLibrary(b);
    int old_width = this->width();
    int lib_width = this->ui->library_widget->width();
    int new_width = old_width;
    this->ui->library_widget->setVisible(b);

    if(!b){

        QSizePolicy p = this->ui->library_widget->sizePolicy();
        m_library_stretch_factor = p.horizontalStretch();

        p.setHorizontalStretch(0);
        this->ui->library_widget->setSizePolicy(p);

        m_library_width = lib_width;
        new_width = old_width - lib_width;
        this->setMinimumSize(300, 500);
	}

	else{
        QSizePolicy p = this->ui->library_widget->sizePolicy();
		p.setHorizontalStretch(m_library_stretch_factor);
        this->ui->library_widget->setSizePolicy(p);
        new_width = old_width + m_library_width;
        this->setMinimumSize(850, 500);
    }

    if(resize){
        QRect rect = this->geometry();
        rect.setWidth(new_width);
        rect.setHeight(this->height());
        this->setGeometry(rect);
    }
}

void GUI_SimplePlayer::show_fullscreen_toggled(bool b){
	// may happend because of F11 too
	ui->action_Fullscreen->setChecked(b);
	if(b)
		this->showFullScreen();
	else this->showNormal();
}

void GUI_SimplePlayer::sl_show_only_tracks(bool b){
	CSettingsStorage* settings = CSettingsStorage::getInstance();
	settings->setLibShowOnlyTracks(b);

	emit sig_show_only_tracks(b);

}

/** VIEW END **/



/** PREFERENCES **/

// TODO: not ok
// -> base
void GUI_SimplePlayer::setLibraryPathClicked(bool b) {
	Q_UNUSED(b);

	QString start_dir = QDir::homePath();
	QString old_dir = CSettingsStorage::getInstance()->getLibraryPath();

    if (old_dir.size() > 0 && QFile::exists(old_dir)){
		start_dir = old_dir;
    }

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			old_dir, QFileDialog::ShowDirsOnly);
    if (dir.size() > 0 && (old_dir.compare(dir) != 0)) {
		emit libpath_changed(dir);
		CSettingsStorage::getInstance()->setLibraryPath(dir);

        QMessageBox dialog(this);

		dialog.setFocus();
		dialog.setIcon(QMessageBox::Question);
		dialog.setText("<b>Library</b>");
		dialog.setInformativeText("Do you want to reload the Library?");
		dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		dialog.setDefaultButton(QMessageBox::Yes);

		int answer = dialog.exec();
		if(answer == QMessageBox::Yes)
            emit reloadLibrary(true);

		dialog.close();
	}
}


// prvt slot
void GUI_SimplePlayer::sl_action_socket_connection_triggered(bool b){
	emit sig_show_socket();
}

// prvt slot
void GUI_SimplePlayer::load_pl_on_startup_toggled(bool b){

	CSettingsStorage::getInstance()->setLoadPlaylist(b);
}

// prvt slot
void GUI_SimplePlayer::show_notification_toggled(bool active){

	CSettingsStorage::getInstance()->setShowNotifications(active);
}

// prvt slot
void GUI_SimplePlayer::min2tray_toggled(bool b){
	CSettingsStorage::getInstance()->setMinimizeToTray(b);
	m_min2tray = b;
}

void GUI_SimplePlayer::only_one_instance_toggled(bool b){
	CSettingsStorage::getInstance()->setAllowOnlyOneInstance(b);
}

// prvt slot
void GUI_SimplePlayer::small_playlist_items_toggled(bool b){
	CSettingsStorage::getInstance()->setShowSmallPlaylist(b);
	emit show_small_playlist_items(b);
}

// private slot
void GUI_SimplePlayer::sl_action_streamripper_toggled(bool b){

    emit sig_show_stream_rec();
}

// prvt slot
void GUI_SimplePlayer::lastFMClicked(bool b) {

	Q_UNUSED(b);
	emit setupLastFM();

}

/** PREFERENCES END **/



// private slot
void GUI_SimplePlayer::about(bool b){
	Q_UNUSED(b);

	QString version = CSettingsStorage::getInstance()->getVersion();

    QMessageBox infobox;
    infobox.setParent(this);
    QPixmap p = QPixmap(Helper::getIconPath() + "logo.png").scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    infobox.setIconPixmap(p);
    infobox.setWindowFlags(Qt::Dialog);
    infobox.setModal(true);

    infobox.setWindowTitle("About Sayonara");
    infobox.setText("<b><font size=\"+2\">Sayonara Player "+ version + "</font></b>");
    infobox.setInformativeText("Written by Lucio Carreras<br /><br />License: GPL<br /><br />Copyright 2011-2012");
    infobox.setStandardButtons(QMessageBox::Ok);
    infobox.button(QMessageBox::Ok)->setFocusPolicy(Qt::NoFocus);
    infobox.exec();
}


