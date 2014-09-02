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

#include <QDir>
#include <QFileDialog>



/** FILE **/
void GUI_Player::fileSelectedClicked(bool) {

	QStringList filetypes = Helper::get_soundfile_extensions();
    filetypes.append(Helper::get_playlistfile_extensions());
	QString filetypes_str = QString(tr("Media files") + " (");
	foreach(QString filetype, filetypes) {
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

    QString lib_path = m_settings->getLibraryPath();

	if(lib_path.size() == 0 || !QFile::exists(lib_path)) {

        int ret = QMessageBox::warning(this, tr("No library path"), tr("Please select library path first"), QMessageBox::Ok, QMessageBox::Cancel);
        if(ret == QMessageBox::Cancel) return;

        setLibraryPathClicked();
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

    QString lib_path = m_settings->getLibraryPath();

	if(lib_path.size() == 0 || !QFile::exists(lib_path)) {

        int ret = QMessageBox::warning(this, 
                                       tr("No library path"), 
                                       tr("Please select library path first"), 
                                       QMessageBox::Ok, QMessageBox::Cancel);
        if(ret == QMessageBox::Cancel) return;

        setLibraryPathClicked();
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

// prvt slot
void GUI_Player::fetch_all_covers_clicked(bool b) {
}
/** FILE END **/


/** VIEW **/

void GUI_Player::showLibrary(bool b, bool resize) {

    m_settings->setShowLibrary(b);
	int old_width = width();
	int lib_width = library_widget->width();
    int new_width = old_width;
	library_widget->setVisible(b);

    // invisble
	if(!b) {

		QSizePolicy p = library_widget->sizePolicy();
        m_library_stretch_factor = p.horizontalStretch();

        p.setHorizontalStretch(0);
		library_widget->setSizePolicy(p);

        m_library_width = lib_width;
        new_width = old_width - lib_width;
		setMinimumSize(300, 500);
	}

    // visible
	else{
		QSizePolicy p = library_widget->sizePolicy();
		p.setHorizontalStretch(m_library_stretch_factor);
		library_widget->setSizePolicy(p);
        new_width = old_width + m_library_width;
		setMinimumSize(850, 500);
    }

	if(resize) {
		QRect rect = geometry();
        rect.setWidth(new_width);
		rect.setHeight(height());
		setGeometry(rect);
    }
}

void GUI_Player::show_fullscreen_toggled(bool b) {
	// may happend because of F11 too
	action_Fullscreen->setChecked(b);
	if(b)
		showFullScreen();
	else showNormal();

    m_settings->setPlayerFullscreen(b);

}

void GUI_Player::sl_show_only_tracks(bool b) {

    m_settings->setLibShowOnlyTracks(b);

	emit sig_show_only_tracks(b);

}

/** VIEW END **/



/** PREFERENCES **/

// TODO: not ok
// -> base
void GUI_Player::setLibraryPathClicked(bool b) {
	Q_UNUSED(b);

	QString start_dir = QDir::homePath();
    QString old_dir = m_settings->getLibraryPath();

	if (old_dir.size() > 0 && QFile::exists(old_dir)) {
		start_dir = old_dir;
    }

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			old_dir, QFileDialog::ShowDirsOnly);
    if (dir.size() > 0 && (old_dir.compare(dir) != 0)) {
		emit sig_libpath_changed(dir);
        m_settings->setLibraryPath(dir);
		if(ui_libpath && ui_libpath->isVisible()) {
            ui_libpath->hide();
            ui_libpath = 0;
			if(m_settings->getShowLibrary()) {
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
		if(answer == QMessageBox::Yes)
            emit sig_reload_library(true);

		dialog.close();
	}
}

void GUI_Player::psl_libpath_changed(QString & dir) {

    if (dir.size() > 0 && ui_libpath) {

		if(ui_libpath->isVisible() && library_widget->isVisible()) {
            ui_libpath->hide();

            ui_library->show();
			ui_library->resize(library_widget->size());
        }

        ui_libpath = 0;
    }
}


// prvt slot
void GUI_Player::sl_action_socket_connection_triggered(bool b) {
	emit sig_show_socket();
}

// prvt slot
void GUI_Player::load_pl_on_startup_toggled(bool b) {

    m_settings->setLoadPlaylist(b);
}

// prvt slot
void GUI_Player::show_notification_toggled(bool active) {

    m_settings->setShowNotifications(active);
}

// prvt slot
void GUI_Player::min2tray_toggled(bool b) {
    m_settings->setMinimizeToTray(b);
	m_min2tray = b;
}

void GUI_Player::only_one_instance_toggled(bool b) {
    m_settings->setAllowOnlyOneInstance(b);
}

// prvt slot
void GUI_Player::small_playlist_items_toggled(bool b) {
    m_settings->setShowSmallPlaylist(b);
	emit sig_show_small_playlist_items(b);
}

// private slot
void GUI_Player::sl_action_streamripper_toggled(bool b) {

    emit sig_show_stream_rec();
}




// prvt slot
void GUI_Player::lastFMClicked(bool b) {

	Q_UNUSED(b);
	emit sig_setup_LastFM();

}

void GUI_Player::sl_live_search(bool b) {
   m_settings->setLibLiveSearch(b);
}

void GUI_Player::sl_action_language_toggled(bool b) {
    Q_UNUSED(b);
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

    QString version = m_settings->getVersion();
	QString link = Helper::createLink("http://sayonara.luciocarreras.de");

    QMessageBox infobox(this);
    infobox.setParent(this);
    infobox.setIconPixmap(Helper::getPixmap("logo.png", QSize(150, 150), true));
    infobox.setWindowFlags(Qt::Dialog);
    infobox.setModal(true);

    QString first_translators;
    QString last_translator;
    QString translator_str = "";

	if(m_translators.size() > 2) {

		for (int i=0; i<m_translators.size() - 1; i++) {

            first_translators += "<b>" + m_translators[i] + "</b>";
            if(i < m_translators.size() - 2) first_translators += ", ";
        }

        last_translator = QString("<b>") + m_translators[m_translators.size() - 1] + "</b>";
        translator_str = QString("<br /><br /><br />") +
                tr("Special thanks to %1 and %2 for translating")
                .arg(first_translators)
                .arg(last_translator);
    }

    infobox.setWindowTitle(tr("About Sayonara"));
    infobox.setText("<b><font size=\"+2\">Sayonara Player "+ version + "</font></b>");
    infobox.setInformativeText( QString("") +
				tr("Written by Lucio Carreras") + "<br /><br />" +
                tr("License") + ": GPLv3<br /><br />" +
                "Copyright 2011-2013<br /><br />" + link + translator_str
                                );

    infobox.setStandardButtons(QMessageBox::Ok);
    infobox.button(QMessageBox::Ok)->setFocusPolicy(Qt::NoFocus);
    Helper::set_deja_vu_font(&infobox);
    infobox.exec();
}


