/* MyListView.cpp */

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
 * MyListView.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: luke
 */
#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/Helper.h"

#include "GUI/ContextMenu.h"
#include "GUI/MyListView.h"
#include "GUI/playlist/PlaylistItemDelegate.h"
#include "GUI/playlist/PlaylistItemDelegateSmall.h"
#include "GUI/playlist/PlaylistItemDelegateInterface.h"

#include <QWidget>
#include <QListView>
#include <QMouseEvent>
#include <QDebug>
#include <QUrl>
#include <QMenu>
#include <QModelIndex>
#include <QDebug>



MyListView::MyListView(QWidget* parent) : QListView(parent) {

    _drag_allowed = true;
    _inner_drag_drop = false;
	_parent = parent;
    _qDrag = 0;
    _last_known_drag_row = -1;
    _model = new PlaylistItemModel(this);
    _delegate = new PlaylistItemDelegateSmall(this);
    _rc_menu = 0;

    this->setModel(_model);
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setSelectionRectVisible(true);
    this->setAlternatingRowColors(true);
    this->setMovement(QListView::Free);



    connect(this, SIGNAL(pressed(const QModelIndex&)), this, SLOT(row_pressed(const QModelIndex&)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(row_double_clicked(const QModelIndex&)));
    connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(row_released(const QModelIndex&)));
}

MyListView::~MyListView() {

    delete _rc_menu;
    delete _model;
}


void MyListView::mousePressEvent(QMouseEvent* event) {

	QPoint pos_org = event->pos();
	QPoint pos = QWidget::mapToGlobal(pos_org);

	switch (event->button()) {

    case Qt::LeftButton:
        if(!_drag_allowed) break;

		QListView::mousePressEvent(event);

		if ((this->model()->rowCount()) * 33 > event->pos().y())
			_drag_pos = event->pos();

		else {
			_drag_pos.setY(-10);
			_drag = false;
		}

		break;

	case Qt::RightButton:
		_drag = false;

		QListView::mousePressEvent(event);
		pos.setY(pos.y());
		pos.setX(pos.x() + 10);


        _rc_menu->exec(pos);

        break;

	default:
		_drag = false;
		break;
	}
}


void MyListView::mouseMoveEvent(QMouseEvent* event) {

	QPoint pos = event->pos();
	int distance =  abs(pos.x() - _drag_pos.x()) +	abs(pos.y() - _drag_pos.y());

    if (_drag && _qDrag && distance > 10 && _drag_allowed) {
        _qDrag->exec(Qt::CopyAction);
	}
}


void MyListView::mouseReleaseEvent(QMouseEvent* event) {

	switch (event->button()) {

		case Qt::LeftButton:

            if(_qDrag) {
               delete _qDrag;
                _qDrag = NULL;
            }

			QListView::mouseReleaseEvent(event);
			event->accept();

			_drag = false;
			break;

		default: break;
	}
}

void MyListView::keyPressEvent(QKeyEvent* event){

    int key = event->key();

    Qt::KeyboardModifiers  modifiers = event->modifiers();
    qDebug() << "Key press event";

    if( (modifiers & Qt::ControlModifier) &&
        (key == Qt::Key_A) ){

        select_all();
    }

    if( key == Qt::Key_Delete){
        remove_cur_selected_rows();
    }


}

void MyListView::init_rc_menu(){
    _rc_menu = new ContextMenu(this);

    connect(_rc_menu, SIGNAL(sig_info_clicked()), this, SLOT(info_clicked()));
    connect(_rc_menu, SIGNAL(sig_edit_clicked()), this, SLOT(edit_clicked()));
    connect(_rc_menu, SIGNAL(sig_remove_clicked()), this, SLOT(remove_clicked()));
}

void MyListView::set_context_menu_actions(int actions){
    if(!_rc_menu) init_rc_menu();
    _rc_menu->setup_entries(actions);
}


void MyListView::set_mimedata(MetaDataList& v_md, QString text) {

    if(!_drag_allowed) return;
    if(_qDrag) delete _qDrag;

    CustomMimeData* mimedata = new CustomMimeData();

    QList<QUrl> urls;
    foreach(MetaData md, v_md){
        QUrl url(QString("file://") + md.filepath);
        urls << url;
    }

    mimedata->setMetaData(v_md);
    mimedata->setText(text);
    mimedata->setUrls(urls);

    _qDrag = new QDrag(this);
    _qDrag->setMimeData(mimedata);

    connect(_qDrag, SIGNAL(destroyed()), this, SLOT(forbid_mimedata_destroyable()));

    _drag = true;
}

void MyListView::forbid_mimedata_destroyable(){
    _qDrag = NULL;
}

void MyListView::set_drag_enabled(bool b){
    _drag_allowed = b;
}


int MyListView::get_num_rows(){
    return _model->rowCount();
}

void MyListView::set_current_track(int row){

    for(int i=0; i<_model->rowCount(); i++){
        QModelIndex idx = _model->index(i);
        MetaData md;
        QVariant v = _model->data(idx, Qt::WhatsThisRole);
        if(!MetaData::fromVariant(v, md)) continue;
        md.pl_playing = (row == i);

        _model->setData(idx, md.toVariant(), Qt::EditRole);
    }

    QModelIndex new_idx = _model->index(row);
    scrollTo(new_idx,  QListView::EnsureVisible);
}


void MyListView::edit_clicked(){
    emit sig_edit_clicked();
}


void MyListView::info_clicked(){
    emit sig_info_clicked();
}

void MyListView::remove_clicked(){
    qDebug() << "Remove clicked" << _cur_selected_rows;
    remove_cur_selected_rows();
}




void MyListView::clear(){
    clear_selection();
    _model->removeRows(0, _model->rowCount());
}


void MyListView::fill(MetaDataList &v_metadata, int cur_play_idx){


    _model->removeRows(0, _model->rowCount());
    if(v_metadata.size() == 0) return;

    _model->insertRows(0, v_metadata.size());

    int idx = 0;
    _cur_selected_rows.clear();

    QModelIndex idx_cur_playing = _model->index(0);
    foreach(MetaData md, v_metadata){

        QModelIndex model_idx = _model->index(idx, 0);
        md.pl_playing = (cur_play_idx == idx);
        if(md.pl_playing) idx_cur_playing = model_idx;

        _model->setData(model_idx, md.toVariant(), Qt::EditRole);
        if(md.pl_selected)
            _cur_selected_rows << idx;

        idx++;
    }

    qDebug() << "Cur selected = " << _cur_selected_rows;

    _model->set_selected(_cur_selected_rows);
    this->select_rows(_cur_selected_rows);

    qDebug() << "Cur selected = " << _cur_selected_rows;
    this->scrollTo(idx_cur_playing, QListView::EnsureVisible);

}

void MyListView::row_pressed(const QModelIndex& idx){

    QList<int> selected_rows = calc_selections();
    _inner_drag_drop = true;

    MetaDataList v_md;
    foreach(int row, selected_rows){
        QVariant mdvariant = _model->data(_model->index(row), Qt::WhatsThisRole);

        MetaData md;

        if(!MetaData::fromVariant(mdvariant, md)) continue;

        v_md.push_back(md);
    }

    set_mimedata(v_md, "tracks");
    emit sig_selection_changed(v_md);
}

void MyListView::row_released(const QModelIndex& idx){
    calc_selections();
    _inner_drag_drop = false;
}

void MyListView::row_double_clicked(const QModelIndex& idx){
    _inner_drag_drop = false;

    if(idx.isValid()) emit sig_double_clicked(idx.row());
}

void MyListView::clear_selection(){

    MetaDataList v_md;
    this->selectionModel()->clearSelection();
    this->clearSelection();
    calc_selections();

}

void MyListView::select_rows(QList<int> lst){

    QItemSelectionModel* sm = this->selectionModel();
    QItemSelection sel;


    foreach(int row, lst){
        QModelIndex idx = _model->index(row);

        sm->select(idx, QItemSelectionModel::Select);
        sel.merge(sm->selection(), QItemSelectionModel::Select);
    }

    sm->clearSelection();
    sm->select(sel,QItemSelectionModel::Select);

    _cur_selected_rows = calc_selections();

}

void MyListView::select_all(){
    selectAll();
    QList<int> list =  calc_selections();
}

QList<int> MyListView::calc_selections(){

    QList<int> selections;

    QModelIndexList idx_list = this->selectionModel()->selectedRows();

    foreach(QModelIndex model_idx, idx_list){
        selections.push_back(model_idx.row());
    }

    _model->set_selected(selections);

    _cur_selected_rows = selections;

    return selections;
}


// remove the black line under the titles
void  MyListView::clear_drag_lines(int row){

    for(int i=row-3; i<=row+3; i++){

        QModelIndex idx = _model->index(i, 0);
        if(!idx.isValid() || idx.row() < 0 || idx.row() >= _model->rowCount())
            continue;

        QVariant mdVariant =_model->data(idx, Qt::WhatsThisRole);
        MetaData md;
        if(MetaData::fromVariant(mdVariant, md)){

            md.pl_dragged = false;
            _model->setData(idx, md.toVariant(), Qt::EditRole);
        }

    }
}



int MyListView::calc_dd_line(QPoint pos){

    if(pos.y() < 0) {
        return -1;
    }
    int row = this->indexAt(pos).row();

    if(row <= -1) row = _model->rowCount()-1;
    return row;
}


// the drag comes, if there's data --> accept it
void MyListView::dragEnterEvent(QDragEnterEvent* event){
    event->accept();
}

void MyListView::dragMoveEvent(QDragMoveEvent* event){

    if( !event->mimeData() )  return;
    event->accept();

    int row = calc_dd_line(event->pos() );

    _last_known_drag_row = row;
    clear_drag_lines(row);

    // paint line
    QModelIndex cur_idx = _model->index(row, 0);
    QVariant mdVariant = _model->data(cur_idx, Qt::WhatsThisRole);
    MetaData md;
    if(!MetaData::fromVariant(mdVariant, md)) return;

    md.pl_dragged = true;
    _model->setData(cur_idx, md.toVariant(), Qt::EditRole);
}


// we start the drag action, all lines has to be cleared
void MyListView::dragLeaveEvent(QDragLeaveEvent* event){
    event->accept();

    clear_drag_lines(_last_known_drag_row);
}


void MyListView::dropEventFromOutside(QDropEvent* event){
    if(event->pos().y() < this->y())
        handle_drop(event, true);

}

// finally drop it
void MyListView::dropEvent(QDropEvent* event){


    event->accept();

    if(!event->mimeData()) return;
    handle_drop(event, false);

}

void MyListView::handle_drop(QDropEvent* event, bool from_outside){


    QList<int> affected_rows;

    QPoint pos = event->pos();
    if(from_outside){
        pos.setY(pos.y() - this->y());
    }

    // where did i drop?
    int row = calc_dd_line(pos);

    if(_inner_drag_drop){
        _inner_drag_drop = false;
        if( _cur_selected_rows.contains(row) ){
            event->ignore();
            clear_drag_lines(row);
            return;
        }

        if(_cur_selected_rows.first() < row ) {
            row -= _cur_selected_rows.size();
        }

        remove_cur_selected_rows();
    }

    const CustomMimeData* d = (const CustomMimeData*) event->mimeData();

    MetaDataList v_metadata;

    QString text = "";
    if(d->hasText()) text = d->text();

    // extern
    if( d->hasUrls() && text.compare("tracks", Qt::CaseInsensitive) ){

        QStringList filelist;
        foreach(QUrl url, d->urls()){
                QString path;
                QString url_str = url.toString();
                path =  url_str.right(url_str.length() - 7).trimmed();
                path = path.replace("%20", " ");
                filelist.push_back(path);

        } // end foreach

        CDirectoryReader reader;
        reader.setFilter(Helper::get_soundfile_extensions());

        reader.getMetadataFromFileList(filelist, v_metadata);

        if(v_metadata.size() == 0) return;
    }

    else if(d->hasHtml()){}
    else if(d->hasImage()){}

    else if(d->hasText() && d->hasMetaData()){

        uint sz = d->getMetaData(v_metadata);
        if(sz == 0) return;

    }

    else if(d->hasText()){}
    else {}

    for(uint i=0; i<v_metadata.size(); i++){
        affected_rows << i + row + 1;
    }

    emit sig_metadata_dropped(v_metadata, row);
}


void MyListView::scrollUp(){
    QPoint p(5, 5);
    int cur_row = this->indexAt(p).row();
    if(cur_row <= 0) return;

    this->scrollTo(_model->index(cur_row - 1));
}

void MyListView::scrollDown(){
    QPoint p(5, this->y() + this->height() - 5);
    int cur_row = this->indexAt(p).row();
    if(cur_row <= 0) return;

    this->scrollTo(_model->index(cur_row - 1));
}

void MyListView::remove_cur_selected_rows(){
    emit sig_rows_removed(_cur_selected_rows);
}


void MyListView::show_big_items(bool big){
    if(!big)
        _delegate = new PlaylistItemDelegateSmall(this);
    else
        _delegate = new PlaylistItemDelegate(this);

    this->setItemDelegate(_delegate);
    this->reset();
}


