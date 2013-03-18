/* PlaylistItemView.h */

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
