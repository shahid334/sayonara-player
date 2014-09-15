/* LibraryItemDelegateAlbumsCover.h */

/* Copyright (C) 2013  Lucio Carreras
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



#ifndef LIBRARYITEMDELEGATEALBUMSCOVER_H
#define LIBRARYITEMDELEGATEALBUMSCOVER_H

#include <QStyledItemDelegate>
#include "GUI/library/model/LibraryItemModelAlbums.h"
#include "GUI/library/view/LibraryView.h"

class LibraryItemDelegateAlbumsCover : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit LibraryItemDelegateAlbumsCover(LibraryItemModel* model, LibraryView* parent=0);
	virtual ~LibraryItemDelegateAlbumsCover();

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void set_skin(bool dark);

private:

	QPen _pen;
	LibraryItemModel* _model;
	QColor _selected_background;
	
};

#endif // LIBRARYITEMDELEGATEALBUMSCOVER_H
