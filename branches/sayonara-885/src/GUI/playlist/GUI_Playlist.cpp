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
GUI_Playlist::GUI_Playlist(PlaylistHandler* playlist, GUI_InfoDialog* info_dialog, QWidget *parent) :
	SayonaraWidget(parent),
	Ui::Playlist_Window()
{
	setupUi(this);

	_parent = parent;

	_playlist = playlist;
	_info_dialog = info_dialog;


    initGUI();

    QAction* clear_action = new QAction(this);
    clear_action->setShortcut(QKeySequence(tr("Ctrl+.")));
    clear_action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(clear_action, SIGNAL(triggered()), btn_clear, SLOT(click()));
	addAction(clear_action);

    _playlist_type = PlaylistTypeStd;
	_playlist_mode = _settings->get(Set::PL_Mode);

	btn_rep1->setChecked(_playlist_mode.rep1);
	btn_append->setChecked(_playlist_mode.append);
	btn_repAll->setChecked(_playlist_mode.repAll);
	btn_dynamic->setChecked(_playlist_mode.dynamic);
	btn_shuffle->setChecked(_playlist_mode.shuffle);
	btn_gapless->setChecked(_playlist_mode.gapless);
	btn_numbers->setChecked(_settings->get(Set::PL_ShowNumbers));

	_tab_idx = 1;


    check_dynamic_play_button();
    setAcceptDrops(true);

	connect(_playlist, SIGNAL(sig_playlist_created(const MetaDataList&,int,PlaylistType,int)),
			this, SLOT(fill_playlist(const MetaDataList&,int,PlaylistType,int)));
	connect(_playlist, SIGNAL(sig_cur_track_idx_changed(int)), this, SLOT(track_changed(int)));



	connect(btn_clear, SIGNAL(clicked()), this, SLOT(clear_playlist_slot()));

	connect(btn_rep1, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_repAll, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_shuffle, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_dynamic, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_append, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_gapless, SIGNAL(released()), this, SLOT(playlist_mode_changed()));

	connect(btn_numbers, SIGNAL(toggled(bool)), this, SLOT(btn_numbers_changed(bool)));

	connect(btn_new_tab, SIGNAL(clicked()), this, SLOT(add_playlist_clicked()));
	connect(tw_playlists, SIGNAL(tabCloseRequested(int)), this, SLOT(close_playlist_clicked(int)));
	connect(tw_playlists, SIGNAL(currentChanged(int)), this, SLOT(tab_playlist_clicked(int)));

	connect(_playlist, SIGNAL(sig_new_playlist_added(int, QString)), this, SLOT(playlist_added(int, QString)));
	connect(_playlist, SIGNAL(sig_playlist_index_changed(int)), this, SLOT(playlist_idx_changed(int)));
	connect(_playlist, SIGNAL(sig_playlist_closed(int)), this, SLOT(playlist_closed(int)));


	initPlaylistView(listView);

	_playlist_views.append(listView);
	_cur_playlist_view = listView;
	_cur_playlist_idx = 0;

	tw_playlists->hide_tabbar();

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
	_cur_playlist_view->update();
	_cur_playlist_view->reset();
}

void GUI_Playlist::language_changed() {

	retranslateUi(this);
    set_total_time_label();
    check_dynamic_play_button();
}


void GUI_Playlist::initGUI() {
	lab_totalTime->setAccessibleDescription("");
}

void GUI_Playlist::initPlaylistView(const PlaylistView* pl_view){

	connect(pl_view, SIGNAL(sig_metadata_dropped(const MetaDataList&,int)), this, SLOT(metadata_dropped(const MetaDataList&,int)));
	connect(pl_view, SIGNAL(sig_rows_removed(const QList<int>&)), this, SLOT(rows_removed(const QList<int>&)));
	connect(pl_view, SIGNAL(sig_rows_moved(const QList<int>&, int)), this, SLOT(rows_moved(const QList<int>&, int)));

	connect(pl_view, SIGNAL(sig_edit_clicked()), this, SLOT(psl_edit_tracks()));
	connect(pl_view, SIGNAL(sig_info_clicked()), this, SLOT(psl_info_tracks()));

	connect(pl_view, SIGNAL(sig_sel_changed(const MetaDataList&, const QList<int>&)),
			this, SLOT(sel_changed(const MetaDataList&, const QList<int>&)));
	connect(pl_view, SIGNAL(sig_double_clicked(int)), this, SLOT(double_clicked(int)));
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
	_playlist->psl_insert_tracks(v_md, row);
}

// SLOT: fill all tracks in v_metadata into playlist
void GUI_Playlist::fill_playlist(const MetaDataList& v_metadata, int cur_play_idx, PlaylistType playlist_type, int pl_idx) {

	_playlist_views[pl_idx]->fill(v_metadata, cur_play_idx);

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
	_cur_playlist_view->clear();

	_playlist->psl_clear_playlist();
}


// private SLOT: playlist item pressed (init drag & drop)
void GUI_Playlist::sel_changed(const MetaDataList& v_md, const QList<int>& sel_rows) {

	if(sel_rows.size() == 0) return;

	_info_dialog->setInfoMode(InfoDialogMode_Tracks);
	_info_dialog->set_metadata(v_md);

	_info_dialog->set_tag_edit_visible( _playlist_type == PlaylistTypeStd );

	_playlist->psl_selection_changed(sel_rows);
}

void GUI_Playlist::rows_moved(const QList<int> & lst, int tgt_idx) {

	if(lst.size() == 0) return;

	_playlist->psl_move_rows(lst, tgt_idx);
}


void GUI_Playlist::double_clicked(int row) {
	_playlist->psl_change_track(row);
}

void GUI_Playlist::track_changed(int row) {
	_cur_playlist_view->set_current_track(row);
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

    _playlist_mode = plm;
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

	btn_rep1->setVisible(_playlist_mode.ui_rep1);
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
	if(event->pos().y() < _cur_playlist_view->y()) _cur_playlist_view->scrollUp();
	else if(event->pos().y() > _cur_playlist_view->y() + _cur_playlist_view->height()) _cur_playlist_view->scrollDown();

}


void GUI_Playlist::dropEvent(QDropEvent* event) {
	_cur_playlist_view->dropEventFromOutside(event);
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

	int n_rows = _cur_playlist_view->get_num_rows();
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
	btn_rep1->setVisible(playlist_type == PlaylistTypeStd);

    int actions = 0;

    if(playlist_type != PlaylistTypeStd){
        actions = ENTRY_INFO;
    }

    else{
        actions = (ENTRY_INFO | ENTRY_REMOVE | ENTRY_EDIT);
    }

	_cur_playlist_view->set_context_menu_actions(actions);
}


void GUI_Playlist::_sl_change_small_playlist_items() {

	bool small_items = _settings->get(Set::PL_SmallItems);
	_cur_playlist_view->show_big_items(!small_items);
}


void GUI_Playlist::btn_numbers_changed(bool b) {

	parentWidget()->setFocus();

	_settings->set(Set::PL_ShowNumbers, b);
	_cur_playlist_view->reset();
}


void GUI_Playlist::rows_removed(const QList<int>& lst) {
	_playlist->psl_remove_rows(lst);
}

void GUI_Playlist::add_playlist_clicked(){
	int idx = _playlist->add_new_playlist();
	_playlist->change_playlist_index(idx);
}

void GUI_Playlist::close_playlist_clicked(int idx){
	_playlist->close_playlist(idx);
}

void GUI_Playlist::tab_playlist_clicked(int idx){
	_playlist->change_playlist_index(idx);
}


void GUI_Playlist::playlist_added(int idx, QString name){

	PlaylistView* pl_view = new PlaylistView();
	initPlaylistView(pl_view);

	QString new_name = name;

	if(name.isEmpty()){
		new_name = QString("Playlist ") + QString::number(_tab_idx + 1);
		_tab_idx++;
	}

	_playlist_views.append(pl_view);
	tw_playlists->addTab(pl_view, new_name);

	tw_playlists->show_tabbar();
	tw_playlists->setCurrentIndex(idx);
}

void GUI_Playlist::playlist_idx_changed(int idx){

	if(idx == _cur_playlist_idx) return;

	if(idx >= tw_playlists->count() || idx < 0) return;

	_cur_playlist_idx = idx;
	_cur_playlist_view = _playlist_views[idx];

	tw_playlists->setCurrentIndex(idx);
}


void GUI_Playlist::playlist_closed(int idx){

	if(idx >= tw_playlists->count() || idx < 0) return;

	tw_playlists->removeTab(idx);
	_playlist_views.removeAt(idx);

	if(_playlist_views.count() == 1){
		tw_playlists->hide_tabbar();
	}
}

