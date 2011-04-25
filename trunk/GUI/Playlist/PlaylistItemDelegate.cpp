/*
 * ItemListDelegate.cpp
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

#include "GUI/Playlist/PlaylistItemDelegate.h"
#include "GUI/Playlist/GUI_PlaylistEntry.h"

#include <QtGui>
#include <QItemDelegate>

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
	QItemDelegate::paint(painter, option, index);

	QRect rect(option.rect);

	painter->save();
	painter->translate(0, 1.0);

	QString text = index.model()->data(index, Qt::WhatsThisRole).toString();
	QStringList strList = text.split(",\n");

	_pl_entry->setArtist(strList.at(0));
	_pl_entry->setAlbum(strList.at(1));
	_pl_entry->setTitle(strList.at(2));
	_pl_entry->setTime(strList.at(3));

	bool has_scrollbar = false;
	if((_parent->model()->rowCount() + 1) * 33 > _parent->height()) has_scrollbar = true;

	int offset = (has_scrollbar == true) ? 15 : 0;

	_pl_entry->setMinimumWidth(_parent->width()-offset);
	_pl_entry->setMaximumWidth(_parent->width()-offset);



	bool cur_track = (strList.at(4) == "1");
	bool is_selected = ((option.state & QStyle::State_Selected) != 0);




	if(!is_selected){
		if(!cur_track){
			if(index.row() % 2 == 0)
				_pl_entry->setStyleSheet("background-color: #FFFFFF;");

			else _pl_entry->setStyleSheet("background-color: #F0F0F0;");
		}

		else
			_pl_entry->setStyleSheet("background-color: #C0C0C0; ");

	}

	else 		_pl_entry->setStyleSheet("background-color: transparent; ");




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


