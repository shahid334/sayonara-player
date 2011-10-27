/*
 * LibraryItemDelegateAlbums.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: luke
 */

#include "LibraryItemDelegateAlbums.h"
#include <HelperStructs/MetaData.h>
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

			//bool is_selected = ((option.state & QStyle::State_Selected) != 0);

			QStringList list = index.model()->data(index, Qt::WhatsThisRole).toStringList();
			Album album;
			album.fromStringList(list);

			QString text = QString("<b>") + album.name + "</b>";// - " + QString::number(album.num_songs) + " tracks, " + Helper::cvtMsecs2TitleLengthString(album.length_sec * 1000);

			label.setText(text);
			label.setContentsMargins(2, 2, 0, 2);
			label.resize(_parent->columnWidth(1), 20);


		}

		else{


			QString text  = index.model()->data(index, Qt::WhatsThisRole).toString();
			if(index.model()->data(index, Qt::WhatsThisRole).toInt() == 0) text = "Unknown";

			label.setText(text);
			label.setContentsMargins(2, 2, 0, 2);
			label.resize(_parent->columnWidth(index.column()), 20);
		}



		int val = _parent->palette().color(QPalette::Active, QPalette::Background).lightness();
		bool is_selected = ((option.state & QStyle::State_Selected) != 0);

		QColor col_highlight = _parent->palette().color(QPalette::Active, QPalette::Highlight);
		QColor col_highlight_lighter = _parent->palette().color(QPalette::Active, QPalette::Highlight).light();

		// DARK SKIN AND NOT SELECTED

		if(val < 128 && !is_selected){
			label.setStyleSheet("background-color: transparent; color: rgb(216, 216, 216);");
		}

		// LIGHT SKIN AND NOT SELECTED
		else if(val >= 128 && !is_selected){
			label.setStyleSheet("background-color: transparent;");
		}

		// SELECTED
		else {
			label.setStyleSheet("background-color: " + col_highlight.name() + ";");
		}

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


