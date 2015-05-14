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

#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/DirectoryReader/DirectoryReader.h"

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
GUI_Playlist::GUI_Playlist(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::Playlist_Window()
{
	setupUi(this);

	_playlist = PlaylistHandler::getInstance();
	_play_manager = PlayManager::getInstance();
	_playlist_menu = new PlaylistMenu(this);
	_info_dialog = new GUI_InfoDialog(this);
	_ui_shutdown = new GUI_Shutdown(this);

	_cur_playlist_idx = -1;
	tw_playlists->removeTab(0);

    QAction* clear_action = new QAction(this);
    clear_action->setShortcut(QKeySequence(tr("Ctrl+.")));
    clear_action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(clear_action, SIGNAL(triggered()), btn_clear, SLOT(click()));
	addAction(clear_action);

	_playlist_mode = _settings->get(Set::PL_Mode);

	btn_rep1->setChecked(_playlist_mode.rep1);
	btn_append->setChecked(_playlist_mode.append);
	btn_repAll->setChecked(_playlist_mode.repAll);
	btn_dynamic->setChecked(_playlist_mode.dynamic);
	btn_shuffle->setChecked(_playlist_mode.shuffle);
	btn_gapless->setChecked(_playlist_mode.gapless);

    check_dynamic_play_button();
    setAcceptDrops(true);


	connect(_playlist,	SIGNAL(sig_cur_track_idx_changed(int, int)),
			this,		SLOT(track_changed(int, int)));
	connect(_playlist,	SIGNAL(sig_playlist_created(const Playlist*)),
			this,		SLOT(fill_playlist(const Playlist*)));
	connect(_playlist,	SIGNAL(sig_tab_name_changed(int, const QString&)),
			this,		SLOT(tab_name_changed(int, const QString&)));
	connect(_playlist,	SIGNAL(sig_new_playlist_added(int, const QString&)),
			this,		SLOT(playlist_added(int, const QString&)));
	connect(_playlist,	SIGNAL(sig_playlist_index_changed(int, bool)),
			this,		SLOT(playlist_idx_changed(int, bool)));
	connect(_playlist,	SIGNAL(sig_playlist_closed(int)),
			this,		SLOT(playlist_closed(int)));

	connect(_play_manager, SIGNAL(sig_playlist_finished()), this, SLOT(tab_clear_all_icons()));
	connect(_play_manager, SIGNAL(sig_playstate_changed(PlayManager::PlayState)),
			this, SLOT(playstate_changed(PlayManager::PlayState)));

	connect(btn_clear, SIGNAL(clicked()), this, SLOT(clear_playlist_slot()));
	connect(btn_rep1, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_repAll, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_shuffle, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_dynamic, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_append, SIGNAL(released()), this, SLOT(playlist_mode_changed()));
	connect(btn_gapless, SIGNAL(released()), this, SLOT(playlist_mode_changed()));


	connect(btn_menu, SIGNAL(clicked()), this, SLOT(btn_menu_pressed()));

	connect(btn_new_tab, SIGNAL(clicked()), this, SLOT(add_playlist_clicked()));

	connect(tw_playlists, SIGNAL(tabCloseRequested(int)), this, SLOT(close_playlist_clicked(int)));
	connect(tw_playlists, SIGNAL(currentChanged(int)), this, SLOT(tab_playlist_clicked(int)));
	connect(tw_playlists, SIGNAL(sig_tab_delete(int)), this, SLOT(tab_delete_playlist_clicked(int)));
	connect(tw_playlists, SIGNAL(sig_tab_save(int)), this, SLOT(tab_save_playlist_clicked(int)));
	connect(tw_playlists, SIGNAL(sig_tab_save_as(int, const QString&)), this, SLOT(tab_save_playlist_as_clicked(int, const QString& )));


	connect(btn_shutdown, SIGNAL(toggled(bool)), this, SLOT(shutdown_toggled(bool)));
	connect(_playlist_menu, SIGNAL(sig_shutdown(bool)), this, SLOT(shutdown_toggled(bool)));
	connect(_ui_shutdown, SIGNAL(sig_closed()), this, SLOT(shutdown_closed()));

	initPlaylistView(listView);

	REGISTER_LISTENER(Set::PL_SmallItems, _sl_change_small_playlist_items);
	REGISTER_LISTENER(Set::PL_Mode, _sl_playlist_mode_changed);
	REGISTER_LISTENER(Set::PL_ShowNumbers, _sl_numbers_changed);

	_playlist->load_old_playlists();
	tw_playlists->setCurrentIndex(_settings->get(Set::PL_LastPlaylist));
	tw_playlists->setTabsClosable(_playlist_views.size() > 1);

	_cur_playlist_idx = tw_playlists->currentIndex();

	btn_shutdown->setVisible(Shutdown::getInstance()->is_running());
}




// DTOR
GUI_Playlist::~GUI_Playlist() {
	delete _info_dialog;
	delete _ui_shutdown;
}

void GUI_Playlist::changeEvent(QEvent* e) {
    e->accept();
}

void GUI_Playlist::resizeEvent(QResizeEvent* e) {

	PlaylistView* cur_view = get_current_view();

    e->accept();

	if(cur_view){
		cur_view->update();
		cur_view->reset();
	}
}

void GUI_Playlist::language_changed() {

	retranslateUi(this);
    set_total_time_label();
    check_dynamic_play_button();
}


void GUI_Playlist::initPlaylistView(const PlaylistView* pl_view){

	connect(pl_view, SIGNAL(sig_metadata_dropped(const MetaDataList&,int)), this, SLOT(metadata_dropped(const MetaDataList&,int)));
	connect(pl_view, SIGNAL(sig_rows_removed(const QList<int>&)), this, SLOT(rows_removed(const QList<int>&)));
	connect(pl_view, SIGNAL(sig_rows_moved(const QList<int>&, int)), this, SLOT(rows_moved(const QList<int>&, int)));

	connect(pl_view, SIGNAL(sig_info_clicked()), this, SLOT(menu_info_clicked()));
	connect(pl_view, SIGNAL(sig_edit_clicked()), this, SLOT(menu_edit_clicked()));


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
	_playlist->insert_tracks(v_md, row);
}

// SLOT: fill all tracks in v_metadata into playlist
void GUI_Playlist::fill_playlist(const Playlist* pl) {

	int pl_idx = pl->get_idx();
	qint64 dur_ms = 0;

	PlaylistType pl_type = pl->get_type();
	const MetaDataList& v_md = pl->get_playlist();

	if(pl_idx < 0 || pl_idx >= _playlist_views.size()) return;

	_playlist_views[pl_idx]->fill(v_md);
	_playlist_views[pl_idx]->set_playlist_type(pl_type);

	for(const MetaData& md : v_md) {
		dur_ms += md.length_ms;
    }

	_total_time[pl_idx] = dur_ms;
	set_total_time_label(dur_ms);
}

// private SLOT: clear button pressed
void GUI_Playlist::clear_playlist_slot() {

	PlaylistView* cur_view = get_current_view();

	lab_totalTime->setText(tr("Playlist empty"));

	if(cur_view){
		cur_view->clear();
		_total_time[_cur_playlist_idx] = 0;
	}

	_playlist->clear_playlist();
}


// private SLOT: playlist item pressed (init drag & drop)
void GUI_Playlist::sel_changed(const MetaDataList& v_md, const QList<int>& sel_rows) {

	if(sel_rows.size() == 0) return;

	_info_dialog->setInfoMode(InfoDialogMode_Tracks);
	_info_dialog->set_metadata(v_md);

	PlaylistView* cur_view = get_current_view();
	if(cur_view){
		PlaylistType pl_type = cur_view->get_playlist_type();
		_info_dialog->set_tag_edit_visible( pl_type == PlaylistTypeStd );
	}

	_playlist->selection_changed(sel_rows);
}

void GUI_Playlist::rows_moved(const QList<int> & lst, int tgt_idx) {

	if(lst.size() == 0) return;

	_playlist->move_rows(lst, tgt_idx);
}


void GUI_Playlist::double_clicked(int row) {
	if(_cur_playlist_idx < 0) return;

	_playlist->change_track(row, _cur_playlist_idx);
}



void GUI_Playlist::track_changed(int row, int playlist_idx) {

	tab_clear_all_icons();

	if(playlist_idx < 0 || playlist_idx >= _playlist_views.size()) {
		return;
	}

	_playlist_views[playlist_idx]->set_current_track(row);

	QIcon icon_play = Helper::getIcon("play_bordered");
	tw_playlists->setTabIcon(playlist_idx, icon_play);
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
	_settings->set(Set::Engine_Gapless, plm.gapless);
}

// setting slot
void GUI_Playlist::_sl_playlist_mode_changed(){

	PlaylistMode plm = _settings->get(Set::PL_Mode);

	if(plm == _playlist_mode) return;

	_playlist_mode = plm;

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


	/*btn_append->setVisible(false);
	btn_dynamic->setVisible(false);
	btn_repAll->setVisible(false);
	btn_shuffle->setVisible(false);
	btn_rep1->setVisible(false);
	btn_gapless->setVisible(false);*/
}


void GUI_Playlist::menu_info_clicked() {
    if(!_info_dialog) return;
	_info_dialog->show(TAB_INFO);
}


void GUI_Playlist::menu_edit_clicked() {
    if(!_info_dialog) return;
	_info_dialog->show(TAB_EDIT);
}


void GUI_Playlist::dragLeaveEvent(QDragLeaveEvent* event) {
    event->accept();
}


void GUI_Playlist::dragEnterEvent(QDragEnterEvent* event) {
    event->accept();
}


void GUI_Playlist::dragMoveEvent(QDragMoveEvent* event) {
	PlaylistView* cur_view = get_current_view();

	if(!cur_view) return;

	if( event->pos().y() < cur_view->y()){
		cur_view->scrollUp();
	}

	else if(event->pos().y() > cur_view->y() + cur_view->height()){
		cur_view->scrollDown();
	}
}


void GUI_Playlist::dropEvent(QDropEvent* event) {
	PlaylistView* cur_view = get_current_view();
	if(cur_view){
		cur_view->dropEventFromOutside(event);
	}
}


void GUI_Playlist::set_total_time_label(qint64 dur_ms) {

	PlaylistView* cur_view = get_current_view();
	PlaylistType pl_type = PlaylistTypeStd;
	int n_rows = 0;
	QString text;
	QString time_str;
	QString playlist_string;


	if(dur_ms > 0){
		time_str = Helper::cvt_ms_to_string(dur_ms, true, false);
	}


	if(cur_view){
		n_rows = cur_view->get_num_rows();
		pl_type = cur_view->get_playlist_type();
	}

	if(pl_type == PlaylistTypeStream) {
		lab_totalTime->setText(tr("Radio"));
		return;
	}


	playlist_string = text + QString::number(n_rows);

	if(n_rows == 1)	{
		playlist_string += " " + tr("Track");
	}

	else {
		playlist_string += " " + tr("Tracks");
	}

	if( !time_str.isEmpty() ){
		playlist_string += " - " + time_str;
	}

	lab_totalTime->setText(playlist_string);
	lab_totalTime->setContentsMargins(0, 2, 0, 2);
}


void GUI_Playlist::set_playlist_type(PlaylistType playlist_type) {

	PlaylistView* cur_view = get_current_view();

	cur_view->set_playlist_type(playlist_type);

	btn_append->setVisible(playlist_type == PlaylistTypeStd);
	btn_dynamic->setVisible(playlist_type == PlaylistTypeStd);
	btn_repAll->setVisible(playlist_type == PlaylistTypeStd);
	btn_shuffle->setVisible(playlist_type == PlaylistTypeStd);
	btn_rep1->setVisible(playlist_type == PlaylistTypeStd);
	btn_gapless->setVisible(playlist_type == PlaylistTypeStd);

    int actions = 0;

    if(playlist_type != PlaylistTypeStd){
        actions = ENTRY_INFO;
    }

    else{
        actions = (ENTRY_INFO | ENTRY_REMOVE | ENTRY_EDIT);
    }

	if(cur_view){
		cur_view->set_context_menu_actions(actions);
	}
}


void GUI_Playlist::_sl_change_small_playlist_items() {

	PlaylistView* cur_view = get_current_view();
	bool small_items = _settings->get(Set::PL_SmallItems);

	if(cur_view) {
		cur_view->show_big_items(!small_items);
	}
}

void GUI_Playlist::_sl_numbers_changed(){

	PlaylistView* cur_view = get_current_view();

	parentWidget()->setFocus();

	if( cur_view ){
		cur_view->reset();
	}
}


void GUI_Playlist::btn_numbers_changed(bool b) {

	_settings->set(Set::PL_ShowNumbers, b);

}


void GUI_Playlist::rows_removed(const QList<int>& lst) {
	_playlist->remove_rows(lst);
}

void GUI_Playlist::add_playlist_clicked(){
	QString name = _playlist->request_new_playlist_name();
	_playlist->create_empty_playlist(name, true);
}

void GUI_Playlist::close_playlist_clicked(int idx){
	_playlist->close_playlist(idx);
}

void GUI_Playlist::tab_playlist_clicked(int idx){

	_playlist->change_playlist_index(idx);
}

PlaylistView* GUI_Playlist::get_view_by_idx(int idx){
	if(idx < 0 || idx >= _playlist_views.size()) return NULL;

	return _playlist_views[idx];
}

PlaylistView* GUI_Playlist::get_current_view(){

	return get_view_by_idx(_cur_playlist_idx);
}

void GUI_Playlist::playlist_added(int idx, const QString& name){

	QString new_name = name;
	PlaylistView* pl_view = new PlaylistView();
	initPlaylistView(pl_view);

	_playlist_views.append(pl_view);
	_total_time.append(0);

	tw_playlists->addTab(pl_view, new_name);
	tw_playlists->setCurrentIndex(idx);
	tw_playlists->setTabsClosable(_playlist_views.size() > 1);

}


void GUI_Playlist::playlist_idx_changed(int idx, bool temporary){

	if(idx >= tw_playlists->count() || idx < 0) return;

	_cur_playlist_idx = idx;

	tw_playlists->setCurrentIndex(idx);

	PlaylistType pl_type = get_current_view()->get_playlist_type();
	bool show_save = (!temporary) && (pl_type == PlaylistTypeStd);
	bool show_save_as = (pl_type == PlaylistTypeStd);
	bool show_delete = (!temporary) && (pl_type == PlaylistTypeStd);

	tw_playlists->show_menu_items(show_save, show_save_as, show_delete);
	set_total_time_label( _total_time[idx] );
}


void GUI_Playlist::playlist_closed(int idx){

	if(idx >= tw_playlists->count() || idx < 0) return;

	_playlist_views.removeAt(idx);

	_total_time.remove(idx);
	tw_playlists->removeTab(idx);
	tw_playlists->setTabsClosable(_playlist_views.size() > 1);
	tw_playlists->repaint();
}


void GUI_Playlist::tab_save_playlist_clicked(int idx){
	PlaylistDBInterface::SaveAsAnswer success = _playlist->save_playlist(idx);
	show_save_message_box(success);
}

void GUI_Playlist::tab_save_playlist_as_clicked(int idx, const QString& str){

	PlaylistDBInterface::SaveAsAnswer success = _playlist->save_playlist_as(idx, str, false);
	if(success == PlaylistDBInterface::SaveAs_AlreadyThere){

		int answer = show_save_message_box(success);

		if(answer == QMessageBox::No) {
			return;
		}

		else{
			success = _playlist->save_playlist_as(idx, str, true);
			show_save_message_box(success);
		}
	}

	else{
		show_save_message_box(success);
	}
}

void GUI_Playlist::tab_delete_playlist_clicked(int idx){
	int answer = QMessageBox::warning(
				this,
				tr("Delete"),
				tr("Really?"),
				QMessageBox::Yes,
				QMessageBox::No
		);

	if(answer == QMessageBox::No) {
		return;
	}

	_playlist->delete_playlist(idx);
}

void GUI_Playlist::tab_name_changed(int idx, const QString &name){
	tw_playlists->rename_tab(idx, name);
}

int GUI_Playlist::show_save_message_box(PlaylistDBInterface::SaveAsAnswer answer){

	switch(answer){
		case PlaylistDBInterface::SaveAs_Error:
			QMessageBox::warning(
						this,
						tr("Save playlist as"),
						tr("Cannot save playlist."));

		break;
		case PlaylistDBInterface::SaveAs_AlreadyThere:
			return QMessageBox::warning(
								this,
								tr("Save playlist as"),
								tr("Playlist exists") + "\n" + tr("Overwrite?"),
								QMessageBox::Yes,
								QMessageBox::No);

		case PlaylistDBInterface::SaveAs_ExternTracksError:
			QMessageBox::warning(
								this,
								tr("Save playlist"),
								tr("Playlists are currently only supported for library tracks."));
		break;
		default:
			return 0;
	}

	return 0;
}

void GUI_Playlist::playstate_changed(PlayManager::PlayState state){
	if(state == PlayManager::PlayState_Stopped){
		tab_clear_all_icons();
	}
}

void GUI_Playlist::tab_clear_all_icons(){

	QIcon icon;

	for(int i=0; i<tw_playlists->count(); i++){
		tw_playlists->setTabIcon(i, icon);
		tw_playlists->setIconSize(QSize(16, 16));
	}
}

void GUI_Playlist::shutdown_toggled(bool b){

	if(Shutdown::getInstance()->is_running() == b) return;

	if(b){
		_ui_shutdown->show();

	}

	else{
		int ret = QMessageBox::warning(this,
								tr("Cancel shutdown"),
								tr("Cancel shutdown?"),
								QMessageBox::Yes, QMessageBox::No);

		if(ret == QMessageBox::No) {
			btn_shutdown->setChecked(true);
			return;
		}

		Shutdown::getInstance()->stop();

	}

	btn_shutdown->setVisible(b);
	btn_shutdown->setChecked(b);
	_playlist_menu->set_shutdown(b);

}


void GUI_Playlist::shutdown_closed(){

	bool b = Shutdown::getInstance()->is_running();
	btn_shutdown->setVisible(b);
	btn_shutdown->setChecked(b);

}

void GUI_Playlist::btn_menu_pressed(){

	QPoint pos = QCursor::pos();
	pos.setY( pos.y() - 160 );
	_playlist_menu->exec(pos);
	btn_menu->setChecked(false);
}
