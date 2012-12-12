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

#include "LibraryItemModelAlbums.h"
#include <HelperStructs/MetaData.h>
#include <HelperStructs/Helper.h>

#include <QAbstractListModel>
#include <QStringList>
#include <QDebug>
LibraryItemModelAlbums::LibraryItemModelAlbums() {

    _headerdata.push_back("#");
    _headerdata.push_back("Album");
    _headerdata.push_back("Duration");
    _headerdata.push_back("Tracks");
    _headerdata.push_back("Year");

}

LibraryItemModelAlbums::~LibraryItemModelAlbums() {
	// TODO Auto-generated destructor stub
}


void LibraryItemModelAlbums::set_selected(QList<int>& rows){
    _selected_rows = rows;
}

bool LibraryItemModelAlbums::is_selected(int row){
    return _selected_rows.contains(row);
}


int LibraryItemModelAlbums::rowCount(const QModelIndex & parent) const
{	Q_UNUSED(parent);
	return _album_list.size();
}

int LibraryItemModelAlbums::columnCount(const QModelIndex& parent) const{

    Q_UNUSED(parent);

    int n_active = 0;
    for(int i=0; i<N_ALBUM_COLS; i++){
        if(_cols_active[i]) n_active++;
    }

    return n_active;
}


bool LibraryItemModelAlbums::removeRows(int position, int rows, const QModelIndex & index)
{
	Q_UNUSED(index);

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {
		 _album_list.removeAt(position);
	 }



	 endRemoveRows();
	 return true;
}



bool LibraryItemModelAlbums::insertRows(int position, int rows, const QModelIndex & index)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {

		 Album album;
		 _album_list.insert(position, album);
	 }

	 endInsertRows();
	 return true;
}


bool LibraryItemModelAlbums::insertColumns(int position, int cols, const QModelIndex &index){

    beginInsertColumns(QModelIndex(), position, position+cols-1);

    for(int i=position; i<position+cols; i++){

        _cols_active[i] = true;
    }

    endInsertColumns();
    return true;
}


bool LibraryItemModelAlbums::removeColumns(int position, int cols, const QModelIndex &index){

    beginRemoveColumns(QModelIndex(), position, position+cols-1);
    for(int i=0; i<N_ALBUM_COLS; i++){
        _cols_active[i] = false;
    }

    endRemoveColumns();
    return true;
}



QVariant LibraryItemModelAlbums::data(const QModelIndex & index, int role) const
{
	 if (!index.isValid())
			 return QVariant();

		 if (index.row() >= _album_list.size())
			 return QVariant();

         if(role == Qt::WhatsThisRole){

            int row = index.row();
            int col = index.column();

            Album album = _album_list[row];
            int idx_col = calc_shown_col(col);

             switch(idx_col){
                 case COL_ALBUM_SAMPLER:
                    return album.is_sampler;
                 case COL_ALBUM_N_SONGS:
                     return album.num_songs;
                 case COL_ALBUM_YEAR:
                     return album.year;
                 case COL_ALBUM_NAME:
                     return album.name;

                 case COL_ALBUM_DURATION:
                    return Helper::cvtMsecs2TitleLengthString(album.length_sec * 1000, true, false);

                default: return "";
             }
         }

         return QVariant();
}



bool LibraryItemModelAlbums::setData(const QModelIndex & index, const QVariant & value, int role)
{

	 if (index.isValid() && role == Qt::EditRole) {

		 if(index.column() == 1) {
			 QStringList list = value.toStringList();

			 Album album;
			 album.fromStringList(list);
             if(album.is_lib_selected)
                _selected_rows << index.row();

			 _album_list.replace(index.row(), album);
		 }

	     emit dataChanged(index, index);
	     return true;
	 }

	 return false;
}



Qt::ItemFlags LibraryItemModelAlbums::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index);
}



QVariant LibraryItemModelAlbums::headerData ( int section, Qt::Orientation orientation, int role ) const{

	 if (role != Qt::DisplayRole)
	         return QVariant();

     int idx_col = calc_shown_col(section);
     if (orientation == Qt::Horizontal)
         return _headerdata[idx_col];
	 return QVariant();

}


void LibraryItemModelAlbums::sort(int column, Qt::SortOrder order){

	Q_UNUSED(column);
	Q_UNUSED(order);
}


int LibraryItemModelAlbums::calc_shown_col(int col) const {
    int idx_col = 0;
    int n_true = -1;
    for(idx_col=0; idx_col<N_ALBUM_COLS; idx_col++){
        if(_cols_active[idx_col]) n_true++;
        if(n_true == col) break;
    }

    return idx_col;
}

bool LibraryItemModelAlbums::is_col_shown(int col) const{
    return _cols_active[col];
}

QStringList  LibraryItemModelAlbums::get_header_names(){
    return _headerdata;
}
