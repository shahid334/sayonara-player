/* LibraryItemModelTracks.h */

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
 * LibraryItemModelTracks.h
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#ifndef LIBRARYITEMMODELTRACKS_H_
#define LIBRARYITEMMODELTRACKS_H_


#include <QObject>
#include <QStringList>
#include <QList>

#include "HelperStructs/MetaData.h"
#include "GUI/library/models/LibraryItemModel.h"
#include "GUI/MyColumnHeader.h"



class LibraryItemModelTracks : public LibraryItemModel {

Q_OBJECT

public:
	LibraryItemModelTracks(QList<ColumnHeader>& headers);

	virtual ~LibraryItemModelTracks();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

	virtual int			getFirstRowOf(QString substr);
    virtual void set_selected(QList<int>& rows);


private:
	QList<MetaData>			_tracklist;
};

#endif /* LIBRARYITEMMODELTRACKS_H_ */
