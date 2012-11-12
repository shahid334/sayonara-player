/*
 * GUI_SimplePlayerMenubar.cpp
 *
 *  Created on: 10.10.2012
 *      Author: lugmair
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

	QString dir = QFileDialog::getExistingDirectory(this, "Open Directory",
				QDir::homePath(), QFileDialog::ShowDirsOnly);

	if(dir.size() > 0){
		emit importDirectory(dir);
	}
}


void GUI_SimplePlayer::reloadLibraryClicked(bool b) {
	Q_UNUSED(b);
	emit reloadLibrary();
}


// prvt slot
void GUI_SimplePlayer::fetch_all_covers_clicked(bool b) {
	Q_UNUSED(b);
	emit sig_fetch_all_covers();
}
/** FILE END **/


/** VIEW **/

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


/** VIEW END **/



/** PREFERENCES **/

// TODO: not ok
// -> base
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
    infobox.button(QMessageBox::Ok)->setStyleSheet(Style::get_pushbutton_style(m_dark));
    infobox.button(QMessageBox::Ok)->setFocusPolicy(Qt::NoFocus);
    infobox.exec();
}

