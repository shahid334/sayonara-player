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
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/CDirectoryReader.h"

#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/playlist/model/PlaylistItemModel.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"

#include "StreamPlugins/LastFM/LastFM.h"

#include <QKeyEvent>
#include <QFileDialog>
#include <QScrollBar>
#include <QMessageBox>
#include <QTextEdit>
#include <QAction>
#include <QMenu>


// CTOR
GUI_Playlist::GUI_Playlist(QWidget *parent, GUI_InfoDialog* dialog) :
	SayonaraWidget(parent),
	Ui::Playlist_Window()
{
	setupUi(this);

	_parent = parent;

    initGUI();

    QAction* clear_action = new QAction(this);
    clear_action->setShortcut(QKeySequence(tr("Ctrl+.")));
    clear_action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(clear_action, SIGNAL(triggered()), btn_clear, SLOT(click()));
	addAction(clear_action);

    _info_dialog = dialog;

    _playlist_type = PlaylistTypeStd;
	_playlist_mode = _settings->get(Set::PL_Mode);

	btn_append->setChecked(_playlist_mode.append);
	btn_repAll->setChecked(_playlist_mode.repAll);
	btn_dynamic->setChecked(_playlist_mode.dynamic);
	btn_shuffle->setChecked(_playlist_mode.shuffle);
	btn_gapless->setChecked(_playlist_mode.gapless);

    check_dynamic_play_button();
    setAcceptDrops(true);

	connect(btn_clear, SIGNAL(clicked()), this, SLOT(clear_playlist_slot()));

	connect(btn_rep1, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_repAll, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_shuffle, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_dynamic, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_append, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_gapless, SIGNAL(released()), this, SLOT(playlist_mode_changed()));


	connect(listView, SIGNAL(sig_metadata_dropped(const MetaDataList&,int)), this, SLOT(metadata_dropped(const MetaDataList&,int)));
	connect(listView, SIGNAL(sig_rows_removed(const QList<int>&, bool)), this, SLOT(rows_removed(const QList<int>&, bool)));
	connect(listView, SIGNAL(sig_rows_moved(const QList<int>&, int)), this, SLOT(rows_moved(const QList<int>&, int)));

	connect(listView, SIGNAL(sig_edit_clicked()), this, SLOT(psl_edit_tracks()));
	connect(listView, SIGNAL(sig_info_clicked()), this, SLOT(psl_info_tracks()));

	connect(listView, SIGNAL(sig_sel_changed(const MetaDataList&, const QList<int>&)),
            this, SLOT(sel_changed(const MetaDataList&, const QList<int>&)));
	connect(listView, SIGNAL(sig_double_clicked(int)), this, SLOT(double_clicked(int)));
	connect(listView, SIGNAL(sig_no_focus()), this, SLOT(no_focus()));

	connect(btn_numbers, SIGNAL(toggled(bool)), this, SLOT(btn_numbers_changed(bool)));

	REGISTER_LISTENER(Set::PL_SmallItems, _sl_change_small_playlist_items);
	REGISTER_LISTENER(Set::PL_Mode, _sl_playlist_mode_changed);
}


// DTOR
GUI_Playlist::~GUI_Playlist() {
}

void GUI_Playlist::changeEvent(QEvent* e) {
    e->accept();
}

void GUI_Playlist::resizeEvent(QResizeEvent* e) {

    e->accept();
	listView->update();
	listView->reset();
}

void GUI_Playlist::focusInEvent(QFocusEvent *e) {

	listView->setFocus();
}

void GUI_Playlist::no_focus() {
	parentWidget()->setFocus();
    emit sig_no_focus();
}

void GUI_Playlist::language_changed() {

	retranslateUi(this);
    set_total_time_label();
    check_dynamic_play_button();
}


void GUI_Playlist::initGUI() {

    btn_append->setIcon(Helper::getIcon("append.png"));
    btn_rep1->setIcon(Helper::getIcon("rep1.png"));
    btn_repAll->setIcon(Helper::getIcon("repAll.png"));
    btn_dynamic->setIcon(Helper::getIcon("dynamic.png"));
    btn_shuffle->setIcon(Helper::getIcon("shuffle.png"));
    btn_clear->setIcon(Helper::getIcon("broom.png"));
    btn_numbers->setIcon(Helper::getIcon("numbers.png"));
    btn_gapless->setIcon(Helper::getIcon("gapless.png"));

    btn_rep1->setVisible(false);

	lab_totalTime->setAccessibleDescription("");
}


void GUI_Playlist::library_path_changed(QString path) {
    Q_UNUSED(path);
    check_dynamic_play_button();
}


void GUI_Playlist::check_dynamic_play_button() {

	QString lib_path = _settings->get(Set::Lib_Path);

	if(lib_path.isEmpty() || !QFile::exists(lib_path)) {
		btn_dynamic->setToolTip(tr("Please set library path first"));
    }

    else{
		btn_dynamic->setToolTip(tr("Dynamic playing"));
    }
}


// Slot: comes from listview
void GUI_Playlist::metadata_dropped(const MetaDataList& v_md, int row) {
    emit sig_tracks_dropped(v_md, row);
}

// SLOT: fill all tracks in v_metadata into playlist
void GUI_Playlist::fillPlaylist(const MetaDataList& v_metadata, int cur_play_idx, PlaylistType playlist_type) {

	listView->fill(v_metadata, cur_play_idx);

	qint64 dur_ms = 0;
	_playlist_type = playlist_type;

	foreach(MetaData md, v_metadata) {
		dur_ms += md.length_ms;
    }

	set_total_time_label(dur_ms);
	set_playlist_type(playlist_type);
}

// private SLOT: clear button pressed
void GUI_Playlist::clear_playlist_slot() {

	lab_totalTime->setText(tr("Playlist empty"));
	lab_totalTime->setAccessibleDescription("");
	listView->clear();

    emit sig_cleared();
}


// private SLOT: playlist item pressed (init drag & drop)
void GUI_Playlist::sel_changed(const MetaDataList& v_md, const QList<int>& sel_rows) {

	if(sel_rows.size() == 0) return;

	_info_dialog->setInfoMode(InfoDialogMode_Tracks);
    _info_dialog->setMetaData(v_md);

	_info_dialog->set_tag_edit_visible( _playlist_type == PlaylistTypeStd );

    emit sig_selection_changed(sel_rows);
}

void GUI_Playlist::rows_moved(const QList<int> & lst, int tgt_idx) {

	if(lst.size() == 0) return;

    emit sig_rows_moved(lst, tgt_idx);
}


void GUI_Playlist::double_clicked(int row) {
    emit sig_cur_idx_changed(row);
}

void GUI_Playlist::track_changed(int row) {
	listView->set_current_track(row);
}


// internal gui slot
void GUI_Playlist::playlist_mode_changed() {

	parentWidget()->setFocus();

	PlaylistMode plm;

	plm.append = btn_append->isChecked();
	plm.rep1 = btn_rep1->isChecked();
	plm.repAll = btn_repAll->isChecked();
	plm.shuffle = btn_shuffle->isChecked();
	plm.dynamic = btn_dynamic->isChecked();
	plm.gapless = btn_gapless->isChecked();

	if(plm == _playlist_mode){
		return;
	}

	plm = _playlist_mode;
	_settings->set(Set::PL_Mode, _playlist_mode);
}

// setting slot
void GUI_Playlist::_sl_playlist_mode_changed(){

	PlaylistMode plm = _settings->get(Set::PL_Mode);

	if(plm == _playlist_mode) return;

	btn_append->setChecked(_playlist_mode.append);
	btn_rep1->setChecked(_playlist_mode.rep1);
	btn_repAll->setChecked(_playlist_mode.repAll);
	btn_shuffle->setChecked(_playlist_mode.shuffle);
	btn_dynamic->setChecked(_playlist_mode.dynamic);
	btn_gapless->setChecked(_playlist_mode.gapless);

	btn_append->setVisible(_playlist_mode.ui_append);
	btn_repAll->setVisible(_playlist_mode.ui_repAll);
	btn_dynamic->setVisible(_playlist_mode.ui_dynamic);
	btn_shuffle->setVisible(_playlist_mode.ui_shuffle);
	btn_gapless->setVisible(_playlist_mode.ui_gapless);
}


void GUI_Playlist::psl_edit_tracks() {
    if(!_info_dialog) return;
    _info_dialog->show(TAB_EDIT);
}


void GUI_Playlist::psl_info_tracks() {
    if(!_info_dialog) return;
    _info_dialog->show(TAB_INFO);
}


void GUI_Playlist::dragLeaveEvent(QDragLeaveEvent* event) {
    event->accept();
}


void GUI_Playlist::dragEnterEvent(QDragEnterEvent* event) {
    event->accept();
}


void GUI_Playlist::dragMoveEvent(QDragMoveEvent* event) {
	if(event->pos().y() < listView->y()) listView->scrollUp();
	else if(event->pos().y() > listView->y() + listView->height()) listView->scrollDown();

}


void GUI_Playlist::dropEvent(QDropEvent* event) {
	listView->dropEventFromOutside(event);
}


void GUI_Playlist::set_total_time_label(qint64 dur_ms) {

	QString text = "";
	QString time_str;

	if(dur_ms > 0){
		time_str = Helper::cvt_ms_to_string(dur_ms, true, false);
	}

	else{
		time_str = lab_totalTime->accessibleDescription();
	}

	if(_playlist_type == PlaylistTypeStream) {

		lab_totalTime->setText(tr("Radio"));
        return;
    }

	int n_rows = listView->get_num_rows();
    QString playlist_string = text + QString::number(n_rows);

	if(n_rows == 1)	{
		playlist_string += " " + tr("Track");
	}

	else {
		playlist_string += " " + tr("Tracks");
	}

	if( !time_str.isEmpty() ){
		playlist_string += " - " + time_str;
		lab_totalTime->setAccessibleDescription(time_str);
	}

	lab_totalTime->setText(playlist_string);
	lab_totalTime->setContentsMargins(0, 2, 0, 2);
}


void GUI_Playlist::set_playlist_type(PlaylistType playlist_type) {

    _playlist_type = playlist_type;

	btn_append->setVisible(playlist_type == PlaylistTypeStd);
	btn_dynamic->setVisible(playlist_type == PlaylistTypeStd);
	btn_repAll->setVisible(playlist_type == PlaylistTypeStd);
	btn_shuffle->setVisible(playlist_type == PlaylistTypeStd);

    int actions = 0;

    if(playlist_type != PlaylistTypeStd){
        actions = ENTRY_INFO;
    }

    else{
        actions = (ENTRY_INFO | ENTRY_REMOVE | ENTRY_EDIT);
    }

	listView->set_context_menu_actions(actions);
}


void GUI_Playlist::_sl_change_small_playlist_items() {

	bool small_items = _settings->get(Set::PL_SmallItems);
	listView->show_big_items(!small_items);
}


void GUI_Playlist::btn_numbers_changed(bool b) {
	parentWidget()->setFocus();
	_settings->set(Set::PL_ShowNumbers, b);
	listView->reset();
}


void GUI_Playlist::rows_removed(const QList<int>& lst, bool select_next_row) {
    emit sig_rows_removed(lst, select_next_row);
}
