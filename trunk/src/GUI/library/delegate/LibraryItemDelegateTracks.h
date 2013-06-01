/* LibraryItemDelegateTracks.h */

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
 * ibraryItemDelegateTracks.h
 *
 *  Created on: Oct 28, 2011
 *      Author: luke
 */

#ifndef IBRARYITEMDELEGATETRACKS_H_
#define IBRARYITEMDELEGATETRACKS_H_

#include <QItemDelegate>
#include <QTableView>
#include <QLabel>
#include <QPen>
#include <QColor>
#include "GUI/library/model/LibraryItemModelTracks.h"

class LibraryItemDelegateTracks : public QItemDelegate {
public:
    LibraryItemDelegateTracks(LibraryItemModel* model, QTableView* parent=0);
	virtual ~LibraryItemDelegateTracks();

public:


	    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;


		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

        void set_skin(bool dark);


private:
		QTableView* _parent;
        QPen        _pen;
        LibraryItemModel* _model;
        QColor _selected_background;

};

#endif /* IBRARYITEMDELEGATETRACKS_H_ */
