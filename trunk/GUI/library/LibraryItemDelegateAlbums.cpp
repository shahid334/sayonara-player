/*
 * LibraryItemDelegateAlbums.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: luke
 */

#include "LibraryItemDelegateAlbums.h"
#include <QObject>
#include <QLabel>
#include <QDebug>
#include <QTableView>
#include <QItemDelegate>
#include <QPainter>
#include <HelperStructs/Helper.h>
LibraryItemDelegateAlbums::LibraryItemDelegateAlbums(QTableView* parent) {
	this->_parent = parent;


}

LibraryItemDelegateAlbums::~LibraryItemDelegateAlbums() {
	// TODO Auto-generated destructor stub
}




void LibraryItemDelegateAlbums::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	QItemDelegate::paint(painter, option, index);

	if(!index.isValid()) return;

		QLabel label;

		QItemDelegate::paint(painter, option, index);

		QRect rect(option.rect);

		painter->save();
		painter->translate(2, 0);

		if(index.column() == 0){

			label.setPixmap(QPixmap(Helper::getIconPath() + "play_small.png"));
			label.resize(20, 20);

		}

		else if(index.column() == 1){
			QString text = index.model()->data(index, Qt::WhatsThisRole).toString();
			if(text.length() == 0) return;

			label.setText(text);
			label.setContentsMargins(2, 2, 0, 2);

			label.resize(_parent->width()-20, 20);

		}

		//bool is_selected = ((option.state & QStyle::State_Selected) != 0);


		int val = _parent->palette().color(QPalette::Background).lightness();
		if(val < 128 && !((option.state & QStyle::State_Selected) != 0)){
			label.setStyleSheet("background-color: transparent; color: rgb(216, 216, 216);");
		}

		else label.setStyleSheet("background-color: transparent");

		label.render(painter, rect.topLeft() );



		painter->restore();


}





QSize LibraryItemDelegateAlbums::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{


	Q_UNUSED(option);
	Q_UNUSED(index);

	return QSize(1,20);


}




void LibraryItemDelegateAlbums::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex & index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(model);
	Q_UNUSED(index);
}

QWidget *LibraryItemDelegateAlbums::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	Q_UNUSED(parent);
	Q_UNUSED(option);
	Q_UNUSED(index);
	return 0;
}




void LibraryItemDelegateAlbums::setEditorData(QWidget *editor, const QModelIndex & index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(index);
}


