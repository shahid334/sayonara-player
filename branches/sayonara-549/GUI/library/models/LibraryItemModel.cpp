/*
 * LibraryItemModel.cpp
 *
 *  Created on: 19.12.2012
 *      Author: lugmair
 */

#include "GUI/library/models/LibraryItemModel.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QModelIndex>


LibraryItemModel::LibraryItemModel(QList<ColumnHeader>& headers){

		_n_all_cols = headers.size();
		_cols_active = new bool[_n_all_cols];

		int i=0;
		foreach(ColumnHeader h, headers){
			_header_names.push_back(h.getTitle());
			_cols_active[i] = true;
		}
}

LibraryItemModel::~LibraryItemModel(){
	delete _cols_active;
}


QVariant LibraryItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const{

	 if (role != Qt::DisplayRole)
			 return QVariant();

	 int idx_col = calc_shown_col(section);
	 if (orientation == Qt::Horizontal)
		 return _header_names[idx_col];
	 return QVariant();

}


QStringList LibraryItemModel::get_header_names() const {
	return _header_names;
}


int LibraryItemModel::get_n_cols() const {
	return _n_all_cols;
}


int LibraryItemModel::calc_shown_col(int col) const {
	int idx_col = 0;
	int n_true = -1;
	for(idx_col=0; idx_col<_n_all_cols; idx_col++){
		if(_cols_active[idx_col]) n_true++;
		if(n_true == col) break;
	}

	return idx_col;
}

bool LibraryItemModel::is_col_shown(int col) const{
	return _cols_active[col];
}


int LibraryItemModel::columnCount(const QModelIndex& parent) const{

	Q_UNUSED(parent);

	int n_active = 0;
	for(int i=0; i<_n_all_cols; i++){
		if(_cols_active[i]) n_active++;
	}

	return n_active;
}


bool LibraryItemModel::insertColumns(int position, int cols, const QModelIndex &index){

	beginInsertColumns(QModelIndex(), position, position+cols-1);

	for(int i=position; i<position+cols; i++){

		_cols_active[i] = true;
	}

	endInsertColumns();
	return true;
}


bool LibraryItemModel::removeColumns(int position, int cols, const QModelIndex &index){

    beginRemoveColumns(QModelIndex(), position, position+cols-1);
    for(int i=0; i<_n_all_cols; i++){
        _cols_active[i] = false;
    }

    endRemoveColumns();
    return true;
}


void LibraryItemModel::set_selected(QList<int>& rows){
    _selected_rows = rows;
}

QList<int> LibraryItemModel::get_selected() const{
    return _selected_rows;
}

bool LibraryItemModel::is_selected(int row) const {
    return _selected_rows.contains(row);
}



