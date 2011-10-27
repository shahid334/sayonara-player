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


	int offset = (this->_parent->verticalScrollBar()->isVisible()) ?  this->_parent->verticalScrollBar()->width()+4 : 4;

	_pl_entry->setMinimumWidth(_parent->width()-offset);
	_pl_entry->setMaximumWidth(_parent->width()-offset);


	bool is_selected = ((option.state & QStyle::State_Selected) != 0);

	QPalette palette = _parent->palette();

	QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);
	QColor col_highlight_lighter = palette.color(QPalette::Active, QPalette::Highlight).light();


	int lightness = _parent->palette().color(QPalette::Active, QPalette::Background).lightness();

	if(cur_track || !is_selected){

		// not selected tracks and not current track
			if(!cur_track && lightness < 96)
				_pl_entry->setStyleSheet("background-color: transparent;  color: rgb(216,216,216);");

			else if(!cur_track && lightness >= 96 )
				_pl_entry->setStyleSheet("background-color: transparent; ");
		// not selected tracks and not current track end

		// cur_track
		else {

			qDebug() << "Color: " << col_highlight_lighter.name();

			if(col_highlight_lighter.name() != "#ffffff")
				_pl_entry->setStyleSheet("background-color: " + col_highlight_lighter.name() );

			else
				_pl_entry->setStyleSheet("background-color: " + col_highlight.name() );
		}

	}

	// selected && !current
	else {
		if(col_highlight_lighter.name() != "#ffffff")
			_pl_entry->setStyleSheet(QString("background-color: ") + col_highlight.name() + ";");

		else
			_pl_entry->setStyleSheet(QString("background-color: transparent;"));
	}


	/*_pl_entry->setLine(false);
	if(insert) _pl_entry->setLine(true);*/

	int y = rect.topLeft().y() +  _pl_entry->height() -1;
	if(insert) painter->drawLine(QLine(0, y, _pl_entry->width(), y));


	_pl_entry->render(painter, rect.topLeft() );


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


