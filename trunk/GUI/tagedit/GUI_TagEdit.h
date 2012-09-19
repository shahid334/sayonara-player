/* GUI_TagEdit.h */

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
 * GUI_TagEdit.h
 *
 *  Created on: May 24, 2011
 *      Author: luke
 */

#ifndef GUI_TAGEDIT_H_
#define GUI_TAGEDIT_H_

#define TAG_TITLE QString("<TITLE>")
#define TAG_ALBUM QString("<ALBUM>")
#define TAG_ARTIST QString("<ARTIST>")
#define TAG_TRACK_NUM QString("<TRACK>")
#define TAG_YEAR QString("<YEAR>")

#include "ui_GUI_TagEdit.h"
#include "HelperStructs/MetaData.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QObject>
#include <QWidget>
#include <QList>
#include <QString>
#include <QStringList>


#include <vector>

using namespace std;

class GUI_TagEdit : public QWidget, private Ui::GUI_TagEdit{

	Q_OBJECT

public:
	GUI_TagEdit(QWidget* parent=0);
	virtual ~GUI_TagEdit();

	signals:
		void id3_tags_changed();
		void id3_tags_changed(vector<MetaData>&);
		void sig_success(bool);
		void sig_cancelled();


	private slots:
		void prev_button_clicked();
		void next_button_clicked();
		void ok_button_clicked();
		void cancel_button_clicked();
		void album_changed(QString text);
		void artist_changed(QString text);

		void all_albums_clicked();
		void all_artists_clicked();
		void all_genre_clicked();
		void all_year_clicked();
        void all_tag_clicked();

        void help_tag_clicked();
        void undo_tag_clicked();
        void apply_tag_clicked();

        void tag_from_path_text_changed(const QString&);


	public slots:
		void change_meta_data(const vector<MetaData>&);
		void change_meta_data(const MetaData&);


	private:
		Ui::GUI_TagEdit* ui;
		QWidget*	_parent;
        QMap<int, bool>  _idx_affected_by_tag;

		int _cur_idx;
		vector<MetaData> _vec_org_metadata;
		vector<MetaData> _vec_tmp_metadata;

		vector<Album> _vec_albums;
		vector<Artist> _vec_artists;

		QStringList _lst_new_albums;
		QStringList _lst_new_artists;

		CDatabaseConnector* _db;

		int _max_album_id;
		int _max_artist_id;

		void init();

		void show_metadata();
		void save_metadata();
		void check_for_new_album_and_artist(QList<Album>& v_album, QList<Artist>& v_artist);


		void change_mp3_file(MetaData& md);
		bool store_to_database(QList<Album>& new_albums, QList<Artist>& new_artists);


        bool calc_tag(int idx, MetaData& md);
        bool remove_aftertag_str(QString& str, QString aftertag, bool looking_for_num);
        bool has_open_tag(QString str);
        bool has_tag(QString str);
        bool is_valid_tag_str(QString str);


};

#endif /* GUI_TAGEDIT_H_ */
