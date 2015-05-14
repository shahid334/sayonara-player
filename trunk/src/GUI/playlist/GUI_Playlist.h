/* GUI_Playlist.h */

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
 * GUI_Playlist.h
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */

#ifndef GUI_PLAYLIST_H_
#define GUI_PLAYLIST_H_

#include "GUI/ui_GUI_Playlist.h"
#include "GUI/playlist/PlaylistTabWidget.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include "GUI/shutdown_dialog/GUI_Shutdown.h"
#include "Playlist/PlaylistHandler.h"

#include "HelperStructs/MetaData/MetaData.h"
#include "HelperStructs/SayonaraClass.h"

#include <QTextEdit>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>

class PlaylistMenu : public QMenu, protected SayonaraClass {

	Q_OBJECT

signals:
	void sig_shutdown(bool);

private:
	QAction* _action_rep1;
	QAction* _action_repAll;
	QAction* _action_append;
	QAction* _action_dynamic;
	QAction* _action_shuffle;
	QAction* _action_gapless;
	QAction* _action_shutdown;
	QAction* _action_numbers;

private slots:

	void show_numbers(){
		_settings->set(Set::PL_ShowNumbers, _action_numbers->isChecked());
	}

	void plm_changed(){

		PlaylistMode plm = _settings->get(Set::PL_Mode);

		_action_append->setChecked(plm.append);
		_action_rep1->setChecked(plm.rep1);
		_action_repAll->setChecked(plm.repAll);
		_action_shuffle->setChecked(plm.shuffle);
		_action_dynamic->setChecked(plm.dynamic);
		_action_gapless->setChecked(plm.gapless);

		_action_rep1->setVisible(plm.ui_rep1);
		_action_append->setVisible(plm.ui_append);
		_action_repAll->setVisible(plm.ui_repAll);
		_action_dynamic->setVisible(plm.ui_dynamic);
		_action_shuffle->setVisible(plm.ui_shuffle);
		_action_gapless->setVisible(plm.ui_gapless);
	}

	void change_plm(){

		PlaylistMode plm = _settings->get(Set::PL_Mode);
		plm.rep1 = _action_rep1->isChecked();
		plm.repAll = _action_repAll->isChecked();
		plm.shuffle = _action_shuffle->isChecked();
		plm.append = _action_append->isChecked();
		plm.dynamic = _action_dynamic->isChecked();
		plm.gapless = _action_gapless->isChecked();

		_settings->set(Set::PL_Mode, plm);

	}

public:
	PlaylistMenu(QWidget* parent=0) :
		QMenu(parent),
		SayonaraClass()
	{
		/*_action_rep1 = new QAction(Helper::getIcon("rep_1"), tr("Repeat 1"), this);
		_action_repAll = new QAction(Helper::getIcon("rep_all"), tr("Repeat all"), this);
		_action_append = new QAction(Helper::getIcon("append"), tr("Append"), this);
		_action_shuffle = new QAction(Helper::getIcon("shuffle"), tr("Shuffle"), this);
		_action_dynamic = new QAction(Helper::getIcon("dynamic"), tr("Dynamic playback"), this);
		_action_gapless = new QAction(Helper::getIcon("gapless"), tr("Gapless playback"), this);
		_action_shutdown = new QAction(Helper::getIcon("shutdown"), tr("Shutdown"), this);*/

		_action_rep1 = new QAction(tr("Repeat 1"), this);
		_action_repAll = new QAction(tr("Repeat all"), this);
		_action_append = new QAction(tr("Append"), this);
		_action_shuffle = new QAction(tr("Shuffle"), this);
		_action_dynamic = new QAction(tr("Dynamic playback"), this);
		_action_gapless = new QAction(tr("Gapless playback"), this);
		_action_shutdown = new QAction(tr("Shutdown"), this);
		_action_numbers = new QAction(tr("Numbers"), this);

		QList<QAction*> actions;
		actions << _action_rep1
				<< _action_repAll
				<< _action_append
				<< _action_dynamic
				<< _action_shuffle
				<< _action_gapless
				<< addSeparator()
				<< _action_numbers
				<< addSeparator()
				<< _action_shutdown;




		for(QAction* action : actions){
			action->setCheckable(true);
			action->setVisible(true);
		}

		this->addActions(actions);


		PlaylistMode plm = _settings->get(Set::PL_Mode);

		_action_rep1->setChecked(plm.rep1);
		_action_append->setChecked(plm.append);
		_action_repAll->setChecked(plm.repAll);
		_action_dynamic->setChecked(plm.dynamic);
		_action_shuffle->setChecked(plm.shuffle);
		_action_gapless->setChecked(plm.gapless);
		_action_shutdown->setChecked(false);
		_action_numbers->setChecked(_settings->get(Set::PL_ShowNumbers));

		connect(_action_rep1, SIGNAL(triggered()), this, SLOT(change_plm()));
		connect(_action_repAll, SIGNAL(triggered()), this, SLOT(change_plm()));
		connect(_action_shuffle, SIGNAL(triggered()), this, SLOT(change_plm()));
		connect(_action_dynamic, SIGNAL(triggered()), this, SLOT(change_plm()));
		connect(_action_gapless, SIGNAL(triggered()), this, SLOT(change_plm()));
		connect(_action_shutdown, SIGNAL(triggered()), this, SLOT(change_plm()));
		connect(_action_numbers, SIGNAL(triggered()), this, SLOT(show_numbers()));
		connect(_action_shutdown, SIGNAL(triggered(bool)), this, SIGNAL(sig_shutdown(bool)));

		REGISTER_LISTENER(Set::PL_Mode, plm_changed);
	}

	void set_shutdown(bool b){
		_action_shutdown->setChecked(b);
	}

};



class GUI_Playlist : public SayonaraWidget, private Ui::Playlist_Window
{
	Q_OBJECT

public:
	GUI_Playlist(QWidget *parent);
	~GUI_Playlist();

	void dragEnterEvent(QDragEnterEvent* event);
	void dragLeaveEvent(QDragLeaveEvent* event);
	void dropEvent(QDropEvent* event);
	void dragMoveEvent(QDragMoveEvent* event);


public slots:

	void library_path_changed(QString);
	void set_playlist_type(PlaylistType playlist_type);


private slots:

	void btn_numbers_changed(bool);
	void btn_menu_pressed();

	void fill_playlist(const Playlist* pl);
	void track_changed(int row, int playlist_idx);

	void sel_changed(const MetaDataList&, const QList<int>&);
	void double_clicked(int);
	void clear_playlist_slot();
	void playlist_mode_changed();
	void rows_moved(const QList<int>&, int);

	void menu_info_clicked();
	void menu_edit_clicked();

	void metadata_dropped(const MetaDataList&, int);
	void rows_removed(const QList<int>&);

	void playlist_added(int idx, const QString& name);
	void playlist_closed(int idx);
	void playlist_idx_changed(int idx, bool temporary);

	void add_playlist_clicked();
	void tab_playlist_clicked(int);
	void close_playlist_clicked(int);

	void tab_save_playlist_clicked(int idx);
	void tab_save_playlist_as_clicked(int idx, const QString& str);
	void tab_delete_playlist_clicked(int idx);
	void tab_name_changed(int idx, const QString& name);
	void tab_clear_all_icons();

	void playstate_changed(PlayManager::PlayState state);

	void shutdown_toggled(bool b);
	void shutdown_closed();


	void _sl_change_small_playlist_items();
	void _sl_playlist_mode_changed();
	void _sl_numbers_changed();



private:
	PlayManager*			_play_manager;
	PlaylistMenu*			_playlist_menu;
	PlaylistHandler*		_playlist;
	GUI_InfoDialog*			_info_dialog;
	PlaylistMode			_playlist_mode;
	PlaylistType			_playlist_type;
	GUI_Shutdown*			_ui_shutdown;

	QList<PlaylistView*>	_playlist_views;
	int						_cur_playlist_idx;

	QVector<qint64>			_total_time;

	PlaylistView* get_view_by_idx(int idx);
	PlaylistView* get_current_view();

	void initPlaylistView(const PlaylistView* pl_view);

	void set_total_time_label(qint64 dur_ms=0);
	void check_dynamic_play_button();
	void prepare_info();
	int show_save_message_box(PlaylistDBInterface::SaveAsAnswer answer);


protected:
	void changeEvent(QEvent* e);
	void resizeEvent(QResizeEvent *e);
	void language_changed();
};



#endif /* GUI_PLAYLIST_H_ */
