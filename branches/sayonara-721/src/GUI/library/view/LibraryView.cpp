/* LibraryView.cpp */

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
 * LibraryView.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: luke
 */

#include "GUI/library/view/LibraryView.h"
#include "GUI/ContextMenu.h"
#include "HelperStructs/CustomMimeData.h"


#include <QTableView>
#include <QMessageBox>
#include <QMouseEvent>
#include <QDebug>
#include <QStringList>
#include <QHeaderView>
#include <QAction>
#include <QIcon>
#include <QUrl>
#include <QLineEdit>
#include <QScrollBar>
#include <QFont>
#include <QMimeData>



#define VIEW_MODE_METADATA 0
#define VIEW_MODE_ALBUMS 1
#define VIEW_MODE_ARTISTS 2

bool _is_alphanumeric(int key){

    if(key >= 0x41 && key <= 0x5a) return true;
    if(key >= 0x30 && key <= 0x39) return true;
    return false;
}


LibraryView::LibraryView(QWidget* parent) : QTableView(parent) {
    _parent = parent;
    _qDrag = 0;
    _rc_header_menu = 0;
    _dark = true;

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
    setAcceptDrops(true);
}
LibraryView::~LibraryView() {
    delete _rc_menu;
    delete _corner_widget;
    delete _edit;
}
void LibraryView::setModel(QAbstractItemModel * model){
    QTableView::setModel(model);

    _model = (LibraryItemModel*) model;
}


void LibraryView::mousePressEvent(QMouseEvent* event){

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
            this->clearSelection();
            this->selectionModel()->clearSelection();

            break;
        }

        else {
            _sel_changed = false;
            QTableView::mousePressEvent(event);
            if(!_sel_changed){
                QItemSelection sel, desel;
                selectionChanged(sel, desel);
            }
            _drag_pos = pos_org;
            _drag = true;
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

void LibraryView::mouseMoveEvent(QMouseEvent* event){
    reset_edit();
    QPoint pos = event->pos();
    int distance =  abs(pos.x() - _drag_pos.x()) +	abs(pos.y() - _drag_pos.y());

    if (_drag && _qDrag && distance > 20) {
        emit sig_no_disc_menu();
        _qDrag->exec(Qt::CopyAction);
    }

}


void LibraryView::mouseReleaseEvent(QMouseEvent* event){
    reset_edit();

    switch (event->button()) {

    case Qt::LeftButton:

        QTableView::mouseReleaseEvent(event);
        event->accept();

        _drag = false;

        break;

    default:
        break;
    }
}


void LibraryView::set_mimedata(const MetaDataList& v_md, QString text, bool drop_entire_folder){

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

void LibraryView::forbid_mimedata_destroyable(){

    _qDrag = NULL;
}

int LibraryView::get_min_selected(){

    QList<int> selections = _model->get_selected();
    if(selections.size() == 0) return 0;
    int min = 10000;
    foreach(int i, selections){
        if(i < min) min = i;
    }
    return min;
}


int LibraryView::get_max_selected(){

    QList<int> selections = _model->get_selected();
    int max = -10000;
    if(selections.size() == 0) return _model->rowCount() - 1;
    foreach(int i, selections){
        if(i > max) max = i;
    }
    return max;
}


void LibraryView::goto_row(int row, bool select){

    if(_model->rowCount() == 0) return;

    if( row < 0 ) row = 0;
    else if( row > _model->rowCount() - 1) row = _model->rowCount() - 1;

    QModelIndex idx = _model->index(row, 0);
    if(select) this->selectRow(row);
    this->scrollTo(idx);
    emit clicked(idx);
}



void LibraryView::keyPressEvent(QKeyEvent* event){

    // _edit has changed
    QString text = event->text();

    if(text.size() > 0 && text[0].isLetterOrNumber()){
        int sb_width = this->verticalScrollBar()->width();
        if(!this->verticalScrollBar()->isVisible()) sb_width = 0;

        int sb_height = this->horizontalScrollBar()->height();
        if(!this->horizontalScrollBar()->isVisible()) sb_height = 0;

        _edit->setGeometry(this->width() - (sb_width + 105), this->height() - (sb_height + 30), 100, 25);
        _edit->setFocus();
        _edit->setText(text);
        _edit->show();
        return;
    }

    int key = event->key();
    Qt::KeyboardModifiers  modifiers = event->modifiers();

    bool shift_pressed = (modifiers & Qt::ShiftModifier);
    bool alt_pressed = (modifiers & Qt::AltModifier);
    bool ctrl_pressed = (modifiers & Qt::ControlModifier);

    if((key == Qt::Key_Up || key == Qt::Key_Down)){
        if(_edit->isVisible()) reset_edit();

        if(ctrl_pressed)
            event->setModifiers(Qt::NoModifier);
    }


    if(key != Qt::Key_Tab && key != Qt::Key_Backtab)
        QTableView::keyPressEvent(event);

    QList<int> selections = get_selections();

    switch(key){

        case Qt::Key_Escape:
            if(_edit->isVisible()) reset_edit();

            else {
                clearSelection();
                this->selectionModel()->clearSelection();
            }
            break;

        case Qt::Key_Return:
        case Qt::Key_Enter:

            if(_edit->isVisible()) {
                reset_edit();
                break;
            }
            if(selections.size() == 0) break;
            if(ctrl_pressed) break;

            // standard enter
            if(!shift_pressed && !alt_pressed)
                emit doubleClicked( _model->index(selections[0], 0));

            // enter with shift
            else if(shift_pressed && !alt_pressed){
                append_clicked();
            }

            else if(alt_pressed){
                play_next_clicked();
            }

            break;

        case Qt::Key_Tab:
            if(alt_pressed || ctrl_pressed) break;
            emit sig_tab_pressed(false);
            break;

        case Qt::Key_Backtab:
            if(alt_pressed || ctrl_pressed) break;
            emit sig_tab_pressed(true);
            break;


        case Qt::Key_End:
            this->selectRow(_model->rowCount() - 1);
            break;

        case Qt::Key_Home:
            this->selectRow(0);
            break;

        default: break;
    }

}



void LibraryView::edit_changed(QString str){

    if(str.size() == 0) {
        reset_edit();
        return;
    }

    int line = _model->getFirstRowOf(str);
    this->scrollTo(_model->index(line, 0));
    this->selectRow(line);
}

void LibraryView::reset_edit(){
    disconnect(_edit, SIGNAL(textChanged(QString)), this, SLOT(edit_changed(QString)));
    _edit->setText("");
    connect(_edit, SIGNAL(textChanged(QString)), this, SLOT(edit_changed(QString)));
    _edit->hide();
    this->setFocus();
}


void LibraryView::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ){


    QModelIndexList idx_list = this->selectionModel()->selectedRows();
    qDebug() << idx_list;

    if(_qDrag) {
        delete _qDrag;
        _qDrag = NULL;
    }


    QTableView::selectionChanged(selected, deselected);

    QList<int> idx_list_int;

    foreach(QModelIndex model_idx, idx_list){
        if(idx_list_int.contains(model_idx.row())) continue;

        idx_list_int.push_back(model_idx.row());
    }

    _model->set_selected(idx_list_int);

    if(selected.indexes().size() > 0)
        this->scrollTo(selected.indexes()[0]);

    emit sig_sel_changed(idx_list_int);
    _sel_changed = true;
}


QList<int> LibraryView::get_selections(){

    QList<int> idx_list_int;
    QModelIndexList idx_list = this->selectionModel()->selectedRows();

    foreach(QModelIndex model_idx, idx_list){
        idx_list_int.push_back(model_idx.row());
    }

    return idx_list_int;
}



template <typename T>
void switch_sorters(T& srcdst, T src1, T src2){
    if(srcdst == src1) srcdst = src2;
    else srcdst = src1;
}


void LibraryView::sort_by_column(int col){

    int idx_col = _model->calc_shown_col(col);

    if(idx_col >= _table_headers.size()) return;

    ColumnHeader h = _table_headers[idx_col];
    switch_sorters(_sort_order, h.get_asc_sortorder(), h.get_desc_sortorder());

    emit sig_sortorder_changed(_sort_order);

}



void LibraryView::rc_menu_init(){
    _rc_menu = new ContextMenu(this);
    _rc_menu->setup_entries(ENTRY_PLAY_NEXT | ENTRY_INFO | ENTRY_DELETE | ENTRY_EDIT | ENTRY_APPEND);
}


void LibraryView::rc_menu_show(const QPoint& p){

    emit sig_no_disc_menu();

    connect(_rc_menu, SIGNAL(sig_edit_clicked()), this, SLOT(edit_clicked()));
    connect(_rc_menu, SIGNAL(sig_info_clicked()), this, SLOT(info_clicked()));
    connect(_rc_menu, SIGNAL(sig_delete_clicked()), this, SLOT(delete_clicked()));
    connect(_rc_menu, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next_clicked()));
    connect(_rc_menu, SIGNAL(sig_append_clicked()), this, SLOT(append_clicked()));

    _rc_menu->exec(p);

    disconnect(_rc_menu, SIGNAL(sig_edit_clicked()), this, SLOT(edit_clicked()));
    disconnect(_rc_menu, SIGNAL(sig_info_clicked()), this, SLOT(info_clicked()));
    disconnect(_rc_menu, SIGNAL(sig_delete_clicked()), this, SLOT(delete_clicked()));
    disconnect(_rc_menu, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next_clicked()));
    disconnect(_rc_menu, SIGNAL(sig_append_clicked()), this, SLOT(append_clicked()));
}


void LibraryView::edit_clicked(){
    emit sig_edit_clicked();
}
void LibraryView::info_clicked(){
    emit sig_info_clicked();
}
void LibraryView::delete_clicked(){
    emit sig_delete_clicked();
}
void LibraryView::play_next_clicked(){
    emit sig_play_next_clicked();
}

void LibraryView::append_clicked(){
    emit sig_append_clicked();
}

void LibraryView::set_table_headers(QList<ColumnHeader>& headers, Sort::SortOrder sorting){

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


void LibraryView::rc_header_menu_init(QStringList& shown_cols){

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



void LibraryView::rc_header_menu_changed(bool b){

    Q_UNUSED(b);

    QList<int> sel_list = get_selections();

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

    foreach(int row, sel_list)
        this->selectRow(row);
}


void LibraryView::set_col_sizes(){

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


void LibraryView::fill_metadata(const MetaDataList& v_md){

    _view_mode = VIEW_MODE_METADATA;

    QList<int> lst;
    _model->set_selected(lst);

    uint metadata_size = v_md.size();

    _model->removeRows(0, _model->rowCount());
    _model->insertRows(0, metadata_size);

    QItemSelectionModel* sm = this->selectionModel();
    QItemSelection sel = sm->selection();

    for(uint row=0; row<metadata_size; row++){
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

void LibraryView::fill_albums(const AlbumList& albums){

    _view_mode = VIEW_MODE_ALBUMS;

    QList<int> lst;
    _model->set_selected(lst);
    uint albums_size = albums.size();

    _model->removeRows(0, _model->rowCount());
    _model->insertRows(0, albums_size); // fake "all albums row"

    QModelIndex idx;
    int first_selected_album_row = -1;

    QItemSelectionModel* sm = this->selectionModel();
    QItemSelection sel = sm->selection();

    for(uint row=0; row < albums_size; row++){
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


void LibraryView::fill_artists(const ArtistList& artists){

    _view_mode = VIEW_MODE_ARTISTS;
    QList<int> lst;
    _model->set_selected(lst);
    uint artist_size = artists.size();

    _model->removeRows(0, _model->rowCount());
    _model->insertRows(0, artist_size); // fake "all albums row"

    QModelIndex idx;
    int first_selected_artist_row = -1;

    QItemSelectionModel* sm = this->selectionModel();
    QItemSelection sel = sm->selection();

    for(uint row=0; row < artist_size; row++){
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

void LibraryView::set_skin(bool dark){
    _dark = dark;
    calc_corner_widget();
}

void LibraryView::calc_corner_widget(){

    if(!this->verticalScrollBar() || !this->verticalScrollBar()->isVisible() || !this->horizontalScrollBar()->isVisible()){
        this->setCornerWidget(NULL);
        _corner_widget->hide();
        return;
    }

    if(!this->cornerWidget()){
        this->setCornerWidget(_corner_widget);
        _corner_widget->show();

    }

    if(this->cornerWidget()){
        if(_dark)
            this->cornerWidget()->setStyleSheet(QString("background: #3c3c3c;"));
        else{
            QPalette palette = _parent->palette();
            QColor bg = palette.color(QPalette::Normal, QPalette::Window);
            this->cornerWidget()->setStyleSheet(QString("background: ") + bg.name() + ";");
        }
    }
}


void LibraryView::resizeEvent(QResizeEvent* event){
    event->ignore();
    QTableView::resizeEvent(event);
    calc_corner_widget();
}

void LibraryView::dropEvent(QDropEvent *event){

    event->accept();
    const QMimeData* mime_data = event->mimeData();

    if(!mime_data) return;

    QString text = "";
    if(mime_data->hasText()) text = mime_data->text();

    // extern drops
    if( !mime_data->hasUrls() || text.compare("tracks", Qt::CaseInsensitive) == 0) {

        return;
    }


    QStringList filelist;
    foreach(QUrl url, mime_data->urls()){
        QString path;
        QString url_str = url.toString();
        path =  url_str.right(url_str.length() - 7).trimmed();
        path = path.replace("%20", " ");

        if(QFile::exists(path)){
            filelist << path;
        }
    } // end foreach

    emit sig_import_files(filelist);

}

void LibraryView::dragEnterEvent(QDragEnterEvent *event){
    event->accept();
}


void  LibraryView::dragMoveEvent(QDragMoveEvent *event){
    event->accept();
}
