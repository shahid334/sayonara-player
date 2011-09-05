/*
 * AlternateCoverItemModel.h
 *
 *  Created on: Jul 1, 2011
 *      Author: luke
 */

#ifndef ALTERNATECOVERITEMMODEL_H_
#define ALTERNATECOVERITEMMODEL_H_

#include <QObject>
#include <QList>
#include <QModelIndex>
#include <QAbstractTableModel>


class AlternateCoverItemModel : public QAbstractTableModel {

Q_OBJECT
public:
	AlternateCoverItemModel();
	virtual ~AlternateCoverItemModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent) const;

	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());


private:
	QList<int> _item_idx_list;


};
#endif
