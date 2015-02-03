/* GUI_Library_windowed.cpp */

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


/*
 * GUI_Library_windowed.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#include "GUI/library/GUI_Library_windowed.h"
#include <QMessageBox>
#include <QFileDialog>


GUI_Library_windowed::GUI_Library_windowed(LocalLibrary* library, GUI_InfoDialog* info_dialog, QWidget* parent) :
	GUI_AbstractLibrary(library, info_dialog, parent),
	Ui::Library_windowed()
{
	setupUi(this);

	_lib_info_dialog = new GUI_Library_Info_Box(this);

	set_lv_elems(lv_artist, lv_album, tb_title);
	set_search_elems(combo_searchfilter, btn_clear, le_search);
	set_btn_info(btn_info);
	set_btn_refresh(btn_refresh);
	set_lab_status(lab_status);
	init_finished();

    setAcceptDrops(true);

	connect(lv_album, SIGNAL(sig_disc_pressed(int)), this, SLOT(disc_pressed(int)));

	connect(lv_album, SIGNAL(sig_import_files(const QStringList&)), this, SLOT(import_files(const QStringList&)));
	connect(lv_artist, SIGNAL(sig_import_files(const QStringList&)), this, SLOT(import_files(const QStringList&)));
	connect(tb_title, SIGNAL(sig_import_files(const QStringList&)), this, SLOT(import_files(const QStringList&)));
	connect(btn_info, SIGNAL(clicked()), _lib_info_dialog, SLOT(psl_refresh()));

	connect(_library, SIGNAL(sig_no_library_path()), this, SLOT(lib_no_lib_path()));

    hide();
}


GUI_Library_windowed::~GUI_Library_windowed() {
	delete _album_model;
	delete _album_delegate;
	delete _artist_model;
	delete _artist_delegate;
	delete _track_model;
	delete _track_delegate;
	delete _lib_info_dialog;
}

void GUI_Library_windowed::language_changed() {

	retranslateUi(this);

	GUI_AbstractLibrary::language_changed();
}


LocalLibrary::TrackDeletionMode GUI_Library_windowed::show_delete_dialog(int n_tracks) {

		QMessageBox dialog(this);
		QAbstractButton* clicked_button;
		QPushButton* only_library_button;

		dialog.setFocus();
		dialog.setIcon(QMessageBox::Warning);
		dialog.setText("<b>" + tr("Warning") + "!</b>");
		dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		only_library_button = dialog.addButton(tr("Only from library"), QMessageBox::AcceptRole);
		dialog.setDefaultButton(QMessageBox::No);
		QString info_text = tr("You are about to delete %1 files").arg(n_tracks);

		dialog.setInformativeText(info_text + "\n" + tr("Continue?") );

		int answer = dialog.exec();
		clicked_button = dialog.clickedButton();
		dialog.close();


		if(answer == QMessageBox::No){
			return LocalLibrary::TrackDeletionModeNone;
		}

		if(answer == QMessageBox::Yes){
			return LocalLibrary::TrackDeletionModeAlsoFiles;
		}

		if(clicked_button->text() == only_library_button->text()) {
			return LocalLibrary::TrackDeletionModeOnlyLibrary;
		}

		return LocalLibrary::TrackDeletionModeNone;;
}



void GUI_Library_windowed::disc_pressed(int disc) {
	LocalLibrary* ll = (LocalLibrary*) _library;
	ll->psl_disc_pressed(disc);
}




void GUI_Library_windowed::lib_no_lib_path(){

	QMessageBox::warning(this, tr("Warning"), tr("Please select your library path first and reload again."));

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),	QDir::homePath(), QFileDialog::ShowDirsOnly);

	if(dir.size() > 3){
		_settings->set(Set::Lib_Path, dir);
	}
}

void GUI_Library_windowed::import_files(const QStringList & lst) {
    emit sig_import_files(lst);
}

void GUI_Library_windowed::import_result(bool success) {

	QString success_string;
	if(success) {
		success_string = tr("Importing was successful");
	}

	else success_string = tr("Importing failed");

	refresh();
}

