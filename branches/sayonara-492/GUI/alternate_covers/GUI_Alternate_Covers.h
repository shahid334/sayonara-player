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

#include "ui_GUI_Alternate_Covers.h"
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
        void sig_covers_changed(QString);
        void sig_no_cover();


	public slots:
		void start(int, bool);
        void start(QString, QString);
        void changeSkin(bool dark);

	private slots:
		void save_button_pressed();
		void cancel_button_pressed();
        void no_cover_pressed();
		void search_button_pressed();
		void cover_pressed(const QModelIndex& idx);
		void covers_there(QString classname, int n_covers);
		void tmp_folder_changed(const QString&);

	private:
		Ui::AlternateCovers* ui;

		int 				_cur_idx;
		QString				_tmp_dir;
		QString				_class_name;
		QString				_calling_class;
		Album				_album;
		Artist				_artist;
		QStringList			_filelist;
		bool				_search_for_album;

		AlternateCoverItemDelegate* _delegate;
		AlternateCoverItemModel*	_model;
		CoverLookup*				_cov_lookup;
        QFileSystemWatcher*			_watcher;

        bool                _no_album;
        QString             _target_filename;


        void cleanup();
        void remove_old_files();
        void update_model();
        QString copy_and_save(QString src_file);
        void delete_token();





};

#endif /* GUI_ALTERNATE_COVERS_H_ */
