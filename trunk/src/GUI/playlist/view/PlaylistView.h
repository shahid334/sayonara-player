/* PlaylistView.h */

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
 * PlaylistView.h
 *
 *  Created on: Jun 27, 2011
 *      Author: luke
 */

#ifndef PLAYLISTVIEW_H_
#define PLAYLISTVIEW_H_

#include "HelperStructs/MetaData/MetaData.h"
#include "HelperStructs/CustomMimeData.h"
#include "GUI/playlist/model/PlaylistItemModel.h"
#include "GUI/playlist/delegate/PlaylistItemDelegate.h"
#include "GUI/ContextMenu.h"
#include "GUI/SearchableWidget.h"
#include "GUI/ui_GUI_PlaylistView.h"

#include <QPoint>
#include <QDrag>
#include <QList>
#include <QMenu>
#include <QModelIndex>
#include <QMouseEvent>



class PlaylistView : public SearchableListView, private Ui::GUI_PlaylistView{

	Q_OBJECT

	signals:
		void context_menu_emitted(const QPoint&);
        void sig_metadata_dropped(const MetaDataList&, int);
		void sig_rows_removed(const QList<int>&);
        void sig_rows_moved(const QList<int>&, int);
        void sig_info_clicked();
        void sig_remove_clicked();
        void sig_edit_clicked();
        void sig_sel_changed(const MetaDataList&, const QList<int>&);
        void sig_double_clicked(int);
        void sig_no_focus();


    private slots:
        void forbid_mimedata_destroyable();
        void edit_clicked();
        void info_clicked();
        void remove_clicked();

        void row_pressed(const QModelIndex&);
        void row_double_clicked(const QModelIndex&);
        void row_released(const QModelIndex&);

	public:
        PlaylistView(QWidget* parent=0);
        virtual ~PlaylistView();


		void set_playlist_type(PlaylistType type);
		PlaylistType get_playlist_type();
        void set_context_menu_actions(int actions);
        void set_drag_enabled(bool b);
        void set_mimedata(MetaDataList& v_md, QString text);
        void clear();
		void fill(const MetaDataList& v_metadata);
        void scrollUp();
        void scrollDown();
        void dropEventFromOutside(QDropEvent* event);
        void set_current_track(int row);
        int get_num_rows();
        void show_big_items(bool);



	protected:
        // overloaded stuff
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
        void keyPressEvent(QKeyEvent *event);
        void resizeEvent(QResizeEvent *e);
        void selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );

        // SearchableListView
        virtual void select_rows(QList<int> lst);
        virtual void select_row(int row);


    private:

        void clear_selection();
        QList<int> get_selections();
        void select_all();
        int calc_dd_line(QPoint pos);
        void set_delegate_max_width(int n_items);

        void clear_drag_lines(int row);
        void dragLeaveEvent(QDragLeaveEvent* event);
        void dragEnterEvent(QDragEnterEvent* event);
        void dragMoveEvent(QDragMoveEvent* event);
        void dropEvent(QDropEvent* event);
        void handle_drop(QDropEvent* event, bool from_outside=false);

		void remove_cur_selected_rows();


        void goto_row(int row);
        int get_min_selected();

        void init_rc_menu();




	private:

        QWidget*        _parent;
		PlaylistType	_playlist_type;

        bool            _drag;
        bool            _drag_allowed;
        bool            _inner_drag_drop;

        QPoint          _drag_pos;
        QDrag*          _qDrag;
        int             _last_known_drag_row;
        bool            _ignore_selection_changes;

        QList<int>      _cur_selected_rows;


        ContextMenu* 	_rc_menu;

		PlaylistItemModel*		_model;
		PlaylistItemDelegate*	_delegate;
		bool					_sel_changed;

};

#endif /* PlaylistView_H_ */
