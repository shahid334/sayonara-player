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


#include "HelperStructs/globals.h"
#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"
#include "GUI/TargetPlaylistDialog/GUI_Target_Playlist_Dialog.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "HelperStructs/DirectoryReader/DirectoryReader.h"



#include <QInputDialog>
#include <QFileDialog>
#include <QPixmap>
#include <QMessageBox>

#include "GUI/ui_GUI_PlaylistChooser.h"


GUI_PlaylistChooser::GUI_PlaylistChooser(QString name, QWidget *parent) :
	PlayerPlugin(name, parent),
	Ui::GUI_PlaylistChooser()
{

	setupUi(this);

	_playlist_chooser = new PlaylistChooser();
	_last_dir = _settings->get(Set::Lib_Path);
	_cur_idx = -1;
    _text_before_save = "";
    _target_playlist_dialog = new GUI_Target_Playlist_Dialog(this);

	btn_tool_pl->show_delete(false);
	btn_tool_pl->show_save(false);
	btn_tool_file->show_save_as(false);
	btn_tool_file->show_open(true);

	connect(btn_tool_pl, SIGNAL(sig_save()), this, SLOT(save_button_pressed()));
	connect(btn_tool_pl, SIGNAL(sig_delete()), this, SLOT(delete_button_pressed()));

	connect(btn_tool_file, SIGNAL(sig_save_as()), this, SLOT(save_as_button_pressed()));
	connect(btn_tool_file, SIGNAL(sig_open()), this, SLOT(load_button_pressed()));

	connect(combo_playlistchooser, SIGNAL(activated(int)), this, SLOT(playlist_selected(int)));
	connect(combo_playlistchooser, SIGNAL(editTextChanged( const QString & )), this, SLOT(text_changed ( const QString & )));
    connect(_target_playlist_dialog, SIGNAL(sig_target_chosen(QString,bool)), this, SLOT(got_save_params(QString,bool)));

	connect(_playlist_chooser, SIGNAL(sig_all_playlists_loaded(const QList<CustomPlaylistSkeleton>&)),
			this, SLOT(all_playlists_fetched(const QList<CustomPlaylistSkeleton>&)));

	_playlist_chooser->load_all_playlists();

    hide();

}

GUI_PlaylistChooser::~GUI_PlaylistChooser() {
	delete _playlist_chooser;
}



void GUI_PlaylistChooser::language_changed() {
	retranslateUi(this);
}



void GUI_PlaylistChooser::all_playlists_fetched(const QList<CustomPlaylistSkeleton>& skeletons) {
    int tmp_cur_idx = _cur_idx;
	combo_playlistchooser->clear();
	combo_playlistchooser->addItem("", -1);

	for(const CustomPlaylistSkeleton& skeleton : skeletons)
	{
		combo_playlistchooser->addItem(skeleton.name, skeleton.id);
	}

    _cur_idx = tmp_cur_idx;

	if(_cur_idx < combo_playlistchooser->count() && _cur_idx >= 0){
		combo_playlistchooser->setCurrentIndex(_cur_idx);
	}

    if(_text_before_save.size() > 0) {
		int idx = combo_playlistchooser->findText(_text_before_save);
		if(idx > 0 && idx < combo_playlistchooser->count()) {
			combo_playlistchooser->setCurrentIndex(idx);
            _cur_idx = idx;
        }
    }

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

		int answer = show_warning(tr("Overwrite?"));

		if(answer == QMessageBox::Yes){
			_playlist_chooser->save_playlist(cur_text);
		}

	}

    else if(cur_text.size() > 0 ) {
		_playlist_chooser->save_playlist(cur_text);
    }

	else return;
}

void GUI_PlaylistChooser::got_save_params(const QString& filename, bool relative) {
	_playlist_chooser->save_playlist_file(filename, relative);
}

void GUI_PlaylistChooser::save_as_button_pressed() {

    _target_playlist_dialog->show();
}


void GUI_PlaylistChooser::delete_button_pressed() {

    _text_before_save = "";
    int answer = show_warning(tr("Delete?"));

	if(_cur_idx < combo_playlistchooser->count() && _cur_idx != -1) {
		int val = combo_playlistchooser->itemData(_cur_idx).toInt();

		if(val >= 0 && answer == QMessageBox::Yes){
			_playlist_chooser->delete_playlist(val);
		}
	}
}


void GUI_PlaylistChooser::playlist_selected(int idx) {

	_cur_idx = idx;
	if(_cur_idx >= combo_playlistchooser->count() || _cur_idx < 0) return;

	QString name = combo_playlistchooser->currentText();

	int val = combo_playlistchooser->itemData(idx).toInt();
    bool val_bigger_zero = (val > 0);
	btn_tool_pl->show_delete(val_bigger_zero);
	text_changed(name);

	_playlist_chooser->load_single_playlist(val);

	le_playlist_file->clear();
}


void GUI_PlaylistChooser::load_button_pressed() {

    QStringList filelist = QFileDialog::getOpenFileNames(
                    this,
                    tr("Open Playlist files"),
                    _last_dir,
                    Helper::get_playlistfile_extensions().join(" "));

    QString lab_text = "";
	for(const QString& filename : filelist) {

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
		_playlist_chooser->playlist_files_selected(filelist);
    }
}


void GUI_PlaylistChooser::text_changed(const QString & text) {

	btn_tool_pl->show_save(text.size() > 0);

    QStringList lst;
	for(int i=0; i<combo_playlistchooser->count(); i++) {
		QString txt = combo_playlistchooser->itemText(i);
        if(txt.size() > 0)
            lst << txt.toLower();
    }

	btn_tool_pl->show_delete(lst.contains(text.toLower()));
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

    return warning_box.exec();
}
