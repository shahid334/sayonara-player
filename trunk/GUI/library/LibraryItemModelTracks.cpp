/* LibraryItemModelTracks.cpp */

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
 * LibraryItemModelTracks.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <HelperStructs/MetaData.h>
#include <HelperStructs/Helper.h>
#include <HelperStructs/CSettingsStorage.h>

#include "LibraryItemModelTracks.h"


LibraryItemModelTracks::LibraryItemModelTracks(QObject* parent) {

	Q_UNUSED(parent);


    _headerdata.push_back("#");
	_headerdata.push_back("Title");
	_headerdata.push_back("Artist");
	_headerdata.push_back("Album");
	_headerdata.push_back("Year");
	_headerdata.push_back("Length");
	_headerdata.push_back("Bitrate");
    _headerdata.push_back("Filesize");



}


LibraryItemModelTracks::~LibraryItemModelTracks() {
	// TODO Auto-generated destructor stub
}



void LibraryItemModelTracks::set_selected(QList<int>& rows){
    _selected_rows = rows;
}

bool LibraryItemModelTracks::is_selected(int row){
    return _selected_rows.contains(row);
}

int LibraryItemModelTracks::rowCount(const QModelIndex &parent) const{

	Q_UNUSED(parent);
	return _tracklist.size();
}

int LibraryItemModelTracks::columnCount(const QModelIndex& parent) const{

	Q_UNUSED(parent);

    int n_active = 0;
    for(int i=0; i<N_COLS; i++){
        if(_cols_active[i]) n_active++;
    }

    return n_active;

	// title, artist, album, length, year

}

QVariant LibraryItemModelTracks::data(const QModelIndex &index, int role) const{

	int row = index.row();
	int col = index.column();

	if (!index.isValid())
		 return QVariant();

	 if (row >= _tracklist.size())
		 return QVariant();

     int idx_col = calc_shown_col(col);

	 if (role == Qt::DisplayRole){

		 MetaData md = _tracklist.at(row);

         switch(idx_col){
			 case COL_TRACK_NUM:
				return QVariant( md.track_num );

			 case COL_TITLE:
				 return QVariant( md.title );

			 case COL_ARTIST:
				 return QVariant( md.artist );

			 case COL_LENGTH:
				 return QVariant( Helper::cvtMsecs2TitleLengthString(md.length_ms)  );

			 case COL_ALBUM:
				 return QVariant(md.album);

			 case COL_YEAR:
				 return QVariant(md.year);

			 case COL_BITRATE:
				return QVariant(md.bitrate);

             case COL_FILESIZE:
                return QVariant(md.filesize);
			 default:
				return QVariant();
		 }
	 }

	 else if (role == Qt::TextAlignmentRole){

         qDebug() << "idx_col = " << idx_col;
          if (idx_col == COL_TRACK_NUM || idx_col == COL_BITRATE || idx_col == COL_LENGTH || idx_col == COL_YEAR || idx_col == COL_FILESIZE)
          {
              return Qt::AlignRight + Qt::AlignVCenter;
          }

          else return Qt::AlignLeft + Qt::AlignVCenter;
	 }

	 else
		 return QVariant();
}




Qt::ItemFlags LibraryItemModelTracks::flags(const QModelIndex &index = QModelIndex()) const{

	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool LibraryItemModelTracks::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole){

	 if (index.isValid() && role == Qt::EditRole) {

		 MetaData md;
		 if(!MetaData::fromVariant(value, md)) return false;

         if(md.is_lib_selected)
             _selected_rows << index.row();
		 _tracklist.replace(index.row(), md);

	     emit dataChanged(index, index);
	     return true;
	 }

	 return false;
}


bool LibraryItemModelTracks::insertRows(int position, int rows, const QModelIndex &index){

    Q_UNUSED(index);



	beginInsertRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {

		 MetaData md;
		 _tracklist.insert(position, md);
	 }

	 endInsertRows();

	 return true;
}

bool LibraryItemModelTracks::removeRows(int position, int rows, const QModelIndex &index){

	Q_UNUSED(index);

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {
		 _tracklist.removeAt(position);
	 }

	 endRemoveRows();

	 return true;

}


bool LibraryItemModelTracks::insertColumns(int position, int cols, const QModelIndex &index){

    beginInsertColumns(QModelIndex(), position, position+cols-1);

    for(int i=position; i<position+cols; i++){

        _cols_active[position] = true;
    }


    endInsertColumns();
    return true;
}


bool LibraryItemModelTracks::removeColumns(int position, int cols, const QModelIndex &index){

    beginRemoveColumns(QModelIndex(), position, position+cols-1);
    for(int i=0; i<N_COLS; i++){
        _cols_active[i] = false;
    }


    endRemoveColumns();
    return true;
}


QVariant LibraryItemModelTracks::headerData ( int col, Qt::Orientation orientation, int role ) const{

	 if (role != Qt::DisplayRole)
	         return QVariant();

     int idx_col = calc_shown_col(col);

	 if (orientation == Qt::Horizontal) {
         return _headerdata[idx_col];
	 }
	 return QVariant();

}


int LibraryItemModelTracks::calc_shown_col(int col) const {
    int idx_col = 0;
    int n_true = -1;
    for(idx_col=0; idx_col<N_COLS; idx_col++){
        if(_cols_active[idx_col]) n_true++;
        if(n_true == col) break;
    }

    return idx_col;
}

bool LibraryItemModelTracks::is_col_shown(int col) const{
    return _cols_active[col];
}

QStringList  LibraryItemModelTracks::get_header_names(){
    return _headerdata;
}
