/*
 * PlaylistItemDelegateInterface.h
 *
 *  Created on: Feb 17, 2012
 *      Author: luke
 */

#ifndef PLAYLISTITEMDELEGATEINTERFACE_H_
#define PLAYLISTITEMDELEGATEINTERFACE_H_


#include <QObject>
#include <QLabel>
#include <QListView>
#include <QItemDelegate>


class PlaylistItemDelegateInterface : public QItemDelegate {

public:
	 //PlaylistItemDelegateInterface(QListView *parent = 0);

	public:

	    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
	               const QModelIndex &index) const = 0;
	    virtual QSize sizeHint(const QStyleOptionViewItem &option,
	                   const QModelIndex &index) const = 0;


	    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
	   	                          const QModelIndex &index) const = 0;
	    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const = 0;
	    virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
	   	                      const QModelIndex &index) const = 0;

	    int rowHeight(){return _row_height;}

	protected:
	    int					_rendered_items;
	    int					_row_height;
	    QListView*			_parent;

};


#endif /* PLAYLISTITEMDELEGATEINTERFACE_H_ */
