/*
 * LibraryItemDelegateArtists.h
 *
 *  Created on: Jun 3, 2011
 *      Author: luke
 */

#ifndef LIBRARYITEMDELEGATEARTISTS_H_
#define LIBRARYITEMDELEGATEARTISTS_H_
#include <QObject>
#include <QLabel>
#include <QTableView>
#include <QItemDelegate>


class LibraryItemDelegateArtists : public QItemDelegate {
	Q_OBJECT
public:
	LibraryItemDelegateArtists(QTableView* parent=0);
	virtual ~LibraryItemDelegateArtists();

public:


	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;


	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;


private:

		QTableView* _parent;
		int _m_column_width;


};

#endif /* LIBRARYITEMDELEGATEARTISTS_H_ */
