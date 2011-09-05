/*
 * AlternateCoverItemDelegate.h
 *
 *  Created on: Jul 1, 2011
 *      Author: luke
 */

#ifndef ALTERNATECOVERITEMDELEGATE_H_
#define ALTERNATECOVERITEMDELEGATE_H_

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QItemDelegate>

class AlternateCoverItemDelegate : public QItemDelegate{

	Q_OBJECT
public:
	AlternateCoverItemDelegate(QTableView* parent=0);
	virtual ~AlternateCoverItemDelegate();

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

#endif /* ALTERNATECOVERITEMDELEGATE_H_ */
