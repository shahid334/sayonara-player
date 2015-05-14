/* LibraryViewAlbum.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef LIBRARYVIEWALBUM_H
#define LIBRARYVIEWALBUM_H

#include <QTimer>

#include "HelperStructs/MetaData/Album.h"
#include "GUI/library/view/LibraryView.h"
#include "GUI/library/DiscPopupMenu.h"

class LibraryViewAlbum : public LibraryView
{
	Q_OBJECT

signals:
	void sig_disc_pressed(int);

protected slots:
	virtual void disc_pressed(int);
	virtual void timer_timed_out();

public:
	explicit LibraryViewAlbum(QWidget *parent = 0);

	template <class TList, class T>
	void fill(const AlbumList& input_data);

protected:
	QVector< QList<quint8> >	_discnumbers;
	DiscPopupMenu*				_discmenu;
	QPoint						_discmenu_point;
	QTimer*						_timer;


	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected );

	virtual void calc_discmenu_point(QModelIndex idx);
	virtual void delete_discmenu();
	virtual void init_discmenu(QModelIndex idx);
	virtual void show_discmenu();

	virtual void rc_menu_show(const QPoint& p);
	virtual void mousePressEvent(QMouseEvent* e);
	virtual void mouseMoveEvent(QMouseEvent* e);

};

#endif // LIBRARYVIEWALBUM_H
