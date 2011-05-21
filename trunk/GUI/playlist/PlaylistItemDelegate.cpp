/*
 * ItemListDelegate.cpp
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

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

	QString text = index.model()->data(index, Qt::WhatsThisRole).toString();
	QStringList strList = text.split(",\n");

	_pl_entry->setArtist(strList.at(0));
	_pl_entry->setAlbum(strList.at(1));
	_pl_entry->setTitle(strList.at(2));
	_pl_entry->setTime(strList.at(3));

	bool has_scrollbar = false;
	if((_parent->model()->rowCount() + 1) * 33 > _parent->height()) has_scrollbar = true;

	int offset = (has_scrollbar == true) ?  this->_parent->horizontalScrollBar()->height()+2 : 4;

	_pl_entry->setMinimumWidth(_parent->width()-offset);
	_pl_entry->setMaximumWidth(_parent->width()-offset);



	bool cur_track = (strList.at(4) == "1");
	bool is_selected = ((option.state & QStyle::State_Selected) != 0);

	QPalette palette = _parent->palette();

	QColor col = palette.color(QPalette::Highlight);
	QColor col_ia = palette.color(QPalette::Highlight).light();


	int lightness = _parent->palette().color(QPalette::Background).lightness();

	if(!is_selected){

		if(!cur_track && lightness < 96) _pl_entry->setStyleSheet("background-color: transparent;  color: rgb(216,216,216);");
		else if(!cur_track && lightness >= 96 ) _pl_entry->setStyleSheet("background-color: transparent; ");

		// cur_track
		else _pl_entry->setStyleSheet("background-color: " + col_ia.name() );
		//else _pl_entry->setStyleSheet("background-color: #F9B46A;");

	}
	else {
		_pl_entry->setStyleSheet(QString("background-color: ") + col.name() + ";");
		//_pl_entry->setStyleSheet(QString("background-color: #F3841A;"));
	}

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


