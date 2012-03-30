/* PlaylistItemDelegate.h */

/* Copyright (C) 2011  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * ItemListDelegate_small.h
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

#ifndef ITEMLISTDELEGATESMALL_H_
#define ITEMLISTDELEGATESMALL_H_

#include "GUI/playlist/GUI_PlaylistEntrySmall.h"
#include "GUI/playlist/PlaylistItemDelegateInterface.h"
#include <QObject>
#include <QLabel>
#include <QListView>
#include <QItemDelegate>


class PlaylistItemDelegateSmall : public PlaylistItemDelegateInterface{
	 Q_OBJECT

public:
	 PlaylistItemDelegateSmall(QListView *parent = 0);
	 virtual ~PlaylistItemDelegateSmall();

	public:


	    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
	               const QModelIndex &index) const;
	    virtual QSize sizeHint(const QStyleOptionViewItem &option,
	                   const QModelIndex &index) const;


	    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
	   	                          const QModelIndex &index) const;
	    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	    virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
	   	                      const QModelIndex &index) const;





	protected:

	    GUI_PlaylistEntrySmall* _pl_entry;

};

#endif /* ITEMLISTDELEGATESMALL_H_ */
