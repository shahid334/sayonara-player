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
	// TODO Auto-generated constructor stub

}

LibraryItemModelTracks::LibraryItemModelTracks(const vector<MetaData>& v_metadata){

	for( int i=0; i<v_metadata.size(); i++){

		MetaData md = v_metadata.at(i);


		QStringList list;
		list.push_back(md.title);
		list.push_back(md.artist);
		list.push_back(md.album);

		int min, sec;
		Helper::cvtSecs2MinAndSecs(md.length_ms/1000, &min, &sec);
		QString length = QString::fromStdString(Helper::cvtNum2String(min, 2)) + ":" + QString::fromStdString(Helper::cvtNum2String(sec, 2));
		list.push_back(length);
		list.push_back(QString::number(md.year));

		_tracklist.push_back(list);
	}
}

LibraryItemModelTracks::~LibraryItemModelTracks() {
	// TODO Auto-generated destructor stub
}





int LibraryItemModelTracks::rowCount(const QModelIndex &parent) const{
	return _tracklist.size();
}

int LibraryItemModelTracks::columnCount(const QModelIndex& parent) const{

	return 5;

	// title, artist, album, length, year

}

QVariant LibraryItemModelTracks::data(const QModelIndex &index, int role) const{
	 if (!index.isValid())
		 return QVariant();

	 if (index.row() >= _tracklist.size())
		 return QVariant();



	 if (role == Qt::DisplayRole){
			return _tracklist.at(index.row()).at(index.column());

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

	 qDebug() << "setting up data, " << "index valid? " << index.isValid() << ", index.row() = " << index.row();

	 if (index.isValid() && role == Qt::EditRole) {

		 QStringList list = _tracklist.at(index.row());
		 for(int i=0; i<5; i++)
			 list.replace(index.column() + i, value.toList().at(i).toString());
		 _tracklist.replace(index.row(), list);

	     emit dataChanged(index, index);
	     return true;
	 }

	 return false;
}


bool LibraryItemModelTracks::insertRows(int position, int rows, const QModelIndex &index){

	beginInsertRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {

		 QStringList list;
		 for(int i=0; i<5; i++) list.append("");
		 _tracklist.insert(position, list);
	 }

	 endInsertRows();
	 return true;
}

bool LibraryItemModelTracks::removeRows(int position, int rows, const QModelIndex &index){

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {
		 _tracklist.removeAt(position);
	 }

	 endRemoveRows();
	 return true;

}

