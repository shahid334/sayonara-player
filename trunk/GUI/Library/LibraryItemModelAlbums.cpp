/*
 * LibraryItemModelAlbums.cpp
 *
 *  Created on: Apr 26, 2011
 *      Author: luke
 */

#include "LibraryItemModelAlbums.h"

#include <QAbstractListModel>
#include <QStringList>
LibraryItemModelAlbums::LibraryItemModelAlbums() {
	// TODO Auto-generated constructor stub

}

LibraryItemModelAlbums::~LibraryItemModelAlbums() {
	// TODO Auto-generated destructor stub
}

int LibraryItemModelAlbums::rowCount(const QModelIndex & parent) const
{
	return _album_list.size();
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

		 _album_list.insert(position, "");
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



		 if (role == Qt::DisplayRole){
				return _album_list.at(index.row());

		 }

		 else
			 return QVariant();
}



bool LibraryItemModelAlbums::setData(const QModelIndex & index, const QVariant & value, int role)
{

	 if (index.isValid() && role == Qt::EditRole) {


		 _album_list.replace(index.row(), value.toString());

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


