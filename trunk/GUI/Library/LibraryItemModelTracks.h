/*
 * LibraryItemModelTracks.h
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#ifndef LIBRARYITEMMODELTRACKS_H_
#define LIBRARYITEMMODELTRACKS_H_

#include <QObject>
#include <QStringList>
#include <QAbstractTableModel>

#include <vector>
#include <HelperStructs/MetaData.h>

using namespace std;


class LibraryItemModelTracks : public QAbstractTableModel {

Q_OBJECT

public:
	LibraryItemModelTracks(QObject* parent=0);
	LibraryItemModelTracks(const vector<MetaData>& v_metadata);
	virtual ~LibraryItemModelTracks();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;



private:
	QList<QStringList>		_tracklist;
	QStringList				_headerdata;
};

#endif /* LIBRARYITEMMODELTRACKS_H_ */
