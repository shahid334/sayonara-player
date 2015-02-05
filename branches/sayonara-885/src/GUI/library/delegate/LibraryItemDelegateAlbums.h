/* LibraryItemDelegateAlbums.h */

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
 * LibraryItemDelegateAlbums.h
 *
 *  Created on: Apr 28, 2011
 *      Author: luke
 */

#ifndef LIBRARYITEMDELEGATEALBUMS_H_
#define LIBRARYITEMDELEGATEALBUMS_H_
#include <QObject>
#include <QLabel>
#include <QTableView>
#include <QPen>

#include "GUI/library/delegate/LibraryRatingDelegate.h"
#include "GUI/library/model/LibraryItemModelAlbums.h"
#include "GUI/library/view/LibraryView.h"

class LibraryItemDelegateAlbums : public LibraryRatingDelegate {

	Q_OBJECT

public:
	LibraryItemDelegateAlbums(LibraryItemModel* model, LibraryView* parent, bool enabled);
	virtual ~LibraryItemDelegateAlbums();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;


private:

	QPixmap _icon_single_album;
	QPixmap _icon_multi_album;

	QPen _pen;
	LibraryItemModel* _model;
};

#endif /* LIBRARYITEMDELEGATEALBUMS_H_ */
