/* LibraryItemModelTracks.cpp */

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
 * LibraryItemModelTracks.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <vector>
#include <HelperStructs/MetaData.h>
#include <HelperStructs/Helper.h>


#include "LibraryItemModelTracks.h"

using namespace std;

LibraryItemModelTracks::LibraryItemModelTracks(QObject* parent) {

	Q_UNUSED(parent);

	_headerdata.push_back("Title");
	_headerdata.push_back("Artist");
	_headerdata.push_back("Album");
	_headerdata.push_back("Year");
	_headerdata.push_back("Length");
	_headerdata.push_back("Bitrate");

}


LibraryItemModelTracks::~LibraryItemModelTracks() {
	// TODO Auto-generated destructor stub
}





int LibraryItemModelTracks::rowCount(const QModelIndex &parent) const{

	Q_UNUSED(parent);
	return _tracklist.size();
}

int LibraryItemModelTracks::columnCount(const QModelIndex& parent) const{

	Q_UNUSED(parent);

	return 7;

	// title, artist, album, length, year

}

QVariant LibraryItemModelTracks::data(const QModelIndex &index, int role) const{
	 if (!index.isValid())
		 return QVariant();

	 if (index.row() >= _tracklist.size())
		 return QVariant();



	 if (role == Qt::DisplayRole){

		 MetaData md = _tracklist.at(index.row());

		 switch(index.column()){
			 case COL_TRACK_NUM:
				return QVariant( md.track_num );

			 case COL_TITLE:
				 return QVariant( md.title );

			 case COL_ARTIST:
				 return QVariant( md.artist );

			 case COL_LENGTH:
				 return QVariant( Helper::cvtMsecs2TitleLengthString(md.length_ms)  );

			 case COL_ALBUM:
				 return QVariant(md.album);

			 case COL_YEAR:
				 return QVariant(md.year);

			 case COL_BITRATE:
				return QVariant(md.bitrate);
			 default:
					return QVariant();
		 }
	 }

	 else
		 return QVariant();
}




Qt::ItemFlags LibraryItemModelTracks::flags(const QModelIndex &index = QModelIndex()) const{

	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool LibraryItemModelTracks::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole){



	 if (index.isValid() && role == Qt::EditRole) {

		 MetaData md_toSet;
		 QStringList val2list = value.toStringList();
		 md_toSet.fromStringList( val2list );

		 _tracklist.replace(index.row(), md_toSet);

	     emit dataChanged(index, index);
	     return true;
	 }

	 return false;
}


bool LibraryItemModelTracks::insertRows(int position, int rows, const QModelIndex &index){

	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {

		 MetaData md;
		 _tracklist.insert(position, md);
	 }

	 endInsertRows();
	 return true;
}

bool LibraryItemModelTracks::removeRows(int position, int rows, const QModelIndex &index){

	Q_UNUSED(index);

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {
		 _tracklist.removeAt(position);
	 }

	 endRemoveRows();
	 return true;

}


QVariant LibraryItemModelTracks::headerData ( int section, Qt::Orientation orientation, int role ) const{

	 if (role != Qt::DisplayRole)
	         return QVariant();

	 if (orientation == Qt::Horizontal) {
		 switch (section) {
			 case COL_TITLE: return tr("Title");
			 case COL_ARTIST: return tr("Artist");
			 case COL_ALBUM: return tr("Album");
			 case COL_LENGTH: return tr("Length");
			 case COL_YEAR: return tr("Year");
			 case COL_BITRATE: return tr("Bitrate");

			 default:
				 return QVariant();
		 }
	 }
	 return QVariant();

}
