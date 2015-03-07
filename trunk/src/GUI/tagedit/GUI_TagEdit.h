/* GUI_TagEdit.h */

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




#ifndef GUI_TAGEDIT_H_
#define GUI_TAGEDIT_H_

#include <QLineEdit>
#include <QFocusEvent>
#include <QString>
#include <QRegExp>

#include "HelperStructs/SayonaraClass.h"
#include "TagEdit/TagExpression.h"
#include "TagEdit/TagEdit.h"
#include "GUI/ui_GUI_TagEdit.h"

typedef QString Tag;
typedef QString ReplacedString;

class GUI_TagEdit : public SayonaraWidget, private Ui::GUI_TagEdit {

	Q_OBJECT
public:
	GUI_TagEdit(TagEdit* tag_edit, QWidget* parent=0);
	virtual ~GUI_TagEdit();

signals:
	void sig_ok_clicked(const MetaDataList&);
	void sig_undo_clicked(int idx);
	void sig_undo_all_clicked();
	void sig_cancelled();


private slots:
	void next_button_clicked();
	void prev_button_clicked();
	void apply_tag_clicked();
	void apply_tag_all_clicked();

	void album_all_changed(bool);
	void artist_all_changed(bool);
	void genre_all_changed(bool);
	void year_all_changed(bool);
	void discnumber_all_changed(bool);

	void btn_title_checked(bool b);
	void btn_artist_checked(bool b);
	void btn_album_checked(bool b);
	void btn_track_nr_checked(bool b);
	void btn_disc_nr_checked(bool b);
	void btn_year_checked(bool b);

	void tag_text_changed(const QString&);
	void set_tag_colors(bool valid);
	void ok_button_clicked();

	void undo_clicked();
	void undo_all_clicked();

	void progress_changed(int);
	void metadata_changed(const MetaDataList&);

	void btn_cancel_clicked();

private:
	TagEdit* _tag_edit;
	TagExpression _tag_expression;

	int _cur_idx;

	QString _album_all;
	QString _artist_all;
	QString _genre_all;

	int _discnumber_all;
	int _year_all;


	// keys: The different tags like <al>...
	// val: The string replaced by this tag
	// this map is used for reverting the string replaced by a tag
	QMap<Tag, ReplacedString> _tag_str_map;


	bool replace_selected_tag_text(QString t, bool b);
	void apply_tag(int idx);

	void track_idx_changed();
	void reset();
	void write_changes(int idx);
	bool check_tag(int idx, const QString& str);
	bool check_idx(int idx);

protected:
	void language_changed();
};

#endif
