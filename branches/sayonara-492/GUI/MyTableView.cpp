/* MyTableView.cpp */

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
 * MyTableView.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: luke
 */

#include "GUI/MyTableView.h"
#include <QWidget>
#include <QTableView>
#include <QMouseEvent>
#include <QDebug>
#include <QMenu>
#include <QString>
#include <QStringList>
#include <QHeaderView>
#include <QAction>
#include <QIcon>



#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"



MyTableView::MyTableView(QWidget* parent) : QTableView(parent) {
	_parent = parent;
	_qDrag = 0;

	_mimedata_destroyable = false;
	_mimedata = 0;

	rc_menu_init();


	//connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_by_column(int)));

}

MyTableView::~MyTableView() {

	/*if(_qDrag) delete _qDrag;
	_qDrag = 0;*/

}


void MyTableView::setModel(QAbstractItemModel * model){
	QTableView::setModel(model);

	_model = (LibraryItemModel*) model;

}

void MyTableView::mousePressEvent(QMouseEvent* event){

	QPoint pos_org = event->pos();
	QPoint pos = QWidget::mapToGlobal(pos_org);

	switch(event->button()){
		case Qt::LeftButton:

            QTableView::mousePressEvent(event);

			if(event->pos().y() > this->model()->rowCount() * 20) {
				_drag = false;
				break;
			}

			else {
				_drag_pos = event->pos();
				_drag = true;
			}

			break;

		case Qt::RightButton:
			_drag = false;

			QTableView::mousePressEvent(event);
			pos.setY(pos.y() + 35);
			pos.setX(pos.x() + 10);
			rc_menu_show(pos);
			emit sig_context_menu_emitted(pos);
			break;

		case Qt::MidButton:
			_drag = false;

			QTableView::mousePressEvent(event);

			emit sig_middle_button_clicked(pos);
			break;

		default:
			_drag = false;
			break;
	}

}

void MyTableView::mouseMoveEvent(QMouseEvent* event){

	QPoint pos = event->pos();
	int distance =  abs(pos.x() - _drag_pos.x()) +	abs(pos.y() - _drag_pos.y());

	if (_drag && _qDrag && distance > 20) {
		_qDrag->exec(Qt::ActionMask);
	}

}


void MyTableView::mouseReleaseEvent(QMouseEvent* event){

	switch (event->button()) {

		case Qt::LeftButton:

            delete _qDrag->mimeData();
			QTableView::mouseReleaseEvent(event);
			event->accept();

			_drag = false;

			break;

		default: break;
	}
}


void MyTableView::set_mimedata(const MetaDataList& v_md, QString text){

	if(_mimedata_destroyable) delete _mimedata;
	_mimedata = new CustomMimeData();
	connect(_mimedata, SIGNAL(destroyed()), this, SLOT(forbid_mimedata_destroyable()));
	_mimedata_destroyable = true;

	_mimedata->setMetaData(v_md);
	_mimedata->setText(text);

    _qDrag = new QDrag(this);
	_qDrag->setMimeData(_mimedata);

	if (_mimedata) _drag = true;
	else _drag = false;
}

void MyTableView::forbid_mimedata_destroyable(){

	_mimedata_destroyable = false;
}


void MyTableView::keyPressEvent(QKeyEvent* event){
    int key = event->key();

    if(key != Qt::Key_A) return;

    Qt::KeyboardModifiers  modifiers = event->modifiers();

    if(modifiers & Qt::ControlModifier){
        selectAll();
        emit sig_all_selected();
    }
}


void MyTableView::set_table_headers(QList<ColumnHeader>& headers){

	_table_headers = headers;

}


QList<int> MyTableView::calc_selections(){

	QList<int> idx_list_int;

	QModelIndexList idx_list = this->selectionModel()->selectedRows();

	foreach(QModelIndex model_idx, idx_list){
		idx_list_int.push_back(model_idx.row());
	}

	_model->set_selected(idx_list_int);
	return idx_list_int;

}



template <typename T>
void switch_sorters(T& srcdst, T src1, T src2){
	if(srcdst == src1) srcdst = src2;
	else srcdst = src1;
}


void MyTableView::sort_by_column(int col){

	int idx_col = _model->calc_shown_col(col);

	if(idx_col >= _table_headers.size()) return;
	ColumnHeader h = _table_headers[idx_col];
	switch_sorters(_sort_order, h.get_asc_sortorder(), h.get_asc_sortorder());

	emit sig_sortorder_changed(_sort_order);

}



void MyTableView::rc_menu_init(){
	_info_action = new QAction(QIcon(Helper::getIconPath() + "info.png"), "Info", this);
	_edit_action = new QAction(QIcon(Helper::getIconPath() + "lyrics.png"), "Edit", this);
	_delete_action = new QAction(QIcon(Helper::getIconPath() + "delete.png"), "Delete", this);
	_play_next_action = new QAction(QIcon(Helper::getIconPath() + "fwd_orange.png"), "Play next", this);

	/// TODO: menu -> tableview
	_right_click_menu = new QMenu(this);
	_right_click_menu->addAction(_info_action);
	_right_click_menu->addAction(_edit_action);
	_right_click_menu->addAction(_delete_action);
	_right_click_menu->addAction(_play_next_action);
}


void MyTableView::rc_menu_show(const QPoint& p){
	connect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_clicked()));
	connect(_info_action, SIGNAL(triggered()), this, SLOT(info_clicked()));
	connect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_clicked()));
	connect(_play_next_action, SIGNAL(triggered()), this, SLOT(play_next_clicked()));

	this->_right_click_menu->exec(p);

	disconnect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_clicked()));
	disconnect(_info_action, SIGNAL(triggered()), this, SLOT(info_clicked()));
	disconnect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_clicked()));
	disconnect(_play_next_action, SIGNAL(triggered()), this, SLOT(play_next_clicked()));

}


void MyTableView::edit_clicked(){
	emit sig_edit_clicked();
}
void MyTableView::info_clicked(){
	emit sig_info_clicked();
}
void MyTableView::delete_clicked(){
	emit sig_delete_clicked();
}
void MyTableView::play_next_clicked(){
	emit sig_play_next_clicked();
}


void MyTableView::rc_header_menu_init(QStringList& shown_cols){

	_rc_header_menu = new QMenu( this->horizontalHeader() );

	//QStringList header_names = _model->get_header_names();


	int i =0;
	foreach(ColumnHeader header, _table_headers){
		QAction* action = new QAction(header.getTitle(), this);

		_header_rc_actions << action;


		if( header.getSwitchable() ) {
			action->setEnabled(false);
			action->setChecked(true);
		}

		if(i < shown_cols.size())
			action->setChecked(shown_cols[i] == "1");
		else
			action->setChecked(false);

		rc_header_menu_changed();

		connect(action, SIGNAL(toggled(bool)), this, SLOT(header_rc_menu_title_changed(bool)));
		this->horizontalHeader()->addAction(action);
		i++;
	}


	this->horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
}





void MyTableView::rc_header_menu_changed(bool b){

	Q_UNUSED(b);
	_model->removeColumns(0, _model->columnCount());

	int col_idx = 0;
	QStringList lst;
	foreach(QAction* action, _header_rc_actions){

		if(action->isChecked()){

			_model->insertColumn(col_idx);
			lst << "1";
		}

		else lst << "0";

		col_idx++;
	}

	set_col_sizes();
}



void MyTableView::set_col_sizes(){

	int altogether_width = 0;
	int tolerance = 20;
	double altogether_percentage = 0;
	int n_cols = _model->columnCount();


	for(int i=0; i<n_cols; i++){
		int col = _model->calc_shown_col(i);
		int preferred_size = 0;

		ColumnHeader h = _table_headers[col];
		if(h.getSizeType() == COL_HEADER_SIZE_TYPE_ABS){

			preferred_size = h.get_preferred_size_abs();
		}

		else{
			altogether_percentage += h.get_preferred_size_rel();
			continue;
		}

		altogether_width += preferred_size;

		this->setColumnWidth(i, preferred_size);
	}

	altogether_width += tolerance;

	int target_width = this->width();

	if(target_width < 600) {
		target_width = 600;
	}

	// width for percentage stuff
	target_width -= altogether_width;

	for(int i=0; i<n_cols; i++){
		int col = _model->calc_shown_col(i);
		int preferred_size = 0;


		ColumnHeader h = _table_headers[col];
		if(h.getSizeType() == COL_HEADER_SIZE_TYPE_REL){

			preferred_size = (h.get_preferred_size_rel() / altogether_percentage) * target_width;
		}

		else continue;

		this->setColumnWidth(i, preferred_size);
	}
}
