/* LibraryViewHeader.cpp */

/* Copyright (C) 2011-2014  Lucio Carreras
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

#include "GUI/library/view/LibraryView.h"
#include <QHeaderView>
#include <QMouseEvent>


template <typename T>
void switch_sorters(T& srcdst, T src1, T src2) {
    if(srcdst == src1) srcdst = src2;
    else srcdst = src1;
}


void LibraryView::sort_by_column(int col) {

    int idx_col = _model->calc_shown_col(col);

	if(idx_col >= _table_headers.size()) {
		return;
	}

    ColumnHeader h = _table_headers[idx_col];
	switch_sorters( _sort_order, h.get_asc_sortorder(), h.get_desc_sortorder() );

    emit sig_sortorder_changed(_sort_order);
}


void LibraryView::set_table_headers(QList<ColumnHeader>& headers, Sort::SortOrder sorting) {

    _table_headers = headers;

    for(int i=0; i<headers.size(); i++) {

        if(headers[i].get_asc_sortorder() == sorting) {
            this->horizontalHeader()->setSortIndicator(i, Qt::AscendingOrder);
            _sort_order = sorting;
            break;

        }

        else if(headers[i].get_desc_sortorder() == sorting) {

            _sort_order = sorting;
            this->horizontalHeader()->setSortIndicator(i, Qt::DescendingOrder);
            break;

        }
    }
}


void LibraryView::rc_header_menu_init(QList<int>& shown_cols) {

    if(_rc_header_menu) delete _rc_header_menu;
    _rc_header_menu = new QMenu( this->horizontalHeader() );

    // in this moment all columns are still shown
    int col_idx = this->horizontalHeader()->sortIndicatorSection();
    Qt::SortOrder asc = this->horizontalHeader()->sortIndicatorOrder();


    int i =0;
    bool show_sorter = true;
	for(const ColumnHeader& header : _table_headers) {
        QAction* action = new QAction(header.getTitle(), this);
        action->setCheckable(true);

        action->setEnabled(header.getSwitchable());

        if( !header.getSwitchable() ) {
            action->setChecked(true);
        }

        else {

            if(i < shown_cols.size()) {
				action->setChecked(shown_cols[i] == 1);

                // where should we put the sorters?
                // if a few columns are missing before the origin position,
                // the index of the sorted column has to be decreased
                if(i<col_idx && !action->isChecked()) col_idx --;
                else if(i == col_idx && !action->isChecked()) show_sorter = false;
            }

            else{
                action->setChecked(false);
            }
        }

        connect(action, SIGNAL(toggled(bool)), this, SLOT(rc_header_menu_changed(bool)));

        _header_rc_actions << action;
        this->horizontalHeader()->addAction(action);
        i++;
    }

    rc_header_menu_changed();

    if(show_sorter) {
        this->horizontalHeader()->setSortIndicator(col_idx, asc);
    }

    this->horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
}



void LibraryView::rc_header_menu_changed(bool b) {

    Q_UNUSED(b);

    QList<int> sel_list = get_selections();

    _model->removeColumns(0, _model->columnCount());

    int col_idx = 0;
	QList<int> lst;
	for(const QAction* action : _header_rc_actions) {

        if(action->isChecked()) {
            _model->insertColumn(col_idx);
			lst << 1;
        }

		else lst << 0;

        col_idx++;
    }

    emit sig_columns_changed(lst);
    set_col_sizes();

	for(const int& row : sel_list){
		this->selectRow(row);
	}
}


void LibraryView::set_col_sizes() {

    int altogether_width = 0;
    int desired_width = 0;
    int tolerance = 30;
    double altogether_percentage = 0;
    int n_cols = _model->columnCount();


    for(int i=0; i<n_cols; i++) {
        int col = _model->calc_shown_col(i);
        int preferred_size = 0;

        ColumnHeader h = _table_headers[col];
		if(h.getSizeType() == ColHeaderSizeAbs) {

            preferred_size = h.get_preferred_size_abs();
        }

        else{

            altogether_percentage += h.get_preferred_size_rel();
            desired_width += h.get_preferred_size_abs();
        }

        altogether_width += preferred_size;
    }

    altogether_width += tolerance;

    int target_width = this->width() - altogether_width;


    if(target_width < desired_width) {
        target_width = desired_width;
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }

    else{
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    // width for percentage stuff
    for(int i=0; i<n_cols; i++) {
        int col = _model->calc_shown_col(i);
        int preferred_size = 0;


        ColumnHeader h = _table_headers[col];
		if(h.getSizeType() == ColHeaderSizeRel) {

            preferred_size = (h.get_preferred_size_rel() / altogether_percentage) * target_width;
        }

        else{
            preferred_size = h.get_preferred_size_abs();
        }

        this->setColumnWidth(i, preferred_size);
    }
}
// header end
