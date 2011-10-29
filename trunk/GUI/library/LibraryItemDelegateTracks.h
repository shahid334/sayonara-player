/*
 * ibraryItemDelegateTracks.h
 *
 *  Created on: Oct 28, 2011
 *      Author: luke
 */

#ifndef IBRARYITEMDELEGATETRACKS_H_
#define IBRARYITEMDELEGATETRACKS_H_

#include <QItemDelegate>
#include <QTableView>

class LibraryItemDelegateTracks : public QItemDelegate {
public:
	LibraryItemDelegateTracks(QTableView* parent=0);
	virtual ~LibraryItemDelegateTracks();

public:


	    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;


		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
		QTableView* _parent;
};

#endif /* IBRARYITEMDELEGATETRACKS_H_ */
