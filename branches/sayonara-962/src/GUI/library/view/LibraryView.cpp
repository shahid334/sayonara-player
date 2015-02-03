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
#include "HelperStructs/MetaData.h"


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
#include "HelperStructs/MetaDataInfo.h"

LibraryView::LibraryView(QWidget* parent) : SearchableTableView(parent) {

    _qDrag = 0;
    _rc_header_menu = 0;
    _cur_filling = false;

    _mimedata = new CustomMimeData();
    _editor = 0;

    rc_menu_init();

    connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_by_column(int)));
    setAcceptDrops(true);

	clearSelection();
}


LibraryView::~LibraryView() {
    delete _rc_menu;
}


// mouse events
void LibraryView::mousePressEvent(QMouseEvent* event) {

	QPoint pos_org = event->pos();
    QPoint pos = QWidget::mapToGlobal(pos_org);
	QModelIndex idx;
	QList<int> idxs;

	switch(event->button()) {

	case Qt::LeftButton:

		SearchableTableView::mousePressEvent(event);

		idx = indexAt(event->pos());
		if(idx.isValid()){
			idxs << idx.row();
			emit sig_sel_changed(idxs);
		}

        _drag_pos = pos_org;
        _drag = true;

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

void LibraryView::mouseMoveEvent(QMouseEvent* event) {

    QPoint pos = event->pos();
    int distance = abs(pos.x() - _drag_pos.x()) +	abs(pos.y() - _drag_pos.y());

    if (_drag && _qDrag && distance > 20) {
        _qDrag->exec(Qt::CopyAction);
    }
}

void LibraryView::mouseDoubleClickEvent(QMouseEvent *event) {

    event->setModifiers(Qt::NoModifier);
    QTableView::mouseDoubleClickEvent(event);
}

void LibraryView::mouseReleaseEvent(QMouseEvent* event) {

    switch (event->button()) {

        case Qt::LeftButton:

            SearchableTableView::mouseReleaseEvent(event);
            event->accept();

            _drag = false;

            break;

        default:
            break;
    }
}
// mouse events end


// keyboard events
void LibraryView::keyPressEvent(QKeyEvent* event) {

	int key = event->key();

    Qt::KeyboardModifiers  modifiers = event->modifiers();

	bool shift_pressed = (modifiers & Qt::ShiftModifier);
	bool alt_pressed = (modifiers & Qt::AltModifier);
	bool ctrl_pressed = (modifiers & Qt::ControlModifier);

	if((key == Qt::Key_Up || key == Qt::Key_Down)) {
		if(this->selectionModel()->selection().isEmpty()) {
			if(_model->rowCount() > 0) selectRow(0);
			return;
		}

        if(ctrl_pressed)
            event->setModifiers(Qt::NoModifier);
    }

	SearchableTableView::keyPressEvent(event);
	if(!event->isAccepted()) return;

    QList<int> selections = get_selections();

	switch(key) {

		case Qt::Key_Right:
        case Qt::Key_Plus:
			if(_editor) {
                qDebug() << "Try to access editor " << _editor->get_id();
                _editor->increase();
            }
			break;

        case Qt::Key_Left:
        case Qt::Key_Minus:
            if(_editor) {
                qDebug() << "Try to access editor " << _editor->get_id();
                _editor->decrease();
            }
			break;

        case Qt::Key_Escape:

            clearSelection();
            this->selectionModel()->clearSelection();

            break;

        case Qt::Key_Return:
        case Qt::Key_Enter:

            if(selections.size() == 0) break;
            if(ctrl_pressed) break;

            // standard enter
			if(!shift_pressed && !alt_pressed){
				emit doubleClicked( _model->index(selections[0], 0));
			}

            // enter with shift
			else if(shift_pressed && !alt_pressed) {
                append_clicked();
            }

			else if(alt_pressed) {
                play_next_clicked();
            }

            break;

		case Qt::Key_End:
			this->selectRow(_model->rowCount() - 1);
            break;

        case Qt::Key_Home:
            this->selectRow(0);
            break;

		default:
			break;
    }
}
// keyboard end

void LibraryView::setModel(LibraryItemModel * model) {

	SearchableTableView::setModel( (AbstractSearchTableModel*) model );
    _model = model;
}


// selections
int LibraryView::get_min_selected() {


    QList<int> selections = _model->get_selected();
    if(selections.size() == 0) return 0;
    int min = 10000;
	for(const int& i : selections) {
        if(i < min) min = i;
    }
    return min;
}

void LibraryView::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ) {

	if(_cur_filling) return;

	QTableView::selectionChanged(selected, deselected);

    QModelIndexList idx_list = this->selectionModel()->selectedIndexes();

	QList<int> idx_list_int;
	for(const QModelIndex& model_idx : idx_list) {

		int row = model_idx.row();

		if( idx_list_int.contains(row) ) continue;

		idx_list_int << row;
	}

	_model->set_selected(idx_list_int);

	if(selected.indexes().size() > 0) {
        scrollTo(selected.indexes()[0]);
	}

    emit sig_sel_changed(idx_list_int);
}


QList<int> LibraryView::get_selections() {

    QList<int> idx_list_int;
    QModelIndexList idx_list = this->selectionModel()->selectedRows();

	for(const QModelIndex& model_idx : idx_list) {
        idx_list_int.push_back(model_idx.row());
    }

    return idx_list_int;
}
// selections end



template void LibraryView::fill<MetaDataList, MetaData>(const MetaDataList&);
template void LibraryView::fill<ArtistList, Artist>(const ArtistList&);
template void LibraryView::fill<AlbumList, Album>(const AlbumList&);


template < class TList, class T >
void LibraryView::fill(const TList& input_data) {

	QModelIndex idx;

    int size = input_data.size();
	int first_selected_row = -1;

	_cur_filling = true;

	_model->remove_all_and_insert(size);

	for(int row=0; row < size; row++) {

		QVariant var_data = T::toVariant( input_data[row] );

		if(first_selected_row == -1){

			if( input_data[row].is_lib_selected ) {
				first_selected_row = row;
			}
		}

		idx = _model->index(row, 1);
		_model->setData(idx, var_data, Qt::EditRole );
	}

	_cur_filling = false;
}


void LibraryView::set_mimedata(const MetaDataList& v_md, bool drop_entire_folder, bool for_artist) {

    _mimedata = new CustomMimeData();

    QList<QUrl> urls;
	if(!drop_entire_folder) {
		for(const MetaData& md : v_md) {
			QUrl url(QString("file://") + md.filepath());
            urls << url;
        }
    }

	else {
        QStringList filenames;
		QStringList folders;

		for(const MetaData& md : v_md) {
			filenames << md.filepath();
		}

		folders = Helper::extract_folders_of_files(filenames);
		for(const QString& folder : folders) {
            QUrl url(QString("file://") + folder);
            urls << url;
        }
    }

    _mimedata->setMetaData(v_md);
	_mimedata->setText("tracks");
    _mimedata->setUrls(urls);

	if(_qDrag){
		delete _qDrag;
	}

    _qDrag = new QDrag(this);
    _qDrag->setMimeData(_mimedata);



	QPixmap pm = Helper::getPixmap("append", QSize(48, 48), false);

	if(v_md.size() > 0){

		CoverLocation cl;
		if(for_artist){

			ArtistInfo ai(this, v_md);
			cl = ai.get_cover_location();
		}

		else{
			MetaDataInfo mdi(this, v_md);
			cl = mdi.get_cover_location();
		}

		QFile f(cl.cover_path);

		if(f.exists()){
			pm = QPixmap(cl.cover_path).scaled(QSize(48, 48), Qt::KeepAspectRatio, Qt::SmoothTransformation) ;
		}
	}

	_qDrag->setPixmap(pm);

    connect(_qDrag, SIGNAL(destroyed()), this, SLOT(drag_deleted()));

    _drag = true;
}

void LibraryView::drag_deleted() {
    _qDrag = NULL;
}
// appearance end


void LibraryView::set_editor(RatingLabel *editor) {

	_editor = editor;
	if(_editor) {
        qDebug() << "New Editor: " << _editor->get_id();
    }

}

void LibraryView::editorDestroyed(QObject* object) {

    _editor = 0;
}


// drag drop
void LibraryView::dropEvent(QDropEvent *event) {

    event->accept();
    const QMimeData* mime_data = event->mimeData();

    if(!mime_data) return;

	QString text = "";


	if(mime_data->hasText()){
		text = mime_data->text();
	}

    // extern drops
    if( !mime_data->hasUrls() || text.compare("tracks", Qt::CaseInsensitive) == 0) {
        return;
    }

	QStringList filelist;
	for(const QUrl& url : mime_data->urls()) {
        QString path;
        QString url_str = url.toString();
        path =  url_str.right(url_str.length() - 7).trimmed();
        path = path.replace("%20", " ");

		if(QFile::exists(path)) {
            filelist << path;
        }
    } // end foreach

    emit sig_import_files(filelist);

}

void LibraryView::dragEnterEvent(QDragEnterEvent *event) {
    event->accept();
}

void  LibraryView::dragMoveEvent(QDragMoveEvent *event) {
    event->accept();
}
// drag drop end



// Right click stuff
void LibraryView::rc_menu_init() {
    _rc_menu = new ContextMenu(this);
    _rc_menu->setup_entries(ENTRY_PLAY_NEXT | ENTRY_INFO | ENTRY_DELETE | ENTRY_EDIT | ENTRY_APPEND);
}

void LibraryView::rc_menu_show(const QPoint& p) {

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


void LibraryView::edit_clicked() {
    emit sig_edit_clicked();
}
void LibraryView::info_clicked() {
    emit sig_info_clicked();
}
void LibraryView::delete_clicked() {
    emit sig_delete_clicked();
}
void LibraryView::play_next_clicked() {
    emit sig_play_next_clicked();
}

void LibraryView::append_clicked() {
    emit sig_append_clicked();
}

// right click stuff end
