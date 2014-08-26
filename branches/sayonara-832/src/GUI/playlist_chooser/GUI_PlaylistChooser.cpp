/* GUI_PlaylistChooser.cpp */

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


#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"
#include "GUI/TargetPlaylistDialog/GUI_Target_Playlist_Dialog.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"

#include <QWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>

#include "GUI/ui_GUI_PlaylistChooser.h"


GUI_PlaylistChooser::GUI_PlaylistChooser(QString name, QWidget *parent) :
	PlayerPlugin(name, parent),
	Ui::GUI_PlaylistChooser()
{

	setupUi(this);

	_cur_idx = -1;
    _dark = false;
    _text_before_save = "";

    QPixmap p = QPixmap(Helper::getIconPath() + "lyrics.png").scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	lab_icon->setPixmap(p);

    _target_playlist_dialog = new GUI_Target_Playlist_Dialog(this);
    _last_dir = CSettingsStorage::getInstance()->getLibraryPath();

	btn_save->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	btn_save_as->setIcon(QIcon(Helper::getIconPath() + "save_as.png"));
	btn_delete->setIcon(QIcon(Helper::getIconPath() + "delete.png"));


	btn_delete->setEnabled(false);
	btn_save->setEnabled(false);
	btn_save_as->setEnabled(false);

	connect(btn_save, SIGNAL(clicked()), this, SLOT(save_button_pressed()));
	connect(btn_save_as, SIGNAL(clicked()), this, SLOT(save_as_button_pressed()));
	connect(btn_delete, SIGNAL(clicked()), this, SLOT(delete_button_pressed()));
	connect(btn_load, SIGNAL(clicked()), this, SLOT(load_button_pressed()));
	connect(combo_playlistchooser, SIGNAL(activated(int)), this, SLOT(playlist_selected(int)));
	connect(combo_playlistchooser, SIGNAL(editTextChanged( const QString & )), this, SLOT(text_changed ( const QString & )));
    connect(_target_playlist_dialog, SIGNAL(sig_target_chosen(QString,bool)), this, SLOT(got_save_params(QString,bool)));

    hide();

}

GUI_PlaylistChooser::~GUI_PlaylistChooser() {

}



void GUI_PlaylistChooser::language_changed() {
	retranslateUi(this);
}

void GUI_PlaylistChooser::changeSkin(bool dark) {
    _dark = dark;
}


void GUI_PlaylistChooser::all_playlists_fetched(QMap<int, QString>& mapping) {
    int tmp_cur_idx = _cur_idx;
	combo_playlistchooser->clear();
	combo_playlistchooser->addItem("", -1);

	QList<int> keys = mapping.keys();
	foreach(int key, keys) {
		QString name = mapping.value(key);
		combo_playlistchooser->addItem(name, key);
	}

    _cur_idx = tmp_cur_idx;

	if(_cur_idx < combo_playlistchooser->count() && _cur_idx >= 0)
		combo_playlistchooser->setCurrentIndex(_cur_idx);

    if(_text_before_save.size() > 0) {
		int idx = combo_playlistchooser->findText(_text_before_save);
		if(idx > 0 && idx < combo_playlistchooser->count()) {
			combo_playlistchooser->setCurrentIndex(idx);
            _cur_idx = idx;
        }
    }

	text_changed(combo_playlistchooser->currentText());
}

// Playlist -> this
void GUI_PlaylistChooser::playlist_changed(const MetaDataList& v_md, int i, PlaylistType playlist_type) {

    Q_UNUSED(i);
    bool empty = (v_md.size() == 0);

	btn_save->setEnabled(!empty && playlist_type == PlaylistTypeStd);
	btn_save_as->setEnabled(!empty && playlist_type == PlaylistTypeStd);

    if(empty)
		le_playlist_file->clear();

	text_changed(combo_playlistchooser->currentText());
}



void GUI_PlaylistChooser::save_button_pressed() {

	if(_cur_idx >= combo_playlistchooser->count()) return;

	QString cur_text = combo_playlistchooser->currentText();
    _text_before_save = cur_text;

    QStringList lst;
	for(int i=0; i<combo_playlistchooser->count(); i++) {
		QString txt = combo_playlistchooser->itemText(i);
        if(txt.size() > 0)
            lst << txt.toLower();
    }

    if( lst.contains(cur_text.toLower()) ) {

		int val = combo_playlistchooser->itemData(_cur_idx).toInt();
        int answer = show_warning(tr("Overwrite?"));

		if(answer == QMessageBox::Yes)
            emit sig_save_playlist(val);
	}

    else if(cur_text.size() > 0 ) {
        emit sig_save_playlist(cur_text);
    }

	else return;
}

void GUI_PlaylistChooser::got_save_params(const QString& filename, bool relative) {
    emit sig_save_playlist_file(filename, relative);

}

void GUI_PlaylistChooser::save_as_button_pressed() {
    _target_playlist_dialog->change_skin(_dark);
    _target_playlist_dialog->show();
}


void GUI_PlaylistChooser::delete_button_pressed() {

    _text_before_save = "";
    int answer = show_warning(tr("Delete?"));

	if(_cur_idx < combo_playlistchooser->count() && _cur_idx != -1) {
		int val = combo_playlistchooser->itemData(_cur_idx).toInt();
		if(val >= 0 && answer == QMessageBox::Yes)
			emit sig_delete_playlist(val);
	}
}


void GUI_PlaylistChooser::playlist_selected(int idx) {

	_cur_idx = idx;
	if(_cur_idx >= combo_playlistchooser->count() || _cur_idx < 0) return;

	int val = combo_playlistchooser->itemData(idx).toInt();
    bool val_bigger_zero = (val > 0);
	btn_delete->setEnabled(val_bigger_zero);
	text_changed(combo_playlistchooser->currentText());


    emit sig_playlist_chosen(val);

	le_playlist_file->clear();
}


void GUI_PlaylistChooser::load_button_pressed() {

    QStringList filelist = QFileDialog::getOpenFileNames(
                    this,
                    tr("Open Playlist files"),
                    _last_dir,
                    Helper::get_playlistfile_extensions().join(" "));

    QString lab_text = "";
    foreach(QString filename, filelist) {
        int last_index_of_sep = filename.lastIndexOf(QDir::separator());
        _last_dir = filename.left(last_index_of_sep);

        QString trimmed_filename = filename.right(filename.size() - last_index_of_sep - 1);
        trimmed_filename = trimmed_filename.left(trimmed_filename.lastIndexOf('.'));
        lab_text += trimmed_filename + ", ";
    }

    if(lab_text.size() > 2)
        lab_text = lab_text.left(lab_text.size() - 2);

	le_playlist_file->setText(lab_text);

    if(filelist.size() > 0) {
        emit sig_files_selected(filelist);
    }
}


void GUI_PlaylistChooser::text_changed(const QString & text) {

	btn_save->setEnabled(text.size() > 0);

    QStringList lst;
	for(int i=0; i<combo_playlistchooser->count(); i++) {
		QString txt = combo_playlistchooser->itemText(i);
        if(txt.size() > 0)
            lst << txt.toLower();
    }

	btn_delete->setEnabled(lst.contains(text.toLower()));
}


int GUI_PlaylistChooser::show_warning(QString title_text) {

    QMessageBox warning_box(this);
        warning_box.setParent(this);
        warning_box.setModal(true);
        warning_box.setWindowFlags(Qt::Dialog);
        warning_box.setIcon(QMessageBox::Warning);
        warning_box.setStandardButtons( QMessageBox::Yes | QMessageBox::No);
        warning_box.setText(QString("<b>") + title_text + "</b>");
        warning_box.setInformativeText(tr("Are you sure?"));
        warning_box.setWindowTitle(title_text);
        warning_box.setDefaultButton(QMessageBox::No);
        Helper::set_deja_vu_font(&warning_box);


    return warning_box.exec();
}
