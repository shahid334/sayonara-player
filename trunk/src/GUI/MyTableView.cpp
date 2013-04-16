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
#include "GUI/ContextMenu.h"

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
#include <QUrl>
#include <QLineEdit>
#include <QScrollBar>
#include <QFont>

#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"

#define VIEW_MODE_METADATA 0
#define VIEW_MODE_ALBUMS 1
#define VIEW_MODE_ARTISTS 2

bool _is_alphanumeric(int key){

    if(key >= 0x41 && key <= 0x5a) return true;
    if(key >= 0x30 && key <= 0x39) return true;
    return false;
}


MyTableView::MyTableView(QWidget* parent) : QTableView(parent) {
    _parent = parent;
    _qDrag = 0;
    _rc_header_menu = 0;

    _mimedata = new CustomMimeData();
    _edit = new QLineEdit(this);
    _edit->hide();

    this->connect(_edit, SIGNAL(textChanged(QString)), this, SLOT(edit_changed(QString)));

    rc_menu_init();

    _corner_widget = new QWidget(this);
    _corner_widget->hide();

    _view_mode = -1;

    QFont f = horizontalHeader()->font();
    f.setFamily("DejaVu Sans");
    f.setPixelSize(12);
    this->horizontalHeader()->setFont(f);

    connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_by_column(int)));
}

MyTableView::~MyTableView() {
    delete _rc_menu;
    delete _corner_widget;
    delete _edit;
}


void MyTableView::setModel(QAbstractItemModel * model){
    QTableView::setModel(model);

    _model = (LibraryItemModel*) model;
}

void MyTableView::mousePressEvent(QMouseEvent* event){

    QPoint pos_org = event->pos();
    QPoint pos = QWidget::mapToGlobal(pos_org);

    reset_edit();

    switch(event->button()){
    case Qt::LeftButton:
        if(event->pos().y() > _model->rowCount() * rowHeight(0)) {
            event->ignore();
            _drag = false;
            QList<int> lst;
            _model->set_selected(lst);
            break;
        }

        else {

            QTableView::mousePressEvent(event);
            _drag_pos = pos_org;
            _drag = true;
            //				emit sig_pressed(pos, indexAt(pos));
        }

        break;

    case Qt::RightButton:
        _drag = false;

        QTableView::mousePressEvent(event);
        pos.setY(pos.y() + 35);
        pos.setX(pos.x() + 10);
        rc_menu_show(pos);

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
    reset_edit();
    QPoint pos = event->pos();
    int distance =  abs(pos.x() - _drag_pos.x()) +	abs(pos.y() - _drag_pos.y());

    if (_drag && _qDrag && distance > 20) {
        emit sig_no_disc_menu();
        _qDrag->exec(Qt::CopyAction);
    }

}


void MyTableView::mouseReleaseEvent(QMouseEvent* event){
    reset_edit();

    switch (event->button()) {

    case Qt::LeftButton:
        if(_qDrag) {
            delete _qDrag;
            _qDrag = NULL;
        }

        QTableView::mouseReleaseEvent(event);
        event->accept();

        _drag = false;

        break;

    default:
        break;
    }
}


void MyTableView::set_mimedata(const MetaDataList& v_md, QString text, bool drop_entire_folder){

    if(_qDrag){
        delete _qDrag;
    }

    _mimedata = new CustomMimeData();

    QList<QUrl> urls;
    if(!drop_entire_folder){
        foreach(MetaData md, v_md){
            QUrl url(QString("file://") + md.filepath);
            urls << url;
        }
    }

    else{
        QStringList filenames;
        foreach(MetaData md, v_md)
            filenames << md.filepath;

        QStringList folders = Helper::extract_folders_of_files(filenames);
        foreach(QString folder, folders){
            QUrl url(QString("file://") + folder);
            urls << url;
        }
    }

    _mimedata->setMetaData(v_md);
    _mimedata->setText(text);
    _mimedata->setUrls(urls);

    _qDrag = new QDrag(this);
    _qDrag->setMimeData(_mimedata);


    connect(_qDrag, SIGNAL(destroyed()), this, SLOT(forbid_mimedata_destroyable()));

    _drag = true;
}

void MyTableView::forbid_mimedata_destroyable(){

    _qDrag = NULL;

}


void MyTableView::keyPressEvent(QKeyEvent* event){
    int key = event->key();
    QString text = event->text();

    Qt::KeyboardModifiers  modifiers = event->modifiers();

    if( (modifiers & Qt::ControlModifier) &&
            (key == Qt::Key_A) ){

        selectAll();
        calc_selections();

        emit sig_all_selected();
    }

    if(text.size() > 0 && text[0].isLetterOrNumber()){
        int sb_width = this->verticalScrollBar()->width();
        if(!this->verticalScrollBar()->isVisible()) sb_width = 0;

        int sb_height = this->horizontalScrollBar()->height();
        if(!this->horizontalScrollBar()->isVisible()) sb_height = 0;

        _edit->setGeometry(this->width() - (sb_width + 105), this->height() - (sb_height + 30), 100, 25);
        _edit->setFocus();
        _edit->setText(text);
        _edit->show();

    }

    else if(key == Qt::Key_Escape){
        reset_edit();
    }
}

void MyTableView::edit_changed(QString str){

    this->clearSelection();
    if(str.size() == 0) {
        reset_edit();
        calc_selections();
        return;
    }

    int line = _model->getFirstRowOf(str);
    this->scrollTo(_model->index(line, 0));
    this->selectRow(line);
    calc_selections();
}

void MyTableView::reset_edit(){
    _edit->setText("");
    _edit->hide();
    this->setFocus();
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


void MyTableView::force_selections() {

    QItemSelectionModel* sm = this->selectionModel();
    QItemSelection sel = sm->selection();
    int rows = _model->rowCount();
    QList<int> lst = _model->get_selected();

    for(int row=0; row<rows; row++){

        if(lst.contains(row)){
            this->selectRow(row);
            sel.merge(sm->selection(), QItemSelectionModel::Select);
        }
    }

    sm->clearSelection();
    sm->select(sel,QItemSelectionModel::Select);
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
    switch_sorters(_sort_order, h.get_asc_sortorder(), h.get_desc_sortorder());

    emit sig_sortorder_changed(_sort_order);

}



void MyTableView::rc_menu_init(){
    _rc_menu = new ContextMenu(this);
    _rc_menu->setup_entries(ENTRY_PLAY_NEXT | ENTRY_INFO | ENTRY_DELETE | ENTRY_EDIT);
}


void MyTableView::rc_menu_show(const QPoint& p){

    emit sig_no_disc_menu();
    connect(_rc_menu, SIGNAL(sig_edit_clicked()), this, SLOT(edit_clicked()));
    connect(_rc_menu, SIGNAL(sig_info_clicked()), this, SLOT(info_clicked()));
    connect(_rc_menu, SIGNAL(sig_delete_clicked()), this, SLOT(delete_clicked()));
    connect(_rc_menu, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next_clicked()));

    _rc_menu->exec(p);

    disconnect(_rc_menu, SIGNAL(sig_edit_clicked()), this, SLOT(edit_clicked()));
    disconnect(_rc_menu, SIGNAL(sig_info_clicked()), this, SLOT(info_clicked()));
    disconnect(_rc_menu, SIGNAL(sig_delete_clicked()), this, SLOT(delete_clicked()));
    disconnect(_rc_menu, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next_clicked()));
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


void MyTableView::set_table_headers(QList<ColumnHeader>& headers, Sort::SortOrder sorting){

    _table_headers = headers;

    for(int i=0; i<headers.size(); i++){

        if(headers[i].get_asc_sortorder() == sorting){
            this->horizontalHeader()->setSortIndicator(i, Qt::AscendingOrder);
            _sort_order = sorting;
            break;

        }

        else if(headers[i].get_desc_sortorder() == sorting){

            _sort_order = sorting;
            this->horizontalHeader()->setSortIndicator(i, Qt::DescendingOrder);
            break;

        }
    }
}


void MyTableView::rc_header_menu_init(QStringList& shown_cols){

    if(_rc_header_menu) delete _rc_header_menu;
    _rc_header_menu = new QMenu( this->horizontalHeader() );

    // in this moment all columns are still shown
    int col_idx = this->horizontalHeader()->sortIndicatorSection();
    Qt::SortOrder asc = this->horizontalHeader()->sortIndicatorOrder();


    int i =0;
    bool show_sorter = true;
    foreach(ColumnHeader header, _table_headers){
        QAction* action = new QAction(header.getTitle(), this);
        action->setCheckable(true);

        action->setEnabled(header.getSwitchable());

        if( !header.getSwitchable() ) {
            action->setChecked(true);
        }

        else {

            if(i < shown_cols.size()){
                action->setChecked(shown_cols[i] == "1");

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

    if(show_sorter){
	    this->horizontalHeader()->setSortIndicator(col_idx, asc);
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

    emit sig_columns_changed(lst);
    set_col_sizes();

    force_selections();
}



void MyTableView::set_col_sizes(){

    int altogether_width = 0;
    int desired_width = 0;
    int tolerance = 30;
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
    //target_width -= altogether_width;

    for(int i=0; i<n_cols; i++){
        int col = _model->calc_shown_col(i);
        int preferred_size = 0;


        ColumnHeader h = _table_headers[col];
        if(h.getSizeType() == COL_HEADER_SIZE_TYPE_REL){

            preferred_size = (h.get_preferred_size_rel() / altogether_percentage) * target_width;
        }

        else{
            preferred_size = h.get_preferred_size_abs();
        }

        this->setColumnWidth(i, preferred_size);
    }

    calc_corner_widget();
}


void MyTableView::fill_metadata(const MetaDataList& v_md){

    _view_mode = VIEW_MODE_METADATA;

    QList<int> lst;
    _model->set_selected(lst);

    _model->removeRows(0, _model->rowCount());
    _model->insertRows(0, v_md.size());

    QItemSelectionModel* sm = this->selectionModel();
    QItemSelection sel = sm->selection();

    for(uint row=0; row<v_md.size(); row++){
        MetaData md = v_md[row];

        if(md.is_lib_selected){
            this->selectRow(row);
            sel.merge(sm->selection(), QItemSelectionModel::Select);
        }

        QModelIndex idx = _model->index(row, 0);

        _model->setData(idx, md.toVariant(), Qt::EditRole);
    }

    sm->clearSelection();
    sm->select(sel,QItemSelectionModel::Select);

    calc_corner_widget();

}

void MyTableView::fill_albums(const AlbumList& albums){

    _view_mode = VIEW_MODE_ALBUMS;

    QList<int> lst;
    _model->set_selected(lst);

    _model->removeRows(0, _model->rowCount());
    _model->insertRows(0, albums.size()); // fake "all albums row"

    QModelIndex idx;
    int first_selected_album_row = -1;

    QItemSelectionModel* sm = this->selectionModel();
    QItemSelection sel = sm->selection();

    for(uint row=0; row < albums.size(); row++){
        Album album = albums[row];

        idx = _model->index(row, 1);

        if(album.is_lib_selected){
            if(first_selected_album_row == -1)
                first_selected_album_row = row;

            this->selectRow(row);
            sel.merge(sm->selection(), QItemSelectionModel::Select);
        }

        QVariant data = album.toVariant();
        _model->setData(idx, data, Qt::EditRole );
    }

    sm->clearSelection();
    sm->select(sel,QItemSelectionModel::Select);

    if(first_selected_album_row >= 0)
        this->scrollTo(_model->index(first_selected_album_row, 0), QTableView::PositionAtCenter);

    calc_corner_widget();
}


void MyTableView::fill_artists(const ArtistList& artists){

    _view_mode = VIEW_MODE_ARTISTS;
    QList<int> lst;
    _model->set_selected(lst);

    _model->removeRows(0, _model->rowCount());
    _model->insertRows(0, artists.size()); // fake "all albums row"

    QModelIndex idx;
    int first_selected_artist_row = -1;

    QItemSelectionModel* sm = this->selectionModel();
    QItemSelection sel = sm->selection();

    for(uint row=0; row < artists.size(); row++){
        Artist artist = artists[row];

        idx = _model->index(row, 1);

        if(artist.is_lib_selected){
            if(first_selected_artist_row == -1)
                first_selected_artist_row = row;

            this->selectRow(row);
            sel.merge(sm->selection(), QItemSelectionModel::Select);
        }

        QVariant data = artist.toVariant();
        _model->setData(idx, data, Qt::EditRole );
    }

    sm->clearSelection();
    sm->select(sel,QItemSelectionModel::Select);

    if(first_selected_artist_row >= 0)
        this->scrollTo(_model->index(first_selected_artist_row, 0), QTableView::PositionAtCenter);

    calc_corner_widget();
}


void MyTableView::calc_corner_widget(){

    if(!this->verticalScrollBar() || !this->verticalScrollBar()->isVisible() || !this->horizontalScrollBar()->isVisible()){
        this->setCornerWidget(NULL);
        _corner_widget->hide();
        return;
    }

    if(!this->cornerWidget()){
        this->setCornerWidget(_corner_widget);
        _corner_widget->show();

    }


    QPalette palette = _parent->palette();
    QColor bg = palette.color(QPalette::Normal, QPalette::Window);
    if(this->cornerWidget())
        this->cornerWidget()->setStyleSheet(QString("background: ") + bg.name() + ";");

}


void MyTableView::resizeEvent(QResizeEvent* event){
    event->ignore();
    QTableView::resizeEvent(event);
    calc_corner_widget();
}

