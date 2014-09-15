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

#include "GUI/library/delegate/LibraryItemDelegateTracks.h"
#include "GUI/library/model/LibraryItemModelTracks.h"
#include "GUI/RatingLabel.h"

#include "HelperStructs/Helper.h"

#include <QLabel>
#include <QTableView>
#include <QItemDelegate>
#include <QPainter>

LibraryItemDelegateTracks::LibraryItemDelegateTracks(LibraryItemModel* model, LibraryView* parent, bool enabled) : LibraryRatingDelegate(model, parent, enabled) {
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

    LibraryItemModelTracks* model = (LibraryItemModelTracks*) index.model();
    int idx_col = model->calc_shown_col(col);
    painter->save();

    QRect 	rect(option.rect);
    QString	text = index.data().toString();

    if(_model->is_selected(index.row())) {
        painter->fillRect(rect, _selected_background);
    }

    if(idx_col == COL_FILESIZE) {
        text = Helper::calc_filesize_str(text.toInt());
        rect.translate(-2, 0);
        painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
    }

    else if(idx_col == COL_BITRATE) {
        text = QString::number(text.toInt() / 1000) + " kbit/s";
        rect.translate(-2, 0);
        painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
    }
    else if(idx_col == COL_YEAR) {
            if(text == "0") text = "";
            painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
    }
    else if(idx_col == COL_TRACK_NUM) {
            painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
    }
    else if(idx_col == COL_LENGTH) {
        rect.translate(-2, 0);
        painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
    }

    else if(idx_col == COL_TRACK_RATING) {
        int r = index.data().toInt();
        Rating rating(r);
        rating.paint(painter, rect, option.palette);
    }

    else{
        rect.translate(2, 0);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
    }


    painter->restore();
}



QSize LibraryItemDelegateTracks::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{


	Q_UNUSED(option);
	Q_UNUSED(index);

	return QSize(1, _parent->rowHeight(index.row()));


}




void LibraryItemDelegateTracks::set_skin(bool dark) {
    if(dark) {
		_selected_background = QColor(66,78,114);
	}

	else{
		_selected_background = _parent->palette().color(QPalette::Active, QPalette::Highlight);
	}
}
