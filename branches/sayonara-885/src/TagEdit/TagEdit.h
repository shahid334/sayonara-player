/* TagEdit.h */

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



#ifndef TAGEDIT_H
#define TAGEDIT_H

#include "HelperStructs/SayonaraClass.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QVector>

class TagEdit : public QObject, private SayonaraClass
{
	Q_OBJECT

signals:
	void sig_progress(int);

	void sig_metadata_received(const MetaDataList&);
	void sig_metadata_changed(const MetaDataList&, const MetaDataList&);
	void sig_metadata_changed(int);


public:
	explicit TagEdit(QObject *parent = 0, bool is_extern=false);

	void undo(int idx);
	void undo_all();

	MetaData get_metadata(int idx);
	MetaDataList get_all_metadata();

	int get_n_tracks();

	void update_track(int idx, const MetaData& md);
	void write_tracks_to_db();
	void write_id3_tags();

public slots:
	void set_metadata(const MetaDataList& v_md);


private:
	MetaDataList _v_md;
	MetaDataList _v_md_orig;
	QVector<bool> _changed_md;
	bool _is_extern;

	CDatabaseConnector* _db;

	void check_for_new_artists_and_albums(QStringList& new_artists, QStringList& new_albums);

	void insert_new_artists(const QStringList& artists);
	void insert_new_albums(const QStringList& albums);

	void apply_artists_and_albums_to_md();


};

#endif // TAGEDIT_H
