/* GUI_Playlist.cpp */

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
 * GUI_Playlist.cpp
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */


#include "HelperStructs/Helper.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/CDirectoryReader.h"

#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/playlist/model/PlaylistItemModel.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"

#include "StreamPlugins/LastFM/LastFM.h"

#include <QDebug>
#include <QKeyEvent>
#include <QFileDialog>
#include <QScrollBar>
#include <QMacStyle>
#include <QStyleFactory>
#include <QMessageBox>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QUrl>
#include <QFileInfo>


// CTOR
GUI_Playlist::GUI_Playlist(QWidget *parent, GUI_InfoDialog* dialog) :
    QWidget(parent)
{

    _parent = parent;

    ui = new Ui::Playlist_Window();
    ui->setupUi(this);
    initGUI();

    QAction* clear_action = new QAction(this);
    clear_action->setShortcut(QKeySequence(tr("Ctrl+.")));
    clear_action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(clear_action, SIGNAL(triggered()), this->ui->btn_clear, SLOT(click()));
    this->addAction(clear_action);


    CSettingsStorage* settings = CSettingsStorage::getInstance();
    bool small_playlist_items = settings->getShowSmallPlaylist();
    ui->listView->show_big_items(!small_playlist_items);

    _info_dialog = dialog;

    _playlist_type = PlaylistTypeStd;

    _playlist_mode = settings->getPlaylistMode();

    ui->btn_append->setChecked(_playlist_mode.append);
    ui->btn_repAll->setChecked(_playlist_mode.repAll);
    ui->btn_dynamic->setChecked(_playlist_mode.dynamic);
    ui->btn_shuffle->setChecked(_playlist_mode.shuffle);
    ui->btn_numbers->setChecked(settings->getPlaylistNumbers());
	ui->btn_gapless->setChecked(settings->getGapless());
    ui->btn_import->setVisible(false);

    check_dynamic_play_button();
    setAcceptDrops(true);

    connect(ui->btn_clear, SIGNAL(clicked()), this, SLOT(clear_playlist_slot()));

    connect(ui->btn_rep1, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
    connect(ui->btn_repAll, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
    connect(ui->btn_shuffle, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
    connect(ui->btn_dynamic, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
    connect(ui->btn_append, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));

    connect(ui->listView, SIGNAL(sig_metadata_dropped(const MetaDataList&,int)), this, SLOT(metadata_dropped(const MetaDataList&,int)));
    connect(ui->listView, SIGNAL(sig_rows_removed(const QList<int>&, bool)), this, SLOT(rows_removed(const QList<int>&, bool)));
    connect(ui->listView, SIGNAL(sig_rows_moved(const QList<int>&, int)), this, SLOT(rows_moved(const QList<int>&, int)));

    connect(ui->listView, SIGNAL(sig_edit_clicked()), this, SLOT(psl_edit_tracks()));
    connect(ui->listView, SIGNAL(sig_info_clicked()), this, SLOT(psl_info_tracks()));

    connect(ui->listView, SIGNAL(sig_sel_changed(const MetaDataList&, const QList<int>&)),
            this, SLOT(sel_changed(const MetaDataList&, const QList<int>&)));
    connect(ui->listView, SIGNAL(sig_double_clicked(int)), this, SLOT(double_clicked(int)));
    connect(ui->listView, SIGNAL(sig_no_focus()), this, SLOT(no_focus()));

	connect(ui->btn_gapless, SIGNAL(toggled(bool)), this, SLOT(gapless_changed(bool)));

    //connect(ui->btn_import, SIGNAL(clicked()), this, SLOT(import_button_clicked()));
    connect(ui->btn_numbers, SIGNAL(toggled(bool)), this, SLOT(btn_numbers_changed(bool)));
}


// DTOR
GUI_Playlist::~GUI_Playlist() {
    delete ui;
}

void GUI_Playlist::changeEvent(QEvent* e){
    e->accept();
}

void GUI_Playlist::resizeEvent(QResizeEvent* e){

    e->accept();
    this->ui->listView->update();
    this->ui->listView->reset();
}

void GUI_Playlist::focusInEvent(QFocusEvent *e){

    this->ui->listView->setFocus();
}

void GUI_Playlist::no_focus(){
    this->parentWidget()->setFocus();
    emit sig_no_focus();
}

void GUI_Playlist::language_changed(){

    this->ui->retranslateUi(this);
    set_total_time_label();
    check_dynamic_play_button();
}

// initialize gui
// maybe the button state (pressed/unpressed) should be loaded from db here
void GUI_Playlist::initGUI(){

    QString icon_path = Helper::getIconPath();

    ui->btn_append->setIcon(QIcon(icon_path + "append.png"));
    ui->btn_rep1->setIcon(QIcon(icon_path + "rep1.png"));
    ui->btn_rep1->setVisible(false);
    ui->btn_repAll->setIcon(QIcon(icon_path + "repAll.png"));
    ui->btn_dynamic->setIcon(QIcon(icon_path + "dynamic.png"));
    ui->btn_shuffle->setIcon(QIcon(icon_path + "shuffle.png"));
    ui->btn_clear->setIcon(QIcon(icon_path + "broom.png"));
    ui->btn_import->setIcon(QIcon(icon_path + "import.png"));
    ui->btn_numbers->setIcon(QIcon(icon_path + "numbers.png"));
	ui->btn_gapless->setIcon(QIcon(icon_path + "gapless.png"));
}


void GUI_Playlist::library_path_changed(QString path){
    Q_UNUSED(path);
    check_dynamic_play_button();
}


void GUI_Playlist::check_dynamic_play_button(){

    QString libraryPath = CSettingsStorage::getInstance()->getLibraryPath();

    if(libraryPath.size() == 0 || !QFile::exists(libraryPath)){
        ui->btn_dynamic->setToolTip(tr("Please set library path first"));
    }

    else{
        ui->btn_dynamic->setToolTip(tr("Dynamic playing"));
    }
}

// Slot: comes from listview
void GUI_Playlist::metadata_dropped(const MetaDataList& v_md, int row){
    emit sig_tracks_dropped(v_md, row);
}

// SLOT: fill all tracks in v_metadata into playlist
void GUI_Playlist::fillPlaylist(const MetaDataList& v_metadata, int cur_play_idx, PlaylistType playlist_type){

    ui->listView->fill(v_metadata, cur_play_idx);
    _total_msecs = 0;
    _playlist_type= playlist_type;
    set_playlist_type(_playlist_type);

    foreach(MetaData md, v_metadata){
        _total_msecs += md.length_ms;
    }

    set_total_time_label();
}

// private SLOT: clear button pressed
void GUI_Playlist::clear_playlist_slot(){

    _total_msecs = 0;
    ui->lab_totalTime->setText(tr("Playlist empty"));
    ui->btn_import->setVisible(false);
    ui->listView->clear();

    emit sig_cleared();
}


// private SLOT: playlist item pressed (init drag & drop)
void GUI_Playlist::sel_changed(const MetaDataList& v_md, const QList<int>& sel_rows){

    _info_dialog->setMetaData(v_md);

    this->_info_dialog->set_tag_edit_visible(_playlist_type == PlaylistTypeStd);
    emit sig_selection_changed(sel_rows);
}

void GUI_Playlist::rows_moved(const QList<int> & lst, int tgt_idx){
    emit sig_rows_moved(lst, tgt_idx);
}


void GUI_Playlist::double_clicked(int row){
    emit sig_cur_idx_changed(row);
}

void GUI_Playlist::track_changed(int row){
    ui->listView->set_current_track(row);
}


void GUI_Playlist::gapless_changed(bool checked){
	 CSettingsStorage::getInstance()->setGapless(checked);
	emit sig_gapless(checked);
}


// private SLOT: rep1, repAll, shuffle or append has changed
void GUI_Playlist::playlist_mode_changed_slot(){

    this->parentWidget()->setFocus();

    _playlist_mode.rep1 = ui->btn_rep1->isChecked();
    _playlist_mode.repAll = ui->btn_repAll->isChecked();
    _playlist_mode.shuffle = ui->btn_shuffle->isChecked();
    _playlist_mode.append = ui->btn_append->isChecked();
    _playlist_mode.dynamic = ui->btn_dynamic->isChecked();

    CSettingsStorage::getInstance()->setPlaylistMode(_playlist_mode);

    emit playlist_mode_changed(_playlist_mode);
    emit save_playlist("bla");
}


void GUI_Playlist::psl_edit_tracks(){
    if(!_info_dialog) return;
    _info_dialog->setMode(INFO_MODE_TRACKS);
    _info_dialog->show(TAB_EDIT);
}

void GUI_Playlist::psl_info_tracks(){
    if(!_info_dialog) return;
    _info_dialog->setMode(INFO_MODE_TRACKS);
    _info_dialog->show(TAB_INFO);
}




void GUI_Playlist::dragLeaveEvent(QDragLeaveEvent* event){
    event->accept();
}


void GUI_Playlist::dragEnterEvent(QDragEnterEvent* event){
    event->accept();
}

void GUI_Playlist::dragMoveEvent(QDragMoveEvent* event){
    if(event->pos().y() < this->ui->listView->y()) this->ui->listView->scrollUp();
    else if(event->pos().y() > this->ui->listView->y() + this->ui->listView->height()) this->ui->listView->scrollDown();

}


void GUI_Playlist::dropEvent(QDropEvent* event){
    this->ui->listView->dropEventFromOutside(event);
}



void GUI_Playlist::set_total_time_label(){

    QString text = "";

    if(_playlist_type == PlaylistTypeStream){

        ui->lab_totalTime->setText(tr("Radio"));
        return;
    }

    ui->lab_totalTime->setContentsMargins(0, 2, 0, 2);

    int n_rows = ui->listView->get_num_rows();
    QString playlist_string = text + QString::number(n_rows);

    if(n_rows == 1)	playlist_string += tr(" Track - ");
    else playlist_string += tr(" Tracks - ");

    playlist_string += Helper::cvtMsecs2TitleLengthString(_total_msecs, false);

    ui->lab_totalTime->setText(playlist_string);
}



void GUI_Playlist::set_playlist_type(PlaylistType playlist_type){

    _playlist_type = playlist_type;

    ui->btn_append->setVisible(playlist_type == PlaylistTypeStd);
    ui->btn_dynamic->setVisible(playlist_type == PlaylistTypeStd);
    ui->btn_repAll->setVisible(playlist_type == PlaylistTypeStd);
    ui->btn_shuffle->setVisible(playlist_type == PlaylistTypeStd);

    int actions = 0;

    if(playlist_type != PlaylistTypeStd)
        actions = ENTRY_INFO;

    else
        actions = (ENTRY_INFO | ENTRY_REMOVE | ENTRY_EDIT);

    ui->listView->set_context_menu_actions(actions);
    ui->listView->set_drag_enabled(playlist_type != PlaylistTypeLFM);
}



void GUI_Playlist::psl_show_small_playlist_items(bool small_playlist_items){

    ui->listView->show_big_items(!small_playlist_items);
}


void GUI_Playlist::btn_numbers_changed(bool b){
    this->parentWidget()->setFocus();
    CSettingsStorage::getInstance()->setPlaylistNumbers(b);
    ui->listView->reset();
}


void GUI_Playlist::rows_removed(const QList<int>& lst, bool select_next_row){
    emit sig_rows_removed(lst, select_next_row);
}
