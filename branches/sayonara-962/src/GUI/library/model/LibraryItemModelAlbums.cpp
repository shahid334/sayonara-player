/* LibraryItemModelAlbums.cpp */

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
 * LibraryItemModelAlbums.cpp
 *
 *  Created on: Apr 26, 2011
 *      Author: luke
 */

#include "GUI/library/model/LibraryItemModel.h"
#include "GUI/library/model/LibraryItemModelAlbums.h"
#include "HelperStructs/Helper.h"


LibraryItemModelAlbums::LibraryItemModelAlbums(QList<ColumnHeader>& headers) :
	LibraryItemModel(headers) {

}

LibraryItemModelAlbums::~LibraryItemModelAlbums() {

}



int LibraryItemModelAlbums::rowCount(const QModelIndex & parent) const
{	Q_UNUSED(parent);
	return _albums.size();
}


bool LibraryItemModelAlbums::removeRows(int position, int rows, const QModelIndex & index)
{
	Q_UNUSED(index);
	_selected_rows.clear();

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {
		 _albums.remove(position);
	 }

	 endRemoveRows();
	 return true;
}



bool LibraryItemModelAlbums::insertRows(int position, int rows, const QModelIndex & index)
{
	Q_UNUSED(index);

	_selected_rows.clear();

	beginInsertRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {

		 Album album;
		 _albums.insert(position, album);
	 }

	 endInsertRows();
	 return true;
}



QVariant LibraryItemModelAlbums::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		 return QVariant();

	if (index.row() >= _albums.size())
		 return QVariant();

	// qDebug() << "Edit Role= " << Qt::EditRole << " my role = " << role;
	 if(role == Qt::DisplayRole || role==Qt::EditRole) {

		int row = index.row();
		int col = index.column();

		const Album& album = _albums[row];

		int idx_col = calc_shown_col(col);

		 switch(idx_col) {
			 case COL_ALBUM_SAMPLER:
				return album.is_sampler;
			 case COL_ALBUM_N_SONGS:
				 return album.num_songs;
			 case COL_ALBUM_YEAR:
				 return album.year;
			 case COL_ALBUM_NAME:
				 return album.name;
			 case COL_ALBUM_DURATION:
				return Helper::cvt_ms_to_string(album.length_sec * 1000, true, false);
			 case COL_ALBUM_RATING:
				return album.rating;


			default: return "";
		 }
	 }

	 return QVariant();
}

bool LibraryItemModelAlbums::setData(const QModelIndex & index, const QVariant & value, int role)
{

	 if (index.isValid() && role == Qt::EditRole) {

		 int row = index.row();
		 int col = index.column();
		 int col_idx = calc_shown_col(col);

		 if(col_idx == COL_ALBUM_RATING) {
			 _albums[row].rating = value.toInt();
         }

         else {

             Album album;
			 if(!Album::fromVariant(value, album)) return false;

			 if(album.is_lib_selected && !_selected_rows.contains(row)){
				_selected_rows << row;
             }

			 _albums[row] = album;

              emit dataChanged(index, index);
		 }

	     return true;
	 }

	 return false;
}



Qt::ItemFlags LibraryItemModelAlbums::flags(const QModelIndex & index) const
{
    if (!index.isValid()) {
		return Qt::ItemIsEnabled;
    }

    int col = index.column();
    int idx_col = calc_shown_col(col);

    if(idx_col == COL_ALBUM_RATING){
        return (QAbstractItemModel::flags(index) | Qt::ItemIsEditable);
    }

    return QAbstractItemModel::flags(index);
}


void LibraryItemModelAlbums::sort(int column, Qt::SortOrder order) {

	Q_UNUSED(column);
	Q_UNUSED(order);
}

QModelIndex LibraryItemModelAlbums::getFirstRowIndexOf(QString substr) {
	if(_albums.isEmpty()) {
        return this->index(-1, -1);
    }

	if(_selected_rows.size() > 0) {
		return getNextRowIndexOf(substr, _selected_rows[0]);
	}

    else{
        return getNextRowIndexOf(substr, 0);
    }
}

QModelIndex LibraryItemModelAlbums::getNextRowIndexOf(QString substr, int row) {

	int len = _albums.size();
    if(len == 0)  {
        return this->index(-1, -1);
    }

	for(int i=0; i<len; i++) {
        int row_idx = (i + row) % len;

		QString album_name = _album_list[row_idx].name;
		if( album_name.startsWith("the ", Qt::CaseInsensitive) ||
			album_name.startsWith("die ", Qt::CaseInsensitive) ) {
			album_name = album_name.right(album_name.size() -4);
		}
        if( album_name.startsWith(substr, Qt::CaseInsensitive ) ||
            album_name.startsWith(substr, Qt::CaseInsensitive )){
                return this->index(row_idx, 0);
        }
	}

	return this->index(-1, -1);
}

QModelIndex LibraryItemModelAlbums::getPrevRowIndexOf(QString substr, int row) {

	int len = _albums.size();
    if(len < row){
        row = len - 1;
    }

	for(int i=0; i<len; i++) {
        int row_idx;
        QString album_name;

        if(row - i < 0){
            row = len - 1;
        }

        row_idx = (row-i) % len;
        album_name = _album_list[row_idx].name;

        if( album_name.startsWith("the ", Qt::CaseInsensitive) ||
			album_name.startsWith("die ", Qt::CaseInsensitive) ) {
			album_name = album_name.right(album_name.size() -4);
		}

        if( album_name.startsWith(substr, Qt::CaseInsensitive) ||
            album_name.startsWith(substr, Qt::CaseInsensitive)){
            return this->index(row_idx, 0);
        }
	}

	return this->index(-1, -1);
}


QList<quint8> LibraryItemModelAlbums::get_discnumbers(const QModelIndex& idx) {

    if(!idx.isValid()) {
        return QList<quint8>();
    }

    if( idx.row() < 0 ||
		idx.row() >= _albums.size())
    {
        return QList<quint8>();
    }

	return _albums[idx.row()].discnumbers;
}
