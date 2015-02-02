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
#include "GUI/library/RatingLabel.h"
#include "GUI/SearchableWidget.h"
#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/Helper.h"

#include <QTableView>
#include <QPoint>
#include <QMenu>
#include <QAction>
#include <QLineEdit>
#include <QDropEvent>
#include <QKeyEvent>

class LibraryView : public SearchableTableView {

    Q_OBJECT

signals:

	void sig_columns_changed(QList<int>& );

    void sig_middle_button_clicked(const QPoint&);
    void sig_all_selected();
    void sig_info_clicked();
    void sig_edit_clicked();
    void sig_delete_clicked();
    void sig_play_next_clicked();
    void sig_append_clicked();
    void sig_sortorder_changed(Sort::SortOrder);

    void sig_no_disc_menu();
    void sig_import_files(const QStringList&);
    void sig_double_clicked(const QList<int>&);
    void sig_sel_changed(const QList<int>&);

protected slots:
	virtual void rc_header_menu_changed(bool b=true);
	virtual void rc_menu_show(const QPoint&);
	virtual void sort_by_column(int);
	virtual void drag_deleted();

	virtual void edit_clicked();
	virtual void info_clicked();
	virtual void delete_clicked();
	virtual void play_next_clicked();
	virtual void append_clicked();

	virtual void editorDestroyed(QObject* editor=0);





public:
    LibraryView(QWidget* parent=0);
    virtual ~LibraryView();

	virtual void rc_header_menu_init(QList<int>& lst);
	virtual void set_mimedata(const MetaDataList& v_md, QString text, bool drop_entire_folder);
	virtual void set_table_headers(QList<ColumnHeader>& headers, Sort::SortOrder sorting);

	template < class TList, class T>
	void fill(const TList& input_data);

	virtual void set_editor(RatingLabel* editor);

	virtual void set_col_sizes();

	virtual QList<int> get_selections();

    using QTableView::setModel;
    virtual void setModel(LibraryItemModel* model);

protected:
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void dropEvent(QDropEvent* event);
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dragMoveEvent(QDragMoveEvent *event);

	virtual void selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );
	virtual void rc_menu_init();


protected:

	bool				_drag;
    QPoint				_drag_pos;
    QDrag*				_qDrag;

	QMenu*              _rc_header_menu;
    QList<ColumnHeader> _table_headers;
    QList<QAction*> 	_header_rc_actions;

    ContextMenu*        _rc_menu;

    Sort::SortOrder		_sort_order;
    CustomMimeData*		_mimedata;

    LibraryItemModel*   _model;
    RatingLabel*        _editor;
    bool                _cur_filling;

	virtual int get_min_selected();
};

#endif /* MYLISTVIEW_H_ */
