/* AlternateCoverItemDelegate.cpp */

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
#include <QDebug>



AlternateCoverItemDelegate::AlternateCoverItemDelegate(QObject* parent) : QItemDelegate(parent) {
    QPixmap pm = Helper::getPixmap("logo.png");
	label = new QLabel();
	label->setScaledContents(true);
	label->setStyleSheet("background: transparent;");

	label->setPixmap(pm);
}

AlternateCoverItemDelegate::~AlternateCoverItemDelegate() {

}


void AlternateCoverItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
	QItemDelegate::paint(painter, option, index);

    if(!index.isValid()) return;

	QRect rect(option.rect);

	painter->save();
	painter->translate(2, 0);

	QVariant var = index.model()->data(index, Qt::WhatsThisRole);
	CoverLocation cl = CoverLocation::getInvalidLocation();

	if( var.canConvert<CoverLocation>()){
		cl = var.value<CoverLocation>();
	}

	label->setEnabled(cl.valid);
	label->setMinimumHeight(100);
	label->setMinimumWidth(100);
    label->resize(100, 100);

	label->setContentsMargins(10, 10, 10, 10);

	QPixmap pixmap( cl.cover_path );

	if(!pixmap.isNull()) {

		label->setPixmap(pixmap);
	}

	label->render(painter, rect.topLeft() );

    painter->restore();

}


QSize AlternateCoverItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{

	return QSize(100, 100);

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
