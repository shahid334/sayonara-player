/* MyTableView.h */

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
 *  Created on: Jun 26, 2011
 *      Author: luke
 */

#ifndef MYTABLEVIEW_H_
#define MYTABLEVIEW_H_

#include <QTableView>
#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QPoint>
#include <QKeyEvent>
#include <QList>
#include <QString>
#include <QMenu>
#include <QAction>



#include "GUI/MyColumnHeader.h"
#include "GUI/library/models/LibraryItemModel.h"
#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/globals.h"



class MyTableView : public QTableView{

	Q_OBJECT

	signals:

        void sig_columns_changed(QStringList& );

        void sig_middle_button_clicked(const QPoint&);
        void sig_all_selected();
        void sig_info_clicked();
        void sig_edit_clicked();
        void sig_delete_clicked();
        void sig_play_next_clicked();

        void sig_sortorder_changed(Sort::SortOrder);

    private slots:
        void rc_header_menu_changed(bool b=true);
        void rc_menu_show(const QPoint&);
        void sort_by_column(int);
        void forbid_mimedata_destroyable();

        void edit_clicked();
        void info_clicked();
        void delete_clicked();
        void play_next_clicked();


	private:
        void rc_menu_init();


	public:
		MyTableView(QWidget* parent=0);
		virtual ~MyTableView();

        void rc_header_menu_init(QStringList& lst);
		void set_mimedata(const MetaDataList& v_md, QString text);
		void set_table_headers(QList<ColumnHeader>& headers);
		void setModel(QAbstractItemModel * model);


        void fill_metadata(const MetaDataList& v_md);
        void fill_albums(const AlbumList& v_albums);
        void fill_artists(const ArtistList& v_artists);

        void set_col_sizes();

		QList<int> calc_selections();



	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
        void keyPressEvent(QKeyEvent* event);


	private:
		QWidget* 			_parent;
		bool				_drag;
		QPoint				_drag_pos;
		QDrag*				_qDrag;
		QList<ColumnHeader> _table_headers;


		QMenu*				_rc_header_menu;
		QMenu* 				_right_click_menu;
		QAction* 			_info_action;
		QAction* 			_edit_action;
		QAction* 			_delete_action;
		QAction*			_play_next_action;


	    QList<QAction*> 	_header_rc_actions;

		LibraryItemModel* 	_model;
		Sort::SortOrder		_sort_order;
		CustomMimeData*		_mimedata;

};

#endif /* MYLISTVIEW_H_ */
