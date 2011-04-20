/*
 * MyAbstractItemModel.h
 *
 *  Created on: Apr 7, 2011
 *      Author: luke
 */

#ifndef MYABSTRACTITEMMODEL_H_
#define MYABSTRACTITEMMODEL_H_

#include "HelperStructs/MetaData.h"

#include <QAbstractItemView>
#include <QList>
#include <QPair>
#include <QString>
#include <vector>


using namespace std;

//typedef QPair<QString, int> Row_Content;

class PlaylistItemView: public QAbstractItemView
 {
	Q_OBJECT
public:
	PlaylistItemView()  ;
	virtual ~PlaylistItemView();


	/*bool insertRows ( int row, int count, const QModelIndex& parent);
	bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() )


	int rowCount(const QModelIndex &parent = QModelIndex());

	QVariant data(const QModelIndex &index, int role);
	bool setData (const QModelIndex &index, const QVariant& value, int role = Qt::EditRole );*/

private:


	/*QList<Row_Content> m_rows;


	QList<Row_Content> m_data2set;*/

};

#endif /* MYABSTRACTITEMMODEL_H_ */
