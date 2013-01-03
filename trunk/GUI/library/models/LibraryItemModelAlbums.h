/* LibraryItemModelAlbums.h */

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
 * LibraryItemModelAlbums.h
 *
 *  Created on: Apr 26, 2011
 *      Author: luke
 */

#ifndef LIBRARYITEMMODELALBUMS_H_
#define LIBRARYITEMMODELALBUMS_H_

#include "HelperStructs/MetaData.h"
#include "GUI/library/models/LibraryItemModel.h"
#include "GUI/MyColumnHeader.h"

#include <QAbstractTableModel>
#include <QList>




using namespace std;

class LibraryItemModelAlbums : public LibraryItemModel {

Q_OBJECT
public:
	LibraryItemModelAlbums(QList<ColumnHeader>& list);
	virtual ~LibraryItemModelAlbums();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

  	void sort(int column, Qt::SortOrder order);

private:
	QList<Album> 			_album_list;

};

#endif /* LIBRARYITEMMODELALBUMS_H_ */
