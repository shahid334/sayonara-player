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

	return 2;

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

		 _artist_list.insert(position, "");
	 }

	 endInsertRows();
	 return true;
}



QVariant LibraryItemModelArtists::data(const QModelIndex & index, int role) const
{
	 if (!index.isValid())
			 return QVariant();

		 if (index.row() >= _artist_list.size() || index.column() == 0)
			 return QVariant();



		 if (role == Qt::WhatsThisRole){
				return _artist_list.at(index.row());

		 }

		 else
			 return QVariant();
}



bool LibraryItemModelArtists::setData(const QModelIndex & index, const QVariant & value, int role)
{

	 if (index.isValid() && role == Qt::EditRole) {

		 _artist_list.replace(index.row(), value.toString());

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
			 default:
				 return QVariant();
		 }
	 }
	 return QVariant();

}

