/* GUI_Alternate_Covers.h */

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
 * GUI_Alternate_Covers.h
 *
 *  Created on: Jul 1, 2011
 *      Author: luke
 */

#ifndef GUI_ALTERNATE_COVERS_H_
#define GUI_ALTERNATE_COVERS_H_

#include <ui_GUI_Alternate_Covers.h>
#include "GUI/alternate_covers/AlternateCoverItemDelegate.h"
#include "GUI/alternate_covers/AlternateCoverItemModel.h"

#include <QWidget>
#include <QPixmap>
#include <QList>
#include <QModelIndex>


using namespace std;



class GUI_Alternate_Covers : public QWidget{

	Q_OBJECT
public:
	GUI_Alternate_Covers();
	virtual ~GUI_Alternate_Covers();

	signals:
		void fetch_covers(const QString&, bool);
		void search_album_image(const QString&);
		void search_artist_image(const QString&);

	public slots:
		void new_cover_found(const QPixmap&);
		void start(const QString&, const QString&);

	private slots:
		void save_button_pressed();
		void cancel_button_pressed();
		void search_album_button_pressed();
		void search_artist_button_pressed();
		void cover_pressed(const QModelIndex& idx);



	private:
		Ui::GUI_Alternate_Covers* ui;
		AlternateCoverItemDelegate* _delegate;
		AlternateCoverItemModel*	_model;

		QList<QPixmap>	_pixmaps;
		int 			_cur_idx;

		void fill_covers();


};

#endif /* GUI_ALTERNATE_COVERS_H_ */
