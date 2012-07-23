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


}

LibraryItemDelegateTracks::~LibraryItemDelegateTracks() {
	// TODO Auto-generated destructor stub
}




void LibraryItemDelegateTracks::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{


	if(!index.isValid()) return;
	int col = index.column();

		painter->save();
		painter->translate(0, 0);

		QRect 	rect(option.rect);
		QLabel 	label;

		QString	text = QString("<b>") + index.model()->data(index, Qt::DisplayRole).toString() + QString("</b>");

		QString style;
		QString fg_color;

		bool is_selected = ((option.state & QStyle::State_Selected) != 0);

		QPalette palette = _parent->palette();
		QColor col_background = palette.color(QPalette::Active, QPalette::Background);
		QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);
		//QColor col_highlight_lighter = palette.color(QPalette::Active, QPalette::Highlight).light();

		int val_bg = col_background.lightness();
		int val_sel = col_highlight.lightness();

		if(!is_selected){

			if(val_bg > 96) fg_color = " color: #202020";
			else fg_color = " color: #D8D8D8; ";

			style = QString("background-color: transparent; ") + fg_color;
		}

		else {
			if(val_sel > 96) fg_color = " color: #202020";
			else fg_color = " color: #D8D8D8; ";

			style = QString("background-color: " + col_highlight.name() + ";") + fg_color;
		}


		if(col == 0 || col >= 4)
				label.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		label.setText(text);
		label.setContentsMargins(2, 0, 2, 2);
		label.setMinimumHeight(25);
		label.setMinimumWidth(_parent->columnWidth(index.column()));
		label.resize(_parent->columnWidth(index.column()), 20);
		label.setStyleSheet(style);

		label.render(painter, rect.topLeft() );

		painter->restore();
}





QSize LibraryItemDelegateTracks::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{


	Q_UNUSED(option);
	Q_UNUSED(index);

	return QSize(1,25);


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


