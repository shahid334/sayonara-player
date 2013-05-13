/* MyListView.h */

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
 * MyListView.h
 *
 *  Created on: Jun 27, 2011
 *      Author: luke
 */

#ifndef MYLISTVIEW_H_
#define MYLISTVIEW_H_

#include "HelperStructs/MetaData.h"
#include "HelperStructs/CustomMimeData.h"
#include "GUI/playlist/PlaylistItemModel.h"
#include "GUI/playlist/PlaylistItemDelegateInterface.h"
#include "GUI/ContextMenu.h"


#include <QListView>
#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QPoint>
#include <QDrag>
#include <QList>
#include <QMenu>
#include <QModelIndex>


class MyListView :public QListView{

	Q_OBJECT

	signals:
		void context_menu_emitted(const QPoint&);
        void sig_metadata_dropped(const MetaDataList&, int);
        void sig_rows_removed(const QList<int> &, bool);
        void sig_info_clicked();
        void sig_remove_clicked();
        void sig_edit_clicked();
        void sig_selection_changed(MetaDataList&);
        void sig_double_clicked(int);

    private slots:
        void forbid_mimedata_destroyable();
        void edit_clicked();
        void info_clicked();
        void remove_clicked();

        void row_pressed(const QModelIndex&);
        void row_double_clicked(const QModelIndex&);
        void row_released(const QModelIndex&);

	public:
		MyListView(QWidget* parent=0);
		virtual ~MyListView();

        void set_context_menu_actions(int actions);
        void set_drag_enabled(bool b);
        void set_mimedata(MetaDataList& v_md, QString text);
        void clear();
        void fill(MetaDataList& v_metadata, int cur_play_idx);
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

    private:

        void clear_selection();
        QList<int> calc_selections();
        void select_all();
        int calc_dd_line(QPoint pos);
        void set_delegate_max_width(int n_items);

        void clear_drag_lines(int row);
        void dragLeaveEvent(QDragLeaveEvent* event);
        void dragEnterEvent(QDragEnterEvent* event);
        void dragMoveEvent(QDragMoveEvent* event);
        void dropEvent(QDropEvent* event);
        void handle_drop(QDropEvent* event, bool from_outside=false);

        void remove_cur_selected_rows(bool select_next_row=true);
        void select_rows(QList<int> lst);

        void init_rc_menu();




	private:
        QWidget*        _parent;

        bool            _drag;
        bool            _drag_allowed;
        bool            _inner_drag_drop;

        QPoint          _drag_pos;
        QDrag*          _qDrag;
        int             _last_known_drag_row;

        QList<int>      _cur_selected_rows;


        ContextMenu* 	_rc_menu;

        PlaylistItemModel*              _model;
        PlaylistItemDelegateInterface* 	_delegate;

};

#endif /* MYLISTVIEW_H_ */
