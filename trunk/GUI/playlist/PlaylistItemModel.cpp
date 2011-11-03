/* PlaylistItemModel.cpp */

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
* PlaylistItemModel.cpp
 *
 *  Created on: Apr 8, 2011
 *      Author: luke
 */

#include "GUI/playlist/PlaylistItemModel.h"
#include "HelperStructs/MetaData.h"

#include <QList>
#include <QDebug>


using namespace std;

PlaylistItemModel::PlaylistItemModel(QObject* parent) : QAbstractListModel(parent) {
}

PlaylistItemModel::~PlaylistItemModel() {
	// TODO Auto-generated destructor stub
}



int PlaylistItemModel::rowCount(const QModelIndex &parent = QModelIndex()) const{
	Q_UNUSED(parent);
	return _labellist.size();
}

QVariant PlaylistItemModel::data(const QModelIndex &index, int role) const{
	 if (!index.isValid())
		 return QVariant();

	 if (index.row() >= _labellist.size())
		 return QVariant();

	 if (role == Qt::DisplayRole){

		return QVariant();

	 }


	 if (role == Qt::WhatsThisRole){
			return _labellist.at(index.row());
	 }

	 else
		 return QVariant();
}




Qt::ItemFlags PlaylistItemModel::flags(const QModelIndex &index = QModelIndex()) const{

	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool PlaylistItemModel::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole){

	 if (index.isValid() && role == Qt::EditRole) {

		 _labellist.replace(index.row(), value.toStringList());
	     emit dataChanged(index, index);
	     return true;
	 }

	 return false;
}


bool PlaylistItemModel::insertRows(int position, int rows, const QModelIndex &index){

	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {

		 QStringList lst;
		 _labellist.insert(position, lst);
	 }

	 endInsertRows();
	 return true;
}

bool PlaylistItemModel::removeRows(int position, int rows, const QModelIndex &index){

	Q_UNUSED(index);

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {
		 _labellist.removeAt(position);
	 }

	 endRemoveRows();
	 return true;

}



