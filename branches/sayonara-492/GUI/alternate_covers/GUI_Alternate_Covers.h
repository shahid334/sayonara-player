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
#include "DatabaseAccess/CDatabaseConnector.h"
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

signals:
    void sig_cover_found();

private:
    bool    _is_sampler;
    Ui::AlternateCovers* ui;
    CDatabaseConnector* _db;
    QString _sel_cover_filename;
    QString _target_cover_filename;

private slots:

    void save_pressed();
    void cancel_pressed();
    void no_cover_pressed();
    void cover_chosen(QString filename);


public:
	GUI_Alternate_Covers(QWidget* parent, QString calling_class);
	virtual ~GUI_Alternate_Covers();

    void search_for_artist_image(QString artist_name);
    void search_for_sampler_image(QString album_name);
    void search_for_album_image(QString album_name, QString artist_name);

    void search_for_artist_image(int artist_id);
    void search_for_sampler_image(int album_id);
    void search_for_album_image(int album_id);





};

#endif /* GUI_ALTERNATE_COVERS_H_ */
