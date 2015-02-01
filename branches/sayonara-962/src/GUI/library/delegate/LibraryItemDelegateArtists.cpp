/* LibraryItemDelegateArtists.cpp */

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
 * LibraryItemDelegateArtists.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: luke
 */

#include "GUI/library/delegate/LibraryItemDelegateArtists.h"
#include "HelperStructs/Helper.h"

#include <QLabel>
#include <QTableView>
#include <QItemDelegate>
#include <QPainter>


LibraryItemDelegateArtists::LibraryItemDelegateArtists(QTableView* parent) : QItemDelegate(parent){

	_parent = parent;

	_icon_single_album = Helper::getPixmap("play_orange", QSize(16, 16), false);
	_icon_multi_album = Helper::getPixmap("fwd_orange", QSize(16, 16), false);

}

LibraryItemDelegateArtists::~LibraryItemDelegateArtists() {

}


void LibraryItemDelegateArtists::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

    if(!index.isValid()) return;

    QRect rect(option.rect);
	painter->save();

	LibraryItemModelArtists* model = (LibraryItemModelArtists*) index.model();

    int col = index.column();
	int idx_col = model->calc_shown_col(col);

    if(idx_col == COL_ARTIST_N_ALBUMS) {
        int col_width = _parent->columnWidth(0)-4;
        int row_height = _parent->rowHeight(0)-4;
        rect.translate(2, 2);

		int num_albums = index.data(Qt::WhatsThisRole).toInt();

		if(num_albums <= 1){
			painter->drawPixmap(rect.x(), rect.y(), col_width, row_height, _icon_single_album);
		}

		else{
            painter->drawPixmap(rect.x(), rect.y(), col_width, row_height, _icon_multi_album);
		}

    }

    else if(idx_col == COL_ARTIST_NAME) {

        rect.translate(2, 0);
		QString name = index.data(Qt::WhatsThisRole).toString();

        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, name);

    }

    else if(idx_col == COL_ARTIST_TRACKS) {

        rect.translate(-2, 0);
		int n_tracks = index.data(Qt::WhatsThisRole).toInt();

        painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, QString::number(n_tracks) + " tracks");

    }

    painter->restore();

}





QSize LibraryItemDelegateArtists::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    return QSize(1, _parent->rowHeight(index.row()));
}




void LibraryItemDelegateArtists::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex & index) const
{
    Q_UNUSED(editor);
    Q_UNUSED(model);
    Q_UNUSED(index);
}

QWidget *LibraryItemDelegateArtists::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    Q_UNUSED(parent);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return 0;
}


void LibraryItemDelegateArtists::setEditorData(QWidget *editor, const QModelIndex & index) const
{
    Q_UNUSED(editor);
    Q_UNUSED(index);
}

