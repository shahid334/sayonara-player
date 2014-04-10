/* AlternateCoverItemModel.h */

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
 * AlternateCoverItemModel.h
 *
 *  Created on: Jul 1, 2011
 *      Author: luke
 */

#ifndef ALTERNATECOVERITEMMODEL_H_
#define ALTERNATECOVERITEMMODEL_H_

#include <QObject>
#include <QList>
#include <QModelIndex>
#include <QAbstractTableModel>
#include <QStringList>


class AlternateCoverItemModel : public QAbstractTableModel {

Q_OBJECT
public:
    AlternateCoverItemModel(QObject* parent);
    virtual ~AlternateCoverItemModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());


private:
	QStringList _pathlist;


};
#endif
