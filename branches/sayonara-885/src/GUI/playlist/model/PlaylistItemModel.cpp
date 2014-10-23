/* PlaylistItemModel.cpp */

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
* PlaylistItemModel.cpp
 *
 *  Created on: Apr 8, 2011
 *      Author: luke
 */

#include "GUI/playlist/model/PlaylistItemModel.h"
#include "HelperStructs/MetaData.h"

#include <QList>
#include <QDebug>


using namespace std;

PlaylistItemModel::PlaylistItemModel(QObject* parent) : AbstractSearchListModel(parent) {
}

PlaylistItemModel::~PlaylistItemModel() {

}



int PlaylistItemModel::rowCount(const QModelIndex &parent) const{
	Q_UNUSED(parent);
    return _v_meta_data.size();
}

QVariant PlaylistItemModel::data(const QModelIndex &index, int role) const{

	if (!index.isValid()) {
		return QVariant();
	}

    if (index.row() >= _v_meta_data.size() || index.row() < 0) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		return QVariant();
	}

	if (role == Qt::WhatsThisRole) {
		return MetaData::toVariant( _v_meta_data[index.row()] );
	}

	else{
		return QVariant();
	}
}




Qt::ItemFlags PlaylistItemModel::flags(const QModelIndex &index = QModelIndex()) const{

	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool PlaylistItemModel::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {

	 if (index.isValid() && role == Qt::EditRole) {

         MetaData md;
         if(MetaData::fromVariant(value, md)) {
            _v_meta_data[index.row()] = md;
            emit dataChanged(index, index);
         }
	     return true;
	 }

	 return false;
}


bool PlaylistItemModel::insertRows(int position, int rows, const QModelIndex &index) {

	Q_UNUSED(index);


	beginInsertRows(QModelIndex(), position, position+rows-1);

        MetaDataList v_md_new;

        // copy old
        for(int i=0; i<position; i++) {
            v_md_new.push_back(_v_meta_data[i]);
        }

        // create new space
        for(int i=0; i<rows; i++) {
            MetaData md;
            v_md_new.push_back(md);
        }

        // copy old
        for(int i=position; i<_v_meta_data.size(); i++) {
            v_md_new.push_back(_v_meta_data[i]);
        }

        _v_meta_data = v_md_new;

	 endInsertRows();
	 return true;
}

bool PlaylistItemModel::removeRows(int position, int rows, const QModelIndex &index) {

	Q_UNUSED(index);

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

     MetaDataList v_md_new;

     for (int i=0; i<_v_meta_data.size(); i++) {

         if(i >= position &&
            i<=(position+rows-1)) continue;

         v_md_new.push_back(_v_meta_data[i]);
	 }


     _v_meta_data = v_md_new;

	 endRemoveRows();
	 return true;

}



void PlaylistItemModel::set_selected(QList<int>& rows) {
    _selected_rows = rows;
    for(int i=0; i<_v_meta_data.size(); i++) {
        _v_meta_data[i].pl_selected = rows.contains(i);
    }
}

bool PlaylistItemModel::is_selected(int row) const {
    return _selected_rows.contains(row);
}

void PlaylistItemModel::get_metadata(const QList<int>& rows, MetaDataList& v_md) {

    v_md.clear();
    foreach(int row, rows)
        v_md.push_back( this->_v_meta_data[row] );
}

#define ALBUM_SEARCH '%'
#define ARTIST_SEARCH '$'
#define JUMP ':'

QModelIndex PlaylistItemModel::getFirstRowIndexOf(QString substr) {

	if(_v_meta_data.empty()) return this->index(-1, -1);

	if(_selected_rows.size() > 0)
		return getNextRowIndexOf(substr, _selected_rows[0]);
	else
		return getNextRowIndexOf(substr, 0);

}

QModelIndex PlaylistItemModel::getPrevRowIndexOf(QString substr, int row) {

    int len = _v_meta_data.size();
	if(len < row) row = len - 1;

	// ALBUM
	if(substr.startsWith(ALBUM_SEARCH)) {
		substr.remove(ALBUM_SEARCH);

        for(int i=0; i<len; i++) {
			if(row - i < 0) row = len - 1;
			int row_idx = (row - i) % len;
			QString album = _v_meta_data[row_idx].album;
			if(album.startsWith(substr, Qt::CaseInsensitive)) {
				return this->index(row_idx, 0);
			}
		}
	}

	//ARTIST
	else if(substr.startsWith(ARTIST_SEARCH)) {
		substr.remove(ARTIST_SEARCH);

		for(int i=0; i<len; i++) {
			if(row - i < 0) row = len - 1;
			int row_idx = (row - i) % len;
			QString artist = _v_meta_data[row_idx].artist;
			if(artist.startsWith(substr, Qt::CaseInsensitive)) {
				return this->index(row_idx, 0);
			}
		}
	}

	// JUMP
	else if(substr.startsWith(JUMP)) {
		substr.remove(JUMP).trimmed();
		bool ok;
		int line = substr.toInt(&ok);
		if(ok && len > line) {
			return this->index(line, 0);
		}

		else return this->index(-1, -1);
	}

	// TITLE
	else {

		for(int i=0; i<len; i++) {
			if(row - i < 0) row = len - 1;
			int row_idx = (row - i) % len;
			QString title = _v_meta_data[row_idx].title;
			if(title.startsWith(substr, Qt::CaseInsensitive)) {
				return this->index(row_idx, 0);
			}
		}
	}

	return this->index(-1, -1);
}

QModelIndex PlaylistItemModel::getNextRowIndexOf(QString substr, int row) {
    int len = _v_meta_data.size();
	if(len < row) row = len - 1;
	// ALBUM
	if(substr.startsWith(ALBUM_SEARCH)) {
		substr.remove(ALBUM_SEARCH);

		for(int i=0; i< len; i++) {
			int row_idx = (i + row) % len;
			QString album = _v_meta_data[row_idx].album;
			if(album.startsWith(substr, Qt::CaseInsensitive)) {
				return this->index(row_idx, 0);
			}
		}
	}

	//ARTIST
	else if(substr.startsWith(ARTIST_SEARCH)) {
		substr.remove(ARTIST_SEARCH);


		for(int i=0; i< len; i++) {
			int row_idx = (i + row) % len;
			QString artist = _v_meta_data[row_idx].artist;
			if(artist.startsWith(substr, Qt::CaseInsensitive)) {
				return this->index(row_idx, 0);
			}
		}
	}

	// JUMP
	else if(substr.startsWith(JUMP)) {
		substr.remove(JUMP).trimmed();
		bool ok;
		int line = substr.toInt(&ok);
        if(ok && (_v_meta_data.size() > line) ){
			return this->index(line, 0);
		}

		else return this->index(-1, -1);
	}

	// TITLE
	else {

		for(int i=0; i< len; i++) {
			int row_idx = (i + row) % len;
			QString title = _v_meta_data[row_idx].title;
			if(title.startsWith(substr, Qt::CaseInsensitive)) {
				return this->index(row_idx, 0);
			}
		}
	}

	return this->index(-1, -1);
}



QMap<QChar, QString> PlaylistItemModel::getExtraTriggers() {
	QMap<QChar, QString> map;
	map.insert(ARTIST_SEARCH, tr("Artist"));
	map.insert(ALBUM_SEARCH, tr("Album"));
	map.insert(JUMP, tr("Goto row"));
	return map;
}
