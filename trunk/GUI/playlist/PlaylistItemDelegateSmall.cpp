/* PlaylistItemDelegateSmall.cpp */

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


#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"
#include "GUI/playlist/PlaylistItemDelegateSmall.h"
#include "GUI/playlist/GUI_PlaylistEntrySmall.h"


#include <QtGui>
#include <QItemDelegate>
#include <QPalette>

#include <iostream>

using namespace std;

static QString get_fg_color(int val_bg){

	if(val_bg > 96)
		return  QString(" color: #202020; ");

	else
		return QString(" color: #D8D8D8 ");
}

PlaylistItemDelegateSmall::PlaylistItemDelegateSmall(QListView *parent ) {
	// TODO Auto-generated constructor stub
	//_label_to_Render = new QLabel(parent);


	_rendered_items = 0;

	_parent = parent;
	_row_height = 20;


	_pl_entry = new GUI_PlaylistEntrySmall();

	_pl_entry->setMinimumHeight(_row_height);
	_pl_entry->setMaximumHeight(_row_height);
}

PlaylistItemDelegateSmall::~PlaylistItemDelegateSmall() {
}




//! [0]
void PlaylistItemDelegateSmall::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{


	if(!index.isValid()) return;

	QVariant mdVariant = index.model()->data(index, Qt::WhatsThisRole);
	MetaData md;
	if( !MetaData::fromVariant(mdVariant, md) ) return;

	QRect rect(option.rect);
	painter->save();
	painter->translate(0, 0);

	_pl_entry->setContent(md, index.row() +1 );

	int offset = (this->_parent->verticalScrollBar()->isVisible()) ?
						this->_parent->verticalScrollBar()->width() + 4 : 4;

	_pl_entry->setWidth(_parent->width() - offset);

	QString style;
	QString col_fg;
	QPalette palette = _parent->palette();

	QColor col_background = palette.color(QPalette::Active, QPalette::Background);
	QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);
	QColor col_highlight_lighter = palette.color(QPalette::Active, QPalette::Highlight).light();

	int highlight_val = col_highlight.lightness();
	int highlight_lighter_val = col_highlight_lighter.lightness();
	int background_val = col_background.lightness();


	if(md.pl_playing)
		style = QString("background-color: ") +
				col_highlight_lighter.name() + ";" +
				get_fg_color(highlight_lighter_val);


	else if(!md.pl_selected)
		style = QString("background-color: transparent;") + get_fg_color(background_val);

	// standard selected
	else
		style = QString("background-color: ") +
				col_highlight.name() + "; " +
				get_fg_color(highlight_val);


	int y = rect.topLeft().y() +  _pl_entry->height()-1;

	_pl_entry->setStyleSheet(style);
	_pl_entry->render(painter, rect.topLeft() );

	if(md.pl_dragged) {
		painter->drawLine(QLine(0, y, _pl_entry->width(), y));
	}

	painter->restore();
}


QSize PlaylistItemDelegateSmall::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{

	Q_UNUSED(option);
	Q_UNUSED(index);
	return QSize(600, _row_height);
}



QWidget *PlaylistItemDelegateSmall::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const

{
	Q_UNUSED(parent);
	Q_UNUSED(option);
	Q_UNUSED(index);

	return 0;
}


void PlaylistItemDelegateSmall::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(index);
}

void PlaylistItemDelegateSmall::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(index);
	Q_UNUSED(model);
}

