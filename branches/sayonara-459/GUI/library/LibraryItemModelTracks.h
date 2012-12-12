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

#ifndef COL_MACROS
    #define COL_MACROS
    #define COL_TRACK_NUM 0
    #define COL_TITLE 1
    #define COL_ALBUM 2
    #define COL_ARTIST 3
    #define COL_YEAR 4
    #define COL_LENGTH 5
    #define COL_BITRATE 6
    #define N_COLS 7
#endif

#include <QObject>
#include <QStringList>
#include <QAbstractTableModel>

#include <HelperStructs/MetaData.h>


class LibraryItemModelTracks : public QAbstractTableModel {

Q_OBJECT

public:
	LibraryItemModelTracks(QObject* parent=0);

	virtual ~LibraryItemModelTracks();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

    bool insertColumns(int position, int cols, const QModelIndex &index=QModelIndex());
    bool removeColumns(int position, int cols, const QModelIndex &index=QModelIndex());

	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    bool is_selected(int row);
    void set_selected(QList<int>& rows);

    int calc_shown_col(int col) const;
    bool is_col_shown(int col) const;


private:
	QList<MetaData>			_tracklist;
	QStringList				_headerdata;
    QList<int>              _selected_rows;
    bool                     _cols_active[N_COLS];
};

#endif /* LIBRARYITEMMODELTRACKS_H_ */
