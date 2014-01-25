/* LibraryView.h */

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

#include "GUI/MyColumnHeader.h"
#include "GUI/ContextMenu.h"
#include "GUI/library/model/LibraryItemModel.h"
#include "GUI/MiniSearcher.h"
#include "GUI/SearchableWidget.h"
#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/Helper.h"

#include <QTableView>
#include <QPoint>
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QLineEdit>
#include <QDropEvent>


class LibraryView : public SearchableTableView{

    Q_OBJECT

signals:

    void sig_columns_changed(QStringList& );

    void sig_middle_button_clicked(const QPoint&);
    void sig_all_selected();
    void sig_info_clicked();
    void sig_edit_clicked();
    void sig_delete_clicked();
    void sig_play_next_clicked();
    void sig_append_clicked();
    void sig_sortorder_changed(Sort::SortOrder);

    void sig_no_disc_menu();
    void sig_tab_pressed(bool);
    void sig_import_files(const QStringList&);
    void sig_double_clicked(const QList<int>&);
    void sig_sel_changed(const QList<int>&);
    void sig_released();

private slots:
    void rc_header_menu_changed(bool b=true);
    void rc_menu_show(const QPoint&);
    void sort_by_column(int);
    void forbid_mimedata_destroyable();

    void edit_clicked();
    void info_clicked();
    void delete_clicked();
    void play_next_clicked();
    void append_clicked();


private:
    void rc_menu_init();
    void calc_corner_widget();


public:
    LibraryView(QWidget* parent=0);
    virtual ~LibraryView();

    void rc_header_menu_init(QStringList& lst);
    void set_mimedata(const MetaDataList& v_md, QString text, bool drop_entire_folder);
    void set_table_headers(QList<ColumnHeader>& headers, Sort::SortOrder sorting);

    void fill_metadata(const MetaDataList& v_md);
    void fill_albums(const AlbumList& v_albums);
    void fill_artists(const ArtistList& v_artists);

    void set_col_sizes();

    QList<int> get_selections();
    void force_selections();
    void set_skin(bool dark);

    using QTableView::setModel;
    virtual void setModel(LibraryItemModel* model);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent *event);
   void keyPressEvent(QKeyEvent* event);
    void resizeEvent(QResizeEvent* event);
    void dropEvent(QDropEvent* event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);


    void 	selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );

private:
    QWidget* 			_parent;
    QWidget*            _corner_widget;
    bool				_drag;
    QPoint				_drag_pos;
    QDrag*				_qDrag;
    QList<ColumnHeader> _table_headers;

    QMenu*              _rc_header_menu;
    QList<QAction*> 	_header_rc_actions;

    ContextMenu*        _rc_menu;

    Sort::SortOrder		_sort_order;
    CustomMimeData*		_mimedata;

    bool                _dark;
    bool                _sel_changed;

    LibraryItemModel*   _model;

    int get_min_selected();

    void goto_row(int row, bool select=false);

    /*// calc selections and insert into db
    // row = row of item in list
    // is selected
    // variant = converted to variant
    // selection in list
    // current first selected row
    // returns updated selected row
    int run_loop(int row, QVariant& variant, bool is_selected, QItemSelection& sel, int first_selected_row);*/

    // prepares model and returns the QItemSelection for the table
    QItemSelection reset_and_get_selection(int size);

    // selects according to selctions
    void select_and_scroll_to(int row);


};

#endif /* MYLISTVIEW_H_ */
