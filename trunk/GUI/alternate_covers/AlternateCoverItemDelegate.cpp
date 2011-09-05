/*
 * AlternateCoverItemDelegate.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: luke
 */

#include "GUI/alternate_covers/AlternateCoverItemDelegate.h"
#include <CoverLookup/CoverLookup.h>

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QItemDelegate>
#include <QLabel>
#include <QItemDelegate>
#include <QPainter>



AlternateCoverItemDelegate::AlternateCoverItemDelegate(QTableView* parent) {
	this->_parent = parent;

}

AlternateCoverItemDelegate::~AlternateCoverItemDelegate() {

	// TODO Auto-generated destructor stub
}


void AlternateCoverItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
	QItemDelegate::paint(painter, option, index);

		if(!index.isValid()) return;

			QLabel label;

			QItemDelegate::paint(painter, option, index);

			QRect rect(option.rect);

			painter->save();
			painter->translate(2, 0);


			CoverLookup* cl = CoverLookup::getInstance();

			int pixmap_idx = index.model()->data(index, Qt::WhatsThisRole).toInt();
			QPixmap pixmap;
			cl->get_found_cover(pixmap_idx, pixmap);

			label.setPixmap(pixmap);
			label.resize(50, 50);


			label.render(painter, rect.topLeft() );
			painter->restore();

}


QSize AlternateCoverItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{

	return QSize(50, 50);

}


QWidget* AlternateCoverItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const{
	Q_UNUSED(parent);
	Q_UNUSED(option);
	Q_UNUSED(index);
	return 0;

}

void AlternateCoverItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const{
	Q_UNUSED(editor);
	Q_UNUSED(index);

}

void AlternateCoverItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const{
	Q_UNUSED(editor);
	Q_UNUSED(model);
	Q_UNUSED(index);

}
