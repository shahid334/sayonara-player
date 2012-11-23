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

#include "GUI/library/LibraryItemDelegateTracks.h"

#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"

#include <QObject>
#include <QLabel>
#include <QDebug>
#include <QTableView>
#include <QItemDelegate>
#include <QPainter>
#include <HelperStructs/Helper.h>

#ifndef COL_MACROS
    #define COL_MACROS
    #define COL_TRACK_NUM 0
    #define COL_TITLE 1
    #define COL_ALBUM 2
    #define COL_ARTIST 3
    #define COL_YEAR 4
    #define COL_LENGTH 5
    #define COL_BITRATE 6
#endif


LibraryItemDelegateTracks::LibraryItemDelegateTracks(QTableView* parent) {
	this->_parent = parent;


}

LibraryItemDelegateTracks::~LibraryItemDelegateTracks() {

}




void LibraryItemDelegateTracks::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	if(!index.isValid()) return;

    int col = index.column();
    painter->save();

    QRect 	rect(option.rect);
    QString	text = index.model()->data(index, Qt::DisplayRole).toString();

   /* QFont font;
    font.setBold(true);
    font.setFamily("DejaVu Sans");

    painter->setFont(font);*/

    switch(col){

        case COL_YEAR:
            if(text == "0") text = "";

        case COL_TRACK_NUM:
        case COL_LENGTH:
        case COL_BITRATE:
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


