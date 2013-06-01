/* PlaylistItemDelegateInterface.cpp */

/* Copyright (C) 2012  Lucio Carreras
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


#ifndef PLAYLISTITEMDELEGATEINTERFACE_H_
#define PLAYLISTITEMDELEGATEINTERFACE_H_


#include <QObject>
#include <QLabel>
#include <QListView>
#include <QItemDelegate>
#include ""


class PlaylistItemDelegate : public QItemDelegate {

public:
	PlaylistItemDelegate(QListView *parent = 0, bool small);
	virtual ~PlaylistItemDelegate();

	public:


		virtual void paint( QPainter *painter, const QStyleOptionViewItem &option, 
		                     const QModelIndex &index) const;


		QSize PlaylistItemDelegateSmall::sizeHint(const QStyleOptionViewItem &option,
				             const QModelIndex &index) const;

		void setMaxWidth(int w);
		int rowHeight();

	


	private:
		int		_rendered_items;
		int		_row_height;
		int		_max_width;
		QListView*	_parent;

public:
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};


#endif /* PLAYLISTITEMDELEGATEINTERFACE_H_ */
