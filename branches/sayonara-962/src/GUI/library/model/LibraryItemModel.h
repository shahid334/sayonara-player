/* LibraryItemModel.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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


#ifndef LIBRARYITEMMODEL_H_
#define LIBRARYITEMMODEL_H_



#define COL_ALBUM_MACROS
#define COL_ALBUM_SAMPLER 0
#define COL_ALBUM_NAME 1
#define COL_ALBUM_DURATION 2
#define COL_ALBUM_N_SONGS 3
#define COL_ALBUM_YEAR 4
#define COL_ALBUM_RATING 5

#define COL_ARTIST_MACROS
#define COL_ARTIST_N_ALBUMS 0
#define COL_ARTIST_NAME 1
#define COL_ARTIST_TRACKS 2

#define COL_TRACK_NUM 0
#define COL_TITLE 1
#define COL_ARTIST 2
#define COL_ALBUM 3
//#define COL_DISCNUMBER 4
#define COL_YEAR 4
#define COL_LENGTH 5
#define COL_BITRATE 6
#define COL_FILESIZE 7
#define COL_TRACK_RATING 8

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "GUI/MyColumnHeader.h"
#include "GUI/AbstractSearchModel.h"


class LibraryItemModel : public AbstractSearchTableModel{

	Q_OBJECT

public:

	LibraryItemModel(QList<ColumnHeader>& headers);
	virtual ~LibraryItemModel();


    QStringList 	get_header_names() const;
    int 			get_n_cols() const;
    int 			calc_shown_col(int col) const;
    bool			is_col_shown(int col) const;
	virtual bool 	is_selected(int row) const;
	virtual QList<int>  get_selected() const;
	virtual void    set_selected(const QList<int>& rows);


	/** Overloaded from QAbstractTableModel **/
    QVariant 	headerData ( int section, Qt::Orientation orientation, int role ) const;
    int 		columnCount(const QModelIndex& parent=QModelIndex()) const;
    bool 		insertColumns(int position, int cols, const QModelIndex &index=QModelIndex());
    bool 		removeColumns(int position, int cols, const QModelIndex &index=QModelIndex());
	virtual void remove_all_and_insert(int n)=0;


	virtual QMap<QChar, QString> getExtraTriggers();

    void set_new_header_names(QStringList& lst);




private:
    QStringList				_header_names;
    int						_n_all_cols;
    bool*                   _cols_active;

protected:
    QList<int>				_selected_rows;

};

#endif /* LIBRARYITEMMODEL_H_ */
