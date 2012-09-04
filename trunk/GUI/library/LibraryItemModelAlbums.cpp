/* LibraryItemModelAlbums.cpp */

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
 * LibraryItemModelAlbums.cpp
 *
 *  Created on: Apr 26, 2011
 *      Author: luke
 */

#include "LibraryItemModelAlbums.h"
#include <HelperStructs/MetaData.h>

#include <QAbstractListModel>
#include <QStringList>
#include <QDebug>
LibraryItemModelAlbums::LibraryItemModelAlbums() {
	// TODO Auto-generated constructor stub

}

LibraryItemModelAlbums::~LibraryItemModelAlbums() {
	// TODO Auto-generated destructor stub
}

int LibraryItemModelAlbums::rowCount(const QModelIndex & parent) const
{	Q_UNUSED(parent);
	return _album_list.size();
}

int LibraryItemModelAlbums::columnCount(const QModelIndex& parent) const{

	Q_UNUSED(parent);

	return 3;

	// title, artist, album, length, year

}


bool LibraryItemModelAlbums::removeRows(int position, int rows, const QModelIndex & index)
{
	Q_UNUSED(index);

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {
		 _album_list.removeAt(position);
	 }

	 endRemoveRows();
	 return true;
}



bool LibraryItemModelAlbums::insertRows(int position, int rows, const QModelIndex & index)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {

		 Album album;
		 _album_list.insert(position, album);
	 }

	 endInsertRows();
	 return true;
}



QVariant LibraryItemModelAlbums::data(const QModelIndex & index, int role) const
{
	 if (!index.isValid())
			 return QVariant();

		 if (index.row() >= _album_list.size())
			 return QVariant();

		 if(role == Qt::WhatsThisRole && index.column() == 0){
			 return _album_list.at(index.row()).is_sampler;
		 }


		 if (role == Qt::WhatsThisRole && index.column() == 1){
			 Album album = _album_list.at(index.row());
			 QStringList strlist = album.toStringList();

			 return strlist;

		 }
		 else  if (role == Qt::WhatsThisRole && index.column() == 2){
			 return _album_list.at(index.row()).year;
		 }

		 else
			 return QVariant();
}



bool LibraryItemModelAlbums::setData(const QModelIndex & index, const QVariant & value, int role)
{

	 if (index.isValid() && role == Qt::EditRole) {

		 if(index.column() == 1) {
			 QStringList list = value.toStringList();

			 Album album;
			 album.fromStringList(list);

			 _album_list.replace(index.row(), album);
		 }


	     emit dataChanged(index, index);
	     return true;
	 }

	 return false;
}



Qt::ItemFlags LibraryItemModelAlbums::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index);
}



QVariant LibraryItemModelAlbums::headerData ( int section, Qt::Orientation orientation, int role ) const{

	 if (role != Qt::DisplayRole)
	         return QVariant();

	 if (orientation == Qt::Horizontal) {
		 switch (section) {
			 case 0: return QVariant();

			 case 1: return tr("Album");
			 case 2: return tr("Year");
			 default:
				 return QVariant();
		 }
	 }
	 return QVariant();

}


void LibraryItemModelAlbums::sort(int column, Qt::SortOrder order){

	Q_UNUSED(column);
	Q_UNUSED(order);



}
