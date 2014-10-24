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

#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"

#include "GUI/MyColumnHeader.h"
#include "GUI/library/model/LibraryItemModel.h"
#include "GUI/library/model/LibraryItemModelTracks.h"



LibraryItemModelTracks::LibraryItemModelTracks(QList<ColumnHeader>& headers) : LibraryItemModel(headers) {

}


LibraryItemModelTracks::~LibraryItemModelTracks() {

}



int LibraryItemModelTracks::rowCount(const QModelIndex &parent) const{

	Q_UNUSED(parent);
	return _tracklist.size();
}


QVariant LibraryItemModelTracks::data(const QModelIndex &index, int role) const{

	int row = index.row();
	int col = index.column();

	if (!index.isValid())
		 return QVariant();

	 if (row >= _tracklist.size())
		 return QVariant();

     int idx_col = calc_shown_col(col);

	 if (role == Qt::DisplayRole) {

         const MetaData& md = _tracklist.at(row);

		 switch(idx_col) {
			 case COL_TRACK_NUM:
				return QVariant( md.track_num );

			 case COL_TITLE:
				 return QVariant( md.title );

			 case COL_ARTIST:
				 return QVariant( md.artist );

			 case COL_LENGTH:
				 return QVariant( Helper::cvtMsecs2TitleLengthString(md.length_ms)  );

			 case COL_ALBUM:
                return QVariant(Helper::get_album_w_disc(md));

			 case COL_YEAR:
				 return QVariant(md.year);

			 case COL_BITRATE:
				return QVariant(md.bitrate);

             case COL_FILESIZE:
                return QVariant(md.filesize);
             case COL_TRACK_RATING:
                 return QVariant(md.rating);
			 default:
				return QVariant();
		 }
	 }

	 else if (role == Qt::TextAlignmentRole) {

          if (idx_col == COL_TRACK_NUM || idx_col == COL_BITRATE || idx_col == COL_LENGTH || idx_col == COL_YEAR || idx_col == COL_FILESIZE /*|| idx_col == COL_DISCNUMBER*/)
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

    int idx_column = index.column();
    int shown_col = calc_shown_col(idx_column);
	if(shown_col == COL_TRACK_RATING) {

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    return QAbstractItemModel::flags(index);
}

bool LibraryItemModelTracks::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {

	 if (index.isValid() && role == Qt::EditRole) {

		 int row = index.row();
		 int col = index.column();
		 int col_idx = calc_shown_col(col);

		 if(col_idx == COL_TRACK_RATING) {
             _tracklist[index.row()].rating = value.toInt();
         }

         else{

             MetaData md;
             if(!MetaData::fromVariant(value, md)) return false;

             if(md.is_lib_selected)
				 _selected_rows << row;

			 _tracklist[row] = md;

             emit dataChanged(index, index);
         }

	     return true;
	 }

	 return false;
}


bool LibraryItemModelTracks::insertRows(int position, int rows, const QModelIndex &index) {

    Q_UNUSED(index);

	_selected_rows.clear();

	beginInsertRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {

		 MetaData md;
		 _tracklist.insert(position, md);
	 }

	 endInsertRows();

	 return true;
}

bool LibraryItemModelTracks::removeRows(int position, int rows, const QModelIndex &index) {

	Q_UNUSED(index);

	_selected_rows.clear();

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 for (int row = 0; row < rows; ++row) {
		 _tracklist.removeAt(position);
	 }

	 endRemoveRows();

	 return true;

}

QModelIndex	LibraryItemModelTracks::getFirstRowIndexOf(QString substr) {
	if(_tracklist.isEmpty()) return this->index(-1, -1);

	if(_selected_rows.size() > 0)
		return getNextRowIndexOf(substr, _selected_rows[0]);
	else
		return getNextRowIndexOf(substr, 0);
}

QModelIndex LibraryItemModelTracks::getNextRowIndexOf(QString substr, int row) {

    int len = _tracklist.size();
	if(len == 0) return this->index(-1, -1);

	for(int i=0; i< len; i++) {
		int row_idx = (i + row) % len;
		QString title = _tracklist[row_idx].title;
		if(title.startsWith(substr, Qt::CaseInsensitive)) {
			return this->index(row_idx, 0);
		}
	}

	return this->index(-1, -1);
}

QModelIndex LibraryItemModelTracks::getPrevRowIndexOf(QString substr, int row) {

    int len = _tracklist.size();
	if(len < row) row = len - 1;
	for(int i=0; i< len; i++) {
		if(row - i < 0) row = len - 1;
		int row_idx = (row - i) % len;
		QString title = _tracklist[row_idx].title;
		if(title.startsWith(substr, Qt::CaseInsensitive)) {
			return this->index(row_idx, 0);
		}
	}

	return this->index(-1, -1);
}



