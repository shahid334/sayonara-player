/* PlaylistItemDelegate.cpp */

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
 * ItemListDelegate.cpp
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"
#include "GUI/playlist/PlaylistItemDelegate.h"
#include "GUI/playlist/GUI_PlaylistEntry.h"


#include <QtGui>
#include <QItemDelegate>
#include <QPalette>

#include <iostream>

using namespace std;

QString get_fg_color(int val_bg){

	if(val_bg > 128)
		return  QString(" color: #202020; ");

	else
		return QString(" color: #D8D8D8 ");
}

PlaylistItemDelegate::PlaylistItemDelegate(QListView *parent ) {
	// TODO Auto-generated constructor stub
	//_label_to_Render = new QLabel(parent);

	_pl_entry = new GUI_PlaylistEntry();
	_rendered_items = 0;
	int num = 33;
	_pl_entry->setMinimumHeight(num);
	_pl_entry->setMaximumHeight(num);


	_parent = parent;


}

PlaylistItemDelegate::~PlaylistItemDelegate() {
}




//! [0]
void PlaylistItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{


	if(!index.isValid()) return;

	QRect rect(option.rect);

	painter->save();
	painter->translate(0, 0);

	QStringList strlist = index.model()->data(index, Qt::WhatsThisRole).toStringList();
	MetaData md;
		md.fromStringList(strlist);

	bool cur_track = (strlist[strlist.length()-2].toInt() == 1);
	bool insert = (strlist.last().toInt() == 1);


	_pl_entry->setArtist(md.artist);
	_pl_entry->setAlbum(md.album);
	_pl_entry->setTitle(md.title);
	_pl_entry->setTime(Helper::cvtMsecs2TitleLengthString(md.length_ms));


	int offset = (this->_parent->verticalScrollBar()->isVisible()) ?
						this->_parent->verticalScrollBar()->width() + 4 : 4;

	_pl_entry->setMinimumWidth(_parent->width()-offset);
	_pl_entry->setMaximumWidth(_parent->width()-offset);

	QString style;
	QString col_fg;


	bool is_selected = ((option.state & QStyle::State_Selected) != 0);

	QPalette palette = _parent->palette();

	QColor col_background = palette.color(QPalette::Active, QPalette::Background);
	QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);
	QColor col_highlight_lighter = palette.color(QPalette::Active, QPalette::Highlight).light();

	int highlight_val = col_highlight.lightness();
	int highlight_lighter_val = col_highlight_lighter.lightness();
	int background_val = col_background.lightness();


	if(cur_track)
		style = QString("background-color: ") +
				col_highlight_lighter.name() + ";" +
				get_fg_color(highlight_lighter_val);


	else if(!is_selected)
		style = QString("background-color: transparent;") + get_fg_color(background_val);



	// standard selected
	else
		style = QString("background-color: ") +
				col_highlight.name() + "; " +
				get_fg_color(highlight_val);



	int y = rect.topLeft().y() +  _pl_entry->height() -1;



	_pl_entry->setStyleSheet(style);
	_pl_entry->render(painter, rect.topLeft() );

	if(insert) {
			painter->drawLine(QLine(0, y, _pl_entry->width(), y));
		}


	painter->restore();


}


QSize PlaylistItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{

	Q_UNUSED(option);
	Q_UNUSED(index);
	return QSize(600, 33);
}



QWidget *PlaylistItemDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const

{
	Q_UNUSED(parent);
	Q_UNUSED(option);
	Q_UNUSED(index);

	return 0;
}


void PlaylistItemDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(index);
}

void PlaylistItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(index);
	Q_UNUSED(model);
}


