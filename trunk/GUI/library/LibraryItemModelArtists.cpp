/* LibraryItemModelArtists.cpp */

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
 * LibraryItemModelArtists.cpp
 *
 *  Created on: Apr 26, 2011
 *      Author: luke
 */

#include "LibraryItemModelArtists.h"

#include <QAbstractListModel>
#include <QStringList>
#include <QDebug>

LibraryItemModelArtists::LibraryItemModelArtists() {
	// TODO Auto-generated constructor stub

}

LibraryItemModelArtists::~LibraryItemModelArtists() {
	// TODO Auto-generated destructor stub
}

int LibraryItemModelArtists::rowCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
	return _artist_list.size();
}

int LibraryItemModelArtists::columnCount(const QModelIndex& parent) const{

	Q_UNUSED(parent);

	return 3;

	// title, artist, album, length, year

}


bool LibraryItemModelArtists::removeRows(int position, int rows, const QModelIndex & index)
{
	Q_UNUSED(index);

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {
		 _artist_list.removeAt(position);
	 }

	 endRemoveRows();
	 return true;
}



bool LibraryItemModelArtists::insertRows(int position, int rows, const QModelIndex & index)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {
		 Artist artist;
		 _artist_list.insert(position, artist);
	 }

	 endInsertRows();
	 return true;
}



QVariant LibraryItemModelArtists::data(const QModelIndex & index, int role) const
{
	 if (!index.isValid())
			 return QVariant();

		 if (index.row() >= _artist_list.size())
			 return QVariant();

		 if( role == Qt::WhatsThisRole && index.column() == 0 ){
			 Artist artist = _artist_list.at(index.row());
			 return artist.num_albums;
		 }


		 else if (role == Qt::WhatsThisRole && index.column() == 1){
			 Artist artist = _artist_list.at(index.row());
				return artist.toStringList();
		 }

		 else if (role == Qt::WhatsThisRole && index.column() == 2){
			 Artist artist = _artist_list.at(index.row());
			 return artist.num_songs;
		 }

		 else
			 return QVariant();
}



bool LibraryItemModelArtists::setData(const QModelIndex & index, const QVariant & value, int role)
{

	 if (index.isValid() && role == Qt::EditRole) {

		 QStringList list = value.toStringList();
		 Artist artist;

		 artist.fromStringList(list);

		 _artist_list.replace(index.row(), artist);

	     emit dataChanged(index, index);
	     return true;
	 }


	 return false;
}



Qt::ItemFlags LibraryItemModelArtists::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index);
}



QVariant LibraryItemModelArtists::headerData ( int section, Qt::Orientation orientation, int role ) const{

	 if (role != Qt::DisplayRole)
	         return QVariant();

	 if (orientation == Qt::Horizontal) {
		 switch (section) {
			 case 0: return QVariant();

			 case 1: return tr("Artist");
			 case 2: return tr("#Tracks");
			 default:
				 return QVariant();
		 }
	 }
	 return QVariant();

}

