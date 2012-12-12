/* LibraryItemModelArtists.h */

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
 * LibraryItemModelArtistts.h
 *
 *  Created on: Apr 26, 2011
 *      Author: luke
 */

#ifndef LIBRARYITEMMODELARTISTS_H_
#define LIBRARYITEMMODELARTISTS_H_

#include <HelperStructs/MetaData.h>
#include <QAbstractTableModel>
#include <QStringList>


#ifndef COL_ARTIST_MACROS
    #define COL_ARTIST_MACROS
    #define COL_ARTIST_N_ALBUMS 0
    #define COL_ARTIST_NAME 1
    #define COL_ARTIST_TRACKS 2
    #define N_ARTIST_COLS 3
#endif

class LibraryItemModelArtists : public QAbstractTableModel {

Q_OBJECT
public:
	LibraryItemModelArtists();
	virtual ~LibraryItemModelArtists();

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
    QStringList get_header_names();



private:
	QList<Artist> _artist_list;
    QList<int> _selected_rows;

    QStringList				_headerdata;
    bool                    _cols_active[N_ARTIST_COLS];

};


#endif /* LIBRARYITEMMODELARTISTS_H_ */
