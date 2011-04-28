/*
 * LibraryItemModelArtistts.h
 *
 *  Created on: Apr 26, 2011
 *      Author: luke
 */

#ifndef LIBRARYITEMMODELARTISTS_H_
#define LIBRARYITEMMODELARTISTS_H_

#include <QAbstractTableModel>
#include <QStringList>

class LibraryItemModelArtists : public QAbstractTableModel {

Q_OBJECT
public:
	LibraryItemModelArtists();
	virtual ~LibraryItemModelArtists();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent) const;

	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;



private:
	QStringList _artist_list;


};


#endif /* LIBRARYITEMMODELARTISTS_H_ */
