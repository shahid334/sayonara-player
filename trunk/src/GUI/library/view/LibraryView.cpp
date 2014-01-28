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

LibraryView::LibraryView(QWidget* parent) : SearchableTableView(parent) {
    _parent = parent;
    _qDrag = 0;
    _rc_header_menu = 0;
    _dark = true;

    _mimedata = new CustomMimeData();

    rc_menu_init();

    _corner_widget = new QWidget(this);
    _corner_widget->hide();

    Helper::set_deja_vu_font(horizontalHeader(), 12);

    connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_by_column(int)));
    setAcceptDrops(true);
}


LibraryView::~LibraryView() {
    delete _rc_menu;
    delete _corner_widget;

}




// mouse events
void LibraryView::mousePressEvent(QMouseEvent* event){

    QPoint pos_org = event->pos();
    QPoint pos = QWidget::mapToGlobal(pos_org);

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
			SearchableTableView::mousePressEvent(event);
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

        SearchableTableView::mousePressEvent(event);
        pos.setY(pos.y() + 35);
        pos.setX(pos.x() + 10);
        rc_menu_show(pos);

        break;

    case Qt::MidButton:
        _drag = false;

        SearchableTableView::mousePressEvent(event);

        emit sig_middle_button_clicked(pos);
        break;

    default:
        _drag = false;
        break;
    }

}

void LibraryView::mouseMoveEvent(QMouseEvent* event){
	//SearchableTableView::mouseMoveEvent(event);
    QPoint pos = event->pos();
    int distance =  abs(pos.x() - _drag_pos.x()) +	abs(pos.y() - _drag_pos.y());

    if (_drag && _qDrag && distance > 20) {
        emit sig_no_disc_menu();
        _qDrag->exec(Qt::CopyAction);
    }

}

void LibraryView::mouseDoubleClickEvent(QMouseEvent *event){

	event->setModifiers(Qt::NoModifier);
    QTableView::mouseDoubleClickEvent(event);
}

void LibraryView::mouseReleaseEvent(QMouseEvent* event){



    switch (event->button()) {

	case Qt::LeftButton:

		SearchableTableView::mouseReleaseEvent(event);
		event->accept();

		_drag = false;
		emit sig_released();

		break;

	default:
		break;
    }
}
// mouse events end



// keyboard events
void LibraryView::keyPressEvent(QKeyEvent* event){


	int key = event->key();

    Qt::KeyboardModifiers  modifiers = event->modifiers();

	bool shift_pressed = (modifiers & Qt::ShiftModifier);
	bool alt_pressed = (modifiers & Qt::AltModifier);
	bool ctrl_pressed = (modifiers & Qt::ControlModifier);

    if((key == Qt::Key_Up || key == Qt::Key_Down)){
        if(ctrl_pressed)
            event->setModifiers(Qt::NoModifier);
    }

	SearchableTableView::keyPressEvent(event);
	if(!event->isAccepted()) return;

    QList<int> selections = get_selections();

    switch(key){

        case Qt::Key_Escape:

            clearSelection();
            this->selectionModel()->clearSelection();

            break;

        case Qt::Key_Return:
        case Qt::Key_Enter:

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
// keyboard end

void LibraryView::setModel(LibraryItemModel * model){
    SearchableTableView::setModel( (AbstractSearchTableModel*) model );

    _model = model;

}


// selections
int LibraryView::get_min_selected(){


    QList<int> selections = _model->get_selected();
    if(selections.size() == 0) return 0;
    int min = 10000;
    foreach(int i, selections){
        if(i < min) min = i;
    }
    return min;
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


void LibraryView::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ){

	QModelIndexList idx_list = this->selectionModel()->selectedRows();

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
// selections end



// fill
void LibraryView::fill_metadata(const MetaDataList& v_md){

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
// fill end



// appearance
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
// appearance end




// drag drop
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
// drag drop end



// Right click stuff
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
// right click stuff end
