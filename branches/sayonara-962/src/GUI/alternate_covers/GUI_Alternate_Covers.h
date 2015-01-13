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
#include "CoverLookup/CoverLookupAlternative.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/SayonaraClass.h"

#include <QPixmap>
#include <QList>
#include <QModelIndex>

class GUI_Alternate_Covers : public SayonaraDialog, private Ui::AlternateCovers{

	Q_OBJECT
public:
	GUI_Alternate_Covers(QWidget* parent, QString calling_class);
	virtual ~GUI_Alternate_Covers();

signals:
	void sig_cover_changed(const CoverLocation&);
	void sig_no_cover();


public slots:
	void start(Album album, const CoverLocation& cl = CoverLocation());
	void start(int album_id, const CoverLocation& cl = CoverLocation());
	void start(QString album_name, QString artist_name, const CoverLocation& cl = CoverLocation());
	void start(Artist artist, const CoverLocation& cl = CoverLocation());
	void start(QString artist_name, const CoverLocation& cl = CoverLocation());

private slots:
	void save_button_pressed();
	void cancel_button_pressed();
	void search_button_pressed();
	void cover_pressed(const QModelIndex& idx);
	void open_file_dialog();
	void cl_new_cover(const CoverLocation& path);
	void cl_finished(bool);

private:

	int 				_cur_idx;
	QString				_last_path;
	CoverLocation		_cover_location;
	QList<CoverLocation> _filelist;
	bool				_is_searching;

	AlternateCoverItemDelegate* _delegate;
	AlternateCoverItemModel*	_model;

	CoverLookupAlternative*        _cl_alternative;

	void reset_model();
	void connect_and_start();
	void delete_all_files();

protected:
	void closeEvent(QCloseEvent* e);
	void language_changed();
};

#endif /* GUI_ALTERNATE_COVERS_H_ */
