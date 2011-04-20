/*
 * MyAbstractItemModel.cpp
 *
 *  Created on: Apr 7, 2011
 *      Author: luke
 */

#include "Playlist/PlaylistItemView.h"
#include "HelperStructs/MetaData.h"

#include <QAbstractItemView>
#include <QList>
#include <QPair>
#include <QVariant>
#include <QString>
#include <vector>


using namespace std;

PlaylistItemView::PlaylistItemView() : QAbstractItemView() {



}

PlaylistItemView::~PlaylistItemView() {

	// TODO Auto-generated destructor stub
}





/*
void MyAbstractItemModel::setData(vector<MetaData>& v_metadata){


	int idx= 0;
	for(vector<MetaData>::iterator it=v_metadata.begin(); it != v_metadata.end(); it++){

		QString info = "<b>" + it->title + "</b>\nby" + it->artist;
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
