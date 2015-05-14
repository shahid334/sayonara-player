/* PlaylistItemModel.h */

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
 * PlaylistItemModel.h
 *
 *  Created on: Apr 8, 2011
 *      Author: luke
 */

#ifndef PLAYLISTITEMMODEL_H_
#define PLAYLISTITEMMODEL_H_

#include "HelperStructs/MetaData/MetaData.h"
#include "GUI/AbstractSearchModel.h"

#include <QObject>
#include <QList>
#include <QLabel>
#include <QAbstractListModel>
#include <QStringList>

using namespace std;



class PlaylistItemModel : public AbstractSearchListModel {
	Q_OBJECT
public:
	PlaylistItemModel(QObject* parent = 0);

	virtual ~PlaylistItemModel();

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);

    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

    void set_selected(QList<int>& rows);
    bool is_selected(int row) const ;

    void get_metadata(const QList<int>& rows, MetaDataList& v_md);

    virtual QModelIndex getFirstRowIndexOf(QString substr);
	virtual QModelIndex getPrevRowIndexOf(QString substr, int row);
	virtual QModelIndex getNextRowIndexOf(QString substr, int row);
	virtual QMap<QChar, QString> getExtraTriggers();

protected:
    MetaDataList		_v_meta_data;
    QList<int>          _selected_rows;
    int					_cur_play_idx;

};

#endif /* PLAYLISTITEMMODEL_H_ */
