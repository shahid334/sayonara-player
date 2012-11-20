/* GUI_PlaylistChooser.cpp */

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


#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"

#include <QWidget>
#include <QDockWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QCloseEvent>


#include "ui_GUI_PlaylistChooser.h"





GUI_PlaylistChooser::GUI_PlaylistChooser(QWidget* parent) : QDockWidget(parent) {

	_cur_idx = -1;
    _dark = false;
    _text_before_save = "";

    this->ui = new Ui::GUI_PlaylistChooser();
	this->ui->setupUi(this);

	this->ui->btn_save->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	this->ui->btn_save_as->setIcon(QIcon(Helper::getIconPath() + "save_as.png"));
	this->ui->btn_delete->setIcon(QIcon(Helper::getIconPath() + "delete.png"));

	this->ui->btn_save->setToolTip("Save");
	this->ui->btn_save_as->setToolTip("Save as");
	this->ui->btn_delete->setToolTip("Delete");

    this->ui->btn_delete->setEnabled(false);
    this->ui->btn_save->setEnabled(false);
    this->ui->btn_save_as->setEnabled(false);

    connect(this->ui->btn_apply, SIGNAL(clicked()), this, SLOT(apply_button_pressed()));

    connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_button_pressed()));
    connect(this->ui->btn_save_as, SIGNAL(clicked()), this, SLOT(save_as_button_pressed()));
    connect(this->ui->btn_delete, SIGNAL(clicked()), this, SLOT(delete_button_pressed()));
    connect(this->ui->btn_load, SIGNAL(clicked()), this, SLOT(load_button_pressed()));
    connect(this->ui->combo_playlistchooser, SIGNAL(currentIndexChanged(int)), this, SLOT(playlist_selected(int)));
    connect(this->ui->combo_playlistchooser, SIGNAL(editTextChanged ( const QString & )), this, SLOT(text_changed ( const QString & )));

    hide();

}

GUI_PlaylistChooser::~GUI_PlaylistChooser() {

}


void GUI_PlaylistChooser::changeSkin(bool dark){
    QString btn_style = Style::get_btn_style(dark);
    this->ui->combo_playlistchooser->setStyleSheet(Style::get_combobox_style(dark));
    this->ui->btn_apply->setStyleSheet(btn_style);
    this->ui->btn_delete->setStyleSheet(btn_style);
    this->ui->btn_save->setStyleSheet(btn_style);
    this->ui->btn_save_as->setStyleSheet(btn_style);
    this->ui->le_playlist_file->setStyleSheet(Style::get_lineedit_style(dark));
    this->ui->btn_load->setStyleSheet(Style::get_pushbutton_style(dark));

    _dark = dark;


}

void GUI_PlaylistChooser::all_playlists_fetched(QMap<int, QString>& mapping){
    int tmp_cur_idx = _cur_idx;
	this->ui->combo_playlistchooser->clear();
    this->ui->combo_playlistchooser->addItem("", -1);

	QList<int> keys = mapping.keys();
	foreach(int key, keys){
		QString name = mapping.value(key);
		this->ui->combo_playlistchooser->addItem(name, key);
	}

    _cur_idx = tmp_cur_idx;

    if(_cur_idx < this->ui->combo_playlistchooser->count() && _cur_idx >= 0)
        this->ui->combo_playlistchooser->setCurrentIndex(_cur_idx);

    if(_text_before_save.size() > 0){
        int idx = this->ui->combo_playlistchooser->findText(_text_before_save);
        if(idx > 0 && idx < this->ui->combo_playlistchooser->count()){
            this->ui->combo_playlistchooser->setCurrentIndex(idx);
            _cur_idx = idx;
        }
    }
}


void GUI_PlaylistChooser::playlist_changed(MetaDataList& v_md, int i, int radio_mode){

    Q_UNUSED(i);
    bool empty = (v_md.size() == 0);

    this->ui->btn_save->setEnabled(!empty && radio_mode == RADIO_OFF);
    this->ui->btn_save_as->setEnabled(!empty && radio_mode == RADIO_OFF);

    if(empty)
        this->ui->le_playlist_file->clear();
}



void GUI_PlaylistChooser::save_button_pressed(){

	if(_cur_idx >= this->ui->combo_playlistchooser->count() || _cur_idx < 0) return;

    QString cur_text = this->ui->combo_playlistchooser->currentText();
    _text_before_save = cur_text;

    QStringList lst;
    for(int i=0; i<this->ui->combo_playlistchooser->count(); i++){
        QString txt = this->ui->combo_playlistchooser->itemText(i);
        if(txt.size() > 0)
            lst << txt.toLower();
    }

    if( lst.contains(cur_text.toLower()) ){

        int val = this->ui->combo_playlistchooser->itemData(_cur_idx).toInt();
        int answer = show_warning("Overwrite?");

		if(answer == QMessageBox::Yes)
            emit sig_save_playlist(val);
	}

    else if(cur_text.size() > 0 ){
        emit sig_save_playlist(cur_text);
    }

	else return;
}



void GUI_PlaylistChooser::save_as_button_pressed(){

    QString filename = QFileDialog::getSaveFileName(this, "Save Playlist as", Helper::getSayonaraPath(), "m3u file(*.m3u)");

    if(filename.size() > 0)
        emit sig_save_playlist_file(filename);
}


void GUI_PlaylistChooser::delete_button_pressed(){

    _text_before_save = "";
    int answer = show_warning("Delete?");

	if(_cur_idx < this->ui->combo_playlistchooser->count() && _cur_idx != -1){
		int val = this->ui->combo_playlistchooser->itemData(_cur_idx).toInt();
		if(val >= 0 && answer == QMessageBox::Yes)
			emit sig_delete_playlist(val);
	}
}


void GUI_PlaylistChooser::playlist_selected(int idx){

	_cur_idx = idx;
    if(_cur_idx >= this->ui->combo_playlistchooser->count() || _cur_idx < 0) return;

	int val = this->ui->combo_playlistchooser->itemData(idx).toInt();
	bool val_bigger_zero = (val >= 0);

	this->ui->btn_delete->setEnabled(val_bigger_zero);
	this->ui->btn_save->setEnabled(val_bigger_zero);
	this->ui->btn_apply->setEnabled(val_bigger_zero);

	if(val_bigger_zero)
		emit sig_playlist_chosen(val);

    this->ui->le_playlist_file->clear();
}


void GUI_PlaylistChooser::load_button_pressed(){
    QStringList filelist = QFileDialog::getOpenFileNames(
                    this,
                    tr("Open Playlist files"),
                    QDir::homePath(),
                    Helper::get_playlistfile_extensions().join(" "));


    QString lab_text = "";
    foreach(QString filename, filelist){
        int last_index_of_sep = filename.lastIndexOf(QDir::separator());
        QString trimmed_filename = filename.right(filename.size() - last_index_of_sep - 1);
        trimmed_filename = trimmed_filename.left(trimmed_filename.lastIndexOf('.'));
        lab_text += trimmed_filename + ", ";
    }

    if(lab_text.size() > 2)
        lab_text = lab_text.left(lab_text.size() - 2);

    this->ui->le_playlist_file->setText(lab_text);

    if(filelist.size() > 0){
        emit sig_files_selected(filelist);
    }
}



void GUI_PlaylistChooser::apply_button_pressed(){

	if(_cur_idx >= this->ui->combo_playlistchooser->count() || _cur_idx == -1) return;
	int val = this->ui->combo_playlistchooser->itemData(_cur_idx).toInt();

	if(val >= 0)
		emit sig_playlist_chosen(val);
}


void GUI_PlaylistChooser::closeEvent ( QCloseEvent * event ){

    event->ignore();
    hide();
    close();
	emit sig_closed();
}


void GUI_PlaylistChooser::text_changed(const QString & text){

    this->ui->btn_save->setEnabled(text.size() > 0);


    QStringList lst;
    for(int i=0; i<this->ui->combo_playlistchooser->count(); i++){
        QString txt = this->ui->combo_playlistchooser->itemText(i);
        if(txt.size() > 0)
            lst << txt.toLower();
    }

    this->ui->btn_delete->setEnabled(lst.contains(text.toLower()));
}


int GUI_PlaylistChooser::show_warning(QString title_text){

    QMessageBox warning_box;
        warning_box.setParent(this);
        warning_box.setModal(true);
        warning_box.setWindowFlags(Qt::Dialog);
        warning_box.setIcon(QMessageBox::Warning);
        warning_box.setStandardButtons( QMessageBox::Yes | QMessageBox::No);
        warning_box.setText(QString("<b>") + title_text + "</b>");
        warning_box.setInformativeText("Are you sure?");
        warning_box.setWindowTitle(title_text);
        warning_box.setDefaultButton(QMessageBox::No);
        warning_box.button(QMessageBox::Yes)->setStyleSheet(Style::get_pushbutton_style(_dark));
        warning_box.button(QMessageBox::No)->setStyleSheet(Style::get_pushbutton_style(_dark));

    return warning_box.exec();
}
