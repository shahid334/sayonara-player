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

#include "GUI/ui_GUI_Alternate_Covers.h"
#include "GUI/alternate_covers/AlternateCoverItemDelegate.h"
#include "GUI/alternate_covers/AlternateCoverItemModel.h"
#include "CoverLookup/CoverLookup.h"
#include "HelperStructs/MetaData.h"

#include <QDialog>
#include <QWidget>
#include <QPixmap>
#include <QList>
#include <QModelIndex>
#include <QFileSystemWatcher>


using namespace std;



class GUI_Alternate_Covers : public QDialog, private Ui::AlternateCovers{

	Q_OBJECT
public:
	GUI_Alternate_Covers(QWidget* parent, QString calling_class);
	virtual ~GUI_Alternate_Covers();

	signals:

		void sig_search_images(const QString&);
        void sig_covers_changed(QString, QString);
        void sig_no_cover();


	public slots:
        void start(Album album);
        void start(QString album_name, QString artist_name);
        void start(Artist artist);
        void start(QString artist_name);

        void changeSkin(bool dark);
        void language_changed();

	private slots:
		void save_button_pressed();
		void cancel_button_pressed();
		void search_button_pressed();
		void cover_pressed(const QModelIndex& idx);
        void open_file_dialog();
        void cl_new_cover(QString);
        void cl_finished();

	private:
		Ui::AlternateCovers* ui;

		int 				_cur_idx;
		QString				_class_name;
		QString				_calling_class;
		Album				_album;
		Artist				_artist;
		bool				_search_for_album;

		AlternateCoverItemDelegate* _delegate;
		AlternateCoverItemModel*	_model;

        bool                _no_album;
        CoverLookupAlternative*        _cl_alternative;

        void update_model();


private slots:
        void cft_destroyed();




};

#endif /* GUI_ALTERNATE_COVERS_H_ */
