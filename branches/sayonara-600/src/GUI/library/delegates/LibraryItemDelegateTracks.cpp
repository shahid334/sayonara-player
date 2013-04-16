/* LibraryItemDelegateTracks.cpp */

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
 * LibraryItemDelegateTracks.cpp
 *
 *  Created on: Oct 28, 2011
 *      Author: luke
 */

#include "GUI/library/delegates/LibraryItemDelegateTracks.h"
#include "GUI/library/models/LibraryItemModelTracks.h"

#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"

#include <QObject>
#include <QLabel>
#include <QDebug>
#include <QTableView>
#include <QItemDelegate>
#include <QPainter>

LibraryItemDelegateTracks::LibraryItemDelegateTracks(LibraryItemModel* model, QTableView* parent) {
	this->_parent = parent;
	_model = model;
	_selected_background = QColor(66,78,114);


}

LibraryItemDelegateTracks::~LibraryItemDelegateTracks() {

}

void LibraryItemDelegateTracks::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	if(!index.isValid()) return;

    int col = index.column();
    LibraryItemModelTracks* model = (LibraryItemModelTracks*) _parent->model();
    int idx_col = model->calc_shown_col(col);
    painter->save();

    QRect 	rect(option.rect);
    QString	text = index.model()->data(index, Qt::DisplayRole).toString();

	if(_model->is_selected(index.row())){
		painter->fillRect(rect, _selected_background);
	}


    switch(idx_col){

        case COL_FILESIZE:
            text = Helper::calc_filesize_str(text.toInt());
            rect.translate(-2, 0);
            painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
            break;

        case COL_BITRATE:
            text = QString::number(text.toInt() / 1000) + " kbit/s";
            rect.translate(-2, 0);
            painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
            break;
        case COL_YEAR:
                if(text == "0") text = "";
        case COL_TRACK_NUM:
        case COL_LENGTH:
            rect.translate(-2, 0);
            painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
            break;
        default:
            rect.translate(2, 0);
            painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
            break;
    }

    painter->restore();
}



QSize LibraryItemDelegateTracks::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{


	Q_UNUSED(option);
	Q_UNUSED(index);

	return QSize(1, _parent->rowHeight(index.row()));


}




void LibraryItemDelegateTracks::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex & index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(model);
	Q_UNUSED(index);
}

QWidget *LibraryItemDelegateTracks::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	Q_UNUSED(parent);
	Q_UNUSED(option);
	Q_UNUSED(index);
	return 0;
}




void LibraryItemDelegateTracks::setEditorData(QWidget *editor, const QModelIndex & index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(index);
}


void LibraryItemDelegateTracks::set_skin(bool dark){
    if(dark){
		_selected_background = QColor(66,78,114);
	}

	else{
		_selected_background = _parent->palette().color(QPalette::Active, QPalette::Highlight);
	}
}
