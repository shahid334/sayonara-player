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
	//QItemDelegate::paint(painter, option, index);

	if(!index.isValid()) return;

		painter->save();
		painter->translate(0, 0);


		QRect 	rect(option.rect);
		QLabel 	label;

		QString	text = QString("<b>") + index.model()->data(index, Qt::DisplayRole).toString() + QString("</b>");

		label.setText(text);
		label.setMinimumHeight(20);


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





QSize LibraryItemDelegateTracks::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{


	Q_UNUSED(option);
	Q_UNUSED(index);

	return QSize(1,20);


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


