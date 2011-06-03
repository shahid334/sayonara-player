/*
 * LibraryItemModelAlbums.h
 *
 *  Created on: Apr 26, 2011
 *      Author: luke
 */

#ifndef LIBRARYITEMMODELALBUMS_H_
#define LIBRARYITEMMODELALBUMS_H_

#include <HelperStructs/MetaData.h>
#include <QAbstractTableModel>
#include <QList>

using namespace std;

class LibraryItemModelAlbums : public QAbstractTableModel {

Q_OBJECT
public:
	LibraryItemModelAlbums();
	virtual ~LibraryItemModelAlbums();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent) const;

	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	void sort(int column, Qt::SortOrder order);


private:
	QList<Album> _album_list;


};

#endif /* LIBRARYITEMMODELALBUMS_H_ */
