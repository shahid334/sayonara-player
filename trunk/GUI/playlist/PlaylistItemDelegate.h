/*
 * ItemListDelegate.h
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

#ifndef ITEMLISTDELEGATE_H_
#define ITEMLISTDELEGATE_H_

#include "GUI/playlist/GUI_PlaylistEntry.h"

#include <QObject>
#include <QLabel>
#include <QListView>
#include <QItemDelegate>


class PlaylistItemDelegate : public QItemDelegate{
	 Q_OBJECT

public:
	 PlaylistItemDelegate(QListView *parent = 0);
	virtual ~PlaylistItemDelegate();

	public:


	    void paint(QPainter *painter, const QStyleOptionViewItem &option,
	               const QModelIndex &index) const;
	    QSize sizeHint(const QStyleOptionViewItem &option,
	                   const QModelIndex &index) const;


	    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
	   	                          const QModelIndex &index) const;
	   	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	   	void setModelData(QWidget *editor, QAbstractItemModel *model,
	   	                      const QModelIndex &index) const;

	private:
	    GUI_PlaylistEntry* _pl_entry;

	    int					_rendered_items;


	    QListView*			_parent;






};

#endif /* ITEMLISTDELEGATE_H_ */
