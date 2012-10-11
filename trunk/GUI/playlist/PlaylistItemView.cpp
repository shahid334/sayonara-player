/* PlaylistItemView.cpp */

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
 * MyAbstractItemModel.cpp
 *
 *  Created on: Apr 7, 2011
 *      Author: luke
 */

#include "GUI/Playlist/PlaylistItemView.h"
#include "HelperStructs/MetaData.h"

#include <QAbstractItemView>
#include <QList>
#include <QPair>
#include <QVariant>
#include <QString>


PlaylistItemView::PlaylistItemView() : QAbstractItemView() {



}

PlaylistItemView::~PlaylistItemView() {

	// TODO Auto-generated destructor stub
}





/*
void MyAbstractItemModel::setData(MetaDataList& v_metadata){


	int idx= 0;
	foreach(MetaData md, v_metadata){

		QString info = "<b>" + md.title + "</b>\nby" + md.artist;
		QPair<QString, int> pair(info, idx);
		m_data2set.push_back(pair);

		idx++;
	}
}



bool MyAbstractItemModel::insertRows(int row, int count, const QModelIndex& parent){

	if(rowCount()<row)
			 row = rowCount()+1;

	QPair<QString, int> pair(QString::null, 0);

	beginInsertRows(QModelIndex(), row, row + count);

	for(int i=0; i<count; i++){

		m_rows.insert(row+i, pair);
	}

	endInsertRows();

	return true;
}


bool MyAbstractItemModel::removeRows(int row, int count, const QModelIndex& parent){



	return true;
}


int MyAbstractItemModel::rowCount(const QModelIndex &parent = QModelIndex()) const {
	return 5;
}



bool MyAbstractItemModel::setData (const QModelIndex &index, const QVariant& value, int role = Qt::EditRole){



	return true;
}




QVariant MyAbstractItemModel::data(const QModelIndex &index, int role) const {
     if (!index.isValid())
         return QVariant();

     if (role == Qt::TextColorRole)
        return QColor(QColor::colorNames().at(index.row()));

     if (role == Qt::DisplayRole)
         return QString("Item %1").arg(index.row() + 1);

 if( role == Qt::)

     else


         return QVariant();
  }

*/
