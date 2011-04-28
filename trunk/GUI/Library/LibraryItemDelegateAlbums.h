/*
 * LibraryItemDelegateAlbums.h
 *
 *  Created on: Apr 28, 2011
 *      Author: luke
 */

#ifndef LIBRARYITEMDELEGATEALBUMS_H_
#define LIBRARYITEMDELEGATEALBUMS_H_
#include <QObject>
#include <QLabel>
#include <QTableView>
#include <QItemDelegate>

class LibraryItemDelegateAlbums : public QItemDelegate {
	Q_OBJECT
public:
	LibraryItemDelegateAlbums(QTableView* parent=0);
	virtual ~LibraryItemDelegateAlbums();

public:


	    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;


		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;


private:

		QTableView* _parent;


};

#endif /* LIBRARYITEMDELEGATEALBUMS_H_ */
