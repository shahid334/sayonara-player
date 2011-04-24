/*
 * PlaylistItemModel.h
 *
 *  Created on: Apr 8, 2011
 *      Author: luke
 */

#ifndef PLAYLISTITEMMODEL_H_
#define PLAYLISTITEMMODEL_H_

#include "HelperStructs/MetaData.h"

#include <QObject>
#include <QList>
#include <QLabel>
#include <QAbstractListModel>
#include <QStringList>

using namespace std;



class PlaylistItemModel : public QAbstractListModel {
	Q_OBJECT
public:
	PlaylistItemModel(QObject* parent = 0);

	virtual ~PlaylistItemModel();

	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool insertRows(int position, int rows, const QModelIndex &index);
	bool removeRows(int position, int rows, const QModelIndex &index);


private:
	QStringList		_labellist;

};

#endif /* PLAYLISTITEMMODEL_H_ */
