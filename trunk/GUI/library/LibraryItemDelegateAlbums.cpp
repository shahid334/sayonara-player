/*
 * LibraryItemDelegateAlbums.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: luke
 */

#include "GUI/library/LibraryItemDelegateAlbums.h"

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

	if(!index.isValid()) return;

	QLabel label;
	QRect rect(option.rect);

	painter->save();
	painter->translate(0, 0);

	if(index.column() == 0){

		label.setPixmap(QPixmap(Helper::getIconPath() + "play_small.png"));
	}

	else if(index.column() == 1){

		QStringList list = index.model()->data(index, Qt::WhatsThisRole).toStringList();
		Album album;
		album.fromStringList(list);

		QString text = QString("<b>") + album.name + "</b>";// - " + QString::number(album.num_songs) + " tracks, " + Helper::cvtMsecs2TitleLengthString(album.length_sec * 1000);

		label.setText(text);
	}

	else{
		QString text  = index.model()->data(index, Qt::WhatsThisRole).toString();
		if(index.model()->data(index, Qt::WhatsThisRole).toInt() == 0) text = "Unknown";

		label.setText(text);
	}



	QString style;
	QString padding = " padding-left: 2px";
	QString fg_color;

	bool is_selected = ((option.state & QStyle::State_Selected) != 0);

	QPalette palette = _parent->palette();
	QColor col_background = palette.color(QPalette::Active, QPalette::Background);
	QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);
	QColor col_highlight_lighter = palette.color(QPalette::Active, QPalette::Highlight).light();

	int val_bg = col_background.lightness();
	int val_sel = col_highlight.lightness();

	if(!is_selected){

		if(val_bg > 128) fg_color = " color: #202020";
		else fg_color = " color: #D8D8D8; ";

		style = QString("background-color: transparent; ") + fg_color;
	}

	else {
		if(val_sel > 128) fg_color = " color: #202020";
		else fg_color = " color: #D8D8D8; ";

		style = QString("background-color: " + col_highlight.name() + ";") + fg_color;
	}

	label.setContentsMargins(2, 0, 2, 2);
	label.resize(_parent->columnWidth(index.column()), 20);
	label.setStyleSheet(style);
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


