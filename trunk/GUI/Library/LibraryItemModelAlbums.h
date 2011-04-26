/*
 * LibraryItemModelAlbums.h
 *
 *  Created on: Apr 26, 2011
 *      Author: luke
 */

#ifndef LIBRARYITEMMODELALBUMS_H_
#define LIBRARYITEMMODELALBUMS_H_

#include <QAbstractListModel>
#include <QStringList>

class LibraryItemModelAlbums : public QAbstractListModel {

Q_OBJECT
public:
	LibraryItemModelAlbums();
	virtual ~LibraryItemModelAlbums();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());



private:
	QStringList _album_list;


};

#endif /* LIBRARYITEMMODELALBUMS_H_ */
