/* PlaylistItemDelegate.cpp */

/* Copyright (C) 2012  Lucio Carreras
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



#include <QObject>
#include <QLabel>
#include <QListView>
#include <QPainter>
#include "GUI/playlist/delegate/PlaylistItemDelegate.h"
#include "GUI/playlist/entry/GUI_PlaylistEntryBig.h"
#include "GUI/playlist/entry/GUI_PlaylistEntrySmall.h"

static QString get_fg_color(int val_bg){

	if(val_bg > 160)
		return  QString(" color: #202020; ");

	else
		return QString(" color: #D8D8D8 ");
}

PlaylistItemDelegate::PlaylistItemDelegate(QListView* parent, bool small){

	if(small){ 	
		_row_height = 20;
		_pl_entry = new GUI_PlaylistEntrySmall();
	}
	else{
		_row_height = 31;		
		_pl_entry = new GUI_PlaylistEntryBig();
	}

	_parent = parent;
	_rendered_items = 0;
}

PlaylistItemDelegate::~PlaylistItemDelegate(){
	delete _pl_entry;
	_row_height = 0;
}

void PlaylistItemDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, 
                     const QModelIndex &index) const {

	if(!index.isValid()) return;

	QRect rect(option.rect);
	_pl_entry->setMaximumSize(_max_width, _row_height);
	_pl_entry->setMinimumSize(_max_width, _row_height);
	_pl_entry->resize(_max_width, _row_height);

	QVariant mdVariant = index.model()->data(index, Qt::WhatsThisRole);
	MetaData md;
	if( !MetaData::fromVariant(mdVariant, md) ) return;

	_pl_entry->setContent(md, index.row() +1 );

	QString style;
	QPalette palette = _parent->palette();

	QColor col_background = palette.color(QPalette::Active, QPalette::Background);
	QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);
	QColor col_highlight_lighter = col_highlight.darker(140);

	int highlight_val = col_highlight.lightness();
	int playing_val = col_highlight_lighter.lightness();
	int background_val = col_background.lightness();

	if(md.pl_playing){
		style = QString("background-color: ") +
			col_highlight_lighter.name() + "; " + 
			get_fg_color(playing_val);
	}

	else if(md.is_disabled){
		style = QString("color: #A0A0A0; background-color: transparent;");
	}

	else if(!md.pl_selected){
		style = QString("background-color: transparent; ") +
			get_fg_color(background_val);
	}

	// standard selected
    else{
		style = QString("background-color: ") + 
			col_highlight.name() + ";" + 
			get_fg_color(highlight_val);
    }

	int y = rect.topLeft().y() +  _pl_entry->height() -1;
	_pl_entry->setStyleSheet(style);
	if(md.is_disabled) _pl_entry->setDisabled(true);

	painter->save();
	painter->translate(0, 0);

	_pl_entry->render(painter, rect.topLeft() );

	if(md.pl_dragged) {
		painter->drawLine(QLine(0, y, _max_width, y));
	}

	painter->restore();
}


QSize PlaylistItemDelegate::sizeHint(const QStyleOptionViewItem &option,
	                     const QModelIndex &index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);

    return QSize(_max_width, _row_height);
}



void PlaylistItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const{
	Q_UNUSED(editor);
	Q_UNUSED(index);
}

void PlaylistItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const{
	Q_UNUSED(editor);
	Q_UNUSED(index);
	Q_UNUSED(model);
}

QWidget* PlaylistItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(parent);
	Q_UNUSED(option);
	Q_UNUSED(index);

	return 0;
}

void PlaylistItemDelegate::setMaxWidth(int w){
	_max_width = w;
}

int PlaylistItemDelegate::rowHeight(){
	return _row_height;
}

	

