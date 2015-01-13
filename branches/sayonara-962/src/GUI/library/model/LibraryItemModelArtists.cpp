/* LibraryItemModelArtists.cpp */

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
 * LibraryItemModelArtists.cpp
 *
 *  Created on: Apr 26, 2011
 *      Author: luke
 */

#include "GUI/library/model/LibraryItemModelArtists.h"

#include <QAbstractListModel>
#include <QStringList>
#include <QDebug>

LibraryItemModelArtists::LibraryItemModelArtists(QList<ColumnHeader>& headers) : LibraryItemModel(headers) {

}

LibraryItemModelArtists::~LibraryItemModelArtists() {

}



int LibraryItemModelArtists::rowCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
	return _artists.size();
}




bool LibraryItemModelArtists::removeRows(int position, int rows, const QModelIndex & index)
{
	Q_UNUSED(index);

	_selected_rows.clear();

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 _artists.remove(position, rows);

	 endRemoveRows();
	 return true;
}



bool LibraryItemModelArtists::insertRows(int position, int rows, const QModelIndex & index)
{
	Q_UNUSED(index);

	_selected_rows.clear();

	beginInsertRows(QModelIndex(), position, position+rows-1);

	 Artist artist;
	 _artists.insert(position, rows, artist);

	 endInsertRows();
	 return true;
}



void LibraryItemModelArtists::remove_all_and_insert(int n){

	beginRemoveRows(QModelIndex(), 0, _artists.size() - 1);
		 _artists.clear();
	endRemoveRows();

	if(n <= 0) return;

	beginInsertRows(QModelIndex(), 0, n - 1);
		_artists.resize(n);
	 endInsertRows();
}





QVariant LibraryItemModelArtists::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
         return QVariant();

	 if (index.row() >= _artists.size())
         return QVariant();

	 if(role == Qt::WhatsThisRole) {

        int row = index.row();
        int col = index.column();

		Artist artist = _artists[row];
        int idx_col = calc_shown_col(col);

		switch(idx_col) {
            case COL_ARTIST_NAME:
                return artist.name;
            case COL_ARTIST_N_ALBUMS:
                return artist.num_albums;
            case COL_ARTIST_TRACKS:
                return artist.num_songs;

            default: return "";
        }
     }

 return QVariant();
}



bool LibraryItemModelArtists::setData(const QModelIndex & index, const QVariant & value, int role)
{

	 if (index.isValid() && role == Qt::EditRole) {

         Artist artist;
		 Artist::fromVariant(value, artist);

         if(artist.is_lib_selected && !_selected_rows.contains(index.row()))
            _selected_rows << index.row();

		 _artists.replace(index.row(), artist);


	     emit dataChanged(index, index);
	     return true;
	 }

	 return false;
}



Qt::ItemFlags LibraryItemModelArtists::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index);
}

QModelIndex LibraryItemModelArtists::getFirstRowIndexOf(QString substr) {
	if(_artists.isEmpty()) return this->index(-1, -1);
	if(_selected_rows.size() > 0)
		return getNextRowIndexOf(substr, _selected_rows[0]);
	else
		return getNextRowIndexOf(substr, 0);

}


QModelIndex	LibraryItemModelArtists::getNextRowIndexOf(QString substr, int row) {

	int len = _artists.size();
	if( len == 0 ) return this->index(-1, -1);

	for(int i=0; i<len; i++) {
		int row_idx = (i + row) % len;

		QString artist_name = _artists[row_idx].name;
        if( artist_name.startsWith("the ", Qt::CaseInsensitive) ||
			artist_name.startsWith("die ", Qt::CaseInsensitive) ) {
            artist_name = artist_name.right(artist_name.size() -4);
        }

		if(artist_name.startsWith(substr, Qt::CaseInsensitive) || artist_name.startsWith(substr, Qt::CaseInsensitive)) {
			return this->index(row_idx, 0);
        }
    }

	return this->index(-1, -1);
}


QModelIndex	LibraryItemModelArtists::getPrevRowIndexOf(QString substr, int row) {

	int len = _artists.size();
	if( len < row) row = len - 1;

	for(int i=0; i<len; i++) {

		if(row - i < 0) row = len - 1;
		int row_idx = (row-i) % len;

		QString artist_name = _artists[row_idx].name;
		if( artist_name.startsWith("the ", Qt::CaseInsensitive) ||
			artist_name.startsWith("die ", Qt::CaseInsensitive) ) {
			artist_name = artist_name.right(artist_name.size() -4);
		}

		if(artist_name.startsWith(substr, Qt::CaseInsensitive) || artist_name.startsWith(substr, Qt::CaseInsensitive)) {
			return this->index(row_idx, 0);
		}
	}

	return this->index(-1, -1);
}



