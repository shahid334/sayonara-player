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


LibraryItemDelegateTracks::LibraryItemDelegateTracks(QTableView* parent) {
	this->_parent = parent;

        _label = new QLabel();
        _label->setContentsMargins(2, 0, 2, 0);

        QPalette palette = _parent->palette();
        QColor col_background = palette.color(QPalette::Active, QPalette::Background);
        QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);

        _col_highlight_name = col_highlight.name();
        _val_bg = col_background.lightness();
        _val_sel = col_highlight.lightness();




}

LibraryItemDelegateTracks::~LibraryItemDelegateTracks() {
	// TODO Auto-generated destructor stub
    delete _label;
}




void LibraryItemDelegateTracks::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{


	if(!index.isValid()) return;

        int col = index.column();
        int col_width = _parent->columnWidth(col);
        int row_height = _parent->rowHeight(index.row()) - 1;

        painter->save();
        painter->translate(0, 0);

        QRect 	rect(option.rect);

        QString	text = QString("<b>") + index.model()->data(index, Qt::DisplayRole).toString() + QString("</b>");
        QString style;

        bool is_selected = ((option.state & QStyle::State_Selected) != 0);

        if(!is_selected)
                style = QString("background-color: transparent; ");

        else
                style = QString("background-color: " + _col_highlight_name + "; ");

        _label->setStyleSheet(style);
        _label->setFixedHeight(row_height);
        _label->setFixedWidth(col_width);
        _label->setText(text);


        if(col == 0 || col >= 4)
                _label->setAlignment( Qt::AlignVCenter | Qt::AlignRight);

        else
                _label->setAlignment( Qt::AlignVCenter | Qt::AlignLeft);


        //_label->render(painter, rect.topLeft() );
        painter->drawText(rect, text);

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


