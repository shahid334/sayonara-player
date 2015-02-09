/* GUI_TagEdit.cpp */

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



#include "GUI/tagedit/GUI_TagEdit.h"
#include "TagEdit/TagExpression.h"
#include "HelperStructs/Helper.h"
#include <QDir>
#include <QMessageBox>

GUI_TagEdit::GUI_TagEdit(TagEdit* tag_edit, QWidget* parent) :
	SayonaraWidget(parent)
{
	setupUi(this);

	_tag_edit = tag_edit;

	connect(btn_next, SIGNAL(clicked()), this, SLOT(next_button_clicked()));
	connect(btn_prev, SIGNAL(clicked()), this, SLOT(prev_button_clicked()));
	connect(btn_ok, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
	connect(btn_apply_tag, SIGNAL(clicked()), this, SLOT(apply_tag_clicked()));
	connect(btn_apply_tag_all, SIGNAL(clicked()), this, SLOT(apply_tag_all_clicked()));

	connect(cb_album_all, SIGNAL(toggled(bool)), this, SLOT(album_all_changed(bool)));
	connect(cb_artist_all, SIGNAL(toggled(bool)), this, SLOT(artist_all_changed(bool)));
	connect(cb_genre_all, SIGNAL(toggled(bool)), this, SLOT(genre_all_changed(bool)));
	connect(cb_year_all, SIGNAL(toggled(bool)), this, SLOT(year_all_changed(bool)));
	connect(cb_discnumber_all, SIGNAL(toggled(bool)), this, SLOT(discnumber_all_changed(bool)));
	connect(le_tag, SIGNAL(textChanged(const QString &)), this, SLOT(tag_text_changed(const QString&)));
	connect(btn_undo, SIGNAL(clicked()), this, SLOT(undo_clicked()));
	connect(btn_undo_all, SIGNAL(clicked()), this, SLOT(undo_all_clicked()));

	connect(le_tag, SIGNAL(selectionChanged()), this, SLOT(tag_selection_changed()));
	connect(btn_title, SIGNAL(toggled(bool)), this, SLOT(btn_title_checked(bool)));
	connect(btn_artist, SIGNAL(toggled(bool)), this, SLOT(btn_artist_checked(bool)));
	connect(btn_album, SIGNAL(toggled(bool)), this, SLOT(btn_album_checked(bool)));
	connect(btn_track_nr, SIGNAL(toggled(bool)), this, SLOT(btn_track_nr_checked(bool)));
	connect(btn_year, SIGNAL(toggled(bool)), this, SLOT(btn_year_checked(bool)));
	connect(btn_disc_nr, SIGNAL(toggled(bool)), this, SLOT(btn_disc_nr_checked(bool)));

	connect(_tag_edit, SIGNAL(sig_progress(int)), this, SLOT(progress_changed(int)));
	connect(_tag_edit, SIGNAL(sig_metadata_received(const MetaDataList&)), this, SLOT(metadata_changed(const MetaDataList&)));
	connect(btn_cancel, SIGNAL(clicked()), this, SLOT(btn_cancel_clicked()));

	reset();
}


GUI_TagEdit::~GUI_TagEdit(){

}

void GUI_TagEdit::language_changed(){

}


void GUI_TagEdit::btn_cancel_clicked(){
	emit sig_cancelled();
}

void GUI_TagEdit::progress_changed(int val){

	pb_progress->setVisible(val >= 0);

	if(val >= 0){
		pb_progress->setValue(val);
	}
}

void GUI_TagEdit::metadata_changed(const MetaDataList& val){

	reset();

	_cur_idx = 0;
	track_idx_changed();
}


bool GUI_TagEdit::check_idx(int idx){
	int n_tracks = _tag_edit->get_n_tracks();
	if(idx <0 || idx >= n_tracks) return false;
	return true;

}



void GUI_TagEdit::next_button_clicked(){

	write_changes(_cur_idx);

	_cur_idx++;

	track_idx_changed();
}


void GUI_TagEdit::prev_button_clicked(){

	write_changes(_cur_idx);

	_cur_idx--;

	track_idx_changed();
}

void GUI_TagEdit::track_idx_changed(){

	int n_tracks = _tag_edit->get_n_tracks();

	btn_next->setEnabled(_cur_idx >= 0 && _cur_idx < n_tracks - 1);
	btn_prev->setEnabled(_cur_idx > 0 && _cur_idx < n_tracks);

	if(!check_idx(_cur_idx)) return;


	MetaData md = _tag_edit->get_metadata(_cur_idx);

	if(le_tag->text().isEmpty()){

		le_tag->setText(md.filepath());
	}

	else if( !(btn_album->isChecked() ||
			btn_artist->isChecked() ||
			btn_title->isChecked() ||
			btn_year->isChecked() ||
			btn_disc_nr->isChecked() ||
			btn_track_nr->isChecked()))
	{
		le_tag->setText(md.filepath());
	}

	_tag_expression.update_tag(le_tag->text(), md.filepath());
	set_tag_colors(_tag_expression.is_valid());

	le_title->setText(md.title);

	if(_album_all.isEmpty()){
		le_album->setText(md.album);
	} else {
		le_album->setText(_album_all);
	}

	if(_artist_all.isEmpty()){
		le_artist->setText(md.artist);
	} else {
		le_artist->setText(_artist_all);
	}

	if(_genre_all.isEmpty()){
		le_genre->setText( md.genres.join(", ") );
	} else {
		le_genre->setText(_genre_all);
	}

	if(_year_all == -1){
		sb_year->setValue(md.year);
	} else {
		sb_year->setValue(_year_all);
	}

	if(_discnumber_all == -1){
		sb_discnumber->setValue(md.discnumber);
	} else {
		sb_discnumber->setValue(_discnumber_all);
	}

	sb_track_num->setValue(md.track_num);



	lab_filepath->clear();
	lab_filepath->insertPlainText( md.filepath() );
	lab_track_num->setText(
			tr("Track ") +
			QString::number( _cur_idx+1 ) + "/" + QString::number( n_tracks )
	);
}

void GUI_TagEdit::reset(){

	_cur_idx = -1;

	cb_album_all->setChecked(false);
	cb_artist_all->setChecked(false);
	cb_genre_all->setChecked(false);
	cb_discnumber_all->setChecked(false);
	cb_year_all->setChecked(false);

	_album_all = "";
	_artist_all = "";
	_genre_all = "";
	_discnumber_all = -1;
	_year_all = -1;

	lab_track_num ->setText(tr("Track ") + "0/0");
	btn_prev->setEnabled(false);
	btn_next->setEnabled(false);

	le_album->clear();
	le_artist->clear();
	le_title->clear();
	le_genre->clear();
	le_tag->clear();
	sb_year->setValue(0);
	sb_discnumber->setValue(0);
	sb_track_num->setValue(0);

	lab_filepath->clear();
	pb_progress->setVisible(false);

	btn_album->setChecked(false);
	btn_artist->setChecked(false);
	btn_title->setChecked(false);
	btn_year->setChecked(false);
	btn_disc_nr->setChecked(false);
	btn_track_nr->setChecked(false);

}

void GUI_TagEdit::album_all_changed(bool b){

	if(b) _album_all = le_album->text();
	else _album_all = "";
}

void GUI_TagEdit::artist_all_changed(bool b){
	if(b) _artist_all = le_artist->text();
	else _artist_all = "";
}

void GUI_TagEdit::genre_all_changed(bool b){
	if(b) _genre_all = le_genre->text();
	else _genre_all = "";
}

void GUI_TagEdit::year_all_changed(bool b){
	if(b) _year_all = sb_year->value();
	else _year_all = -1;
}

void GUI_TagEdit::discnumber_all_changed(bool b){
	if(b) _discnumber_all = sb_discnumber->value();
	else _discnumber_all = -1;
}


void GUI_TagEdit::undo_clicked(){

	_tag_edit->undo(_cur_idx);
	track_idx_changed();
}

void GUI_TagEdit::undo_all_clicked(){
	_tag_edit->undo_all();
	track_idx_changed();
}


void GUI_TagEdit::write_changes(int idx){

	if( !check_idx(idx) ) return;

	MetaData md = _tag_edit->get_metadata(idx);

	md.title = le_title->text();
	md.artist = le_artist->text();
	md.album = le_album->text();
	md.genres = le_genre->text().split(", ");
	md.discnumber = sb_discnumber->value();
	md.year = sb_year->value();
	md.track_num = sb_track_num->value();

	_tag_edit->update_track(idx, md);
}

void GUI_TagEdit::ok_button_clicked(){

	write_changes(_cur_idx);

	for(int i=0; i<_tag_edit->get_n_tracks(); i++){

		if(i == _cur_idx) continue;

		MetaData md = _tag_edit->get_metadata(i);

		if( !_album_all.isEmpty()){
			md.album = _album_all;
		}
		if( ! _artist_all.isEmpty()){
			md.artist = _artist_all;
		}
		if( ! _genre_all.isEmpty()){
			md.genres = _genre_all.split(", ");
		}

		if( _discnumber_all >= 0){
			md.discnumber = _discnumber_all;
		}

		if( _year_all >= 0){
			md.year = _year_all;
		}

		_tag_edit->update_track(i, md);
	}

	_tag_edit->write_tracks_to_db();
}


/*** TAG ***/

void GUI_TagEdit::set_tag_colors(bool valid){

	if( !valid ){
		le_tag->setStyleSheet("color: red;");
	}

	else{
		le_tag->setStyleSheet("");
	}

	btn_apply_tag->setEnabled(valid);
	btn_apply_tag_all->setEnabled(valid);
}

void GUI_TagEdit::tag_text_changed(const QString& str){

	if( !check_idx(_cur_idx) ) return;
	MetaData md = _tag_edit->get_metadata(_cur_idx);
	_tag_expression.update_tag(str, md.filepath() );

	set_tag_colors(_tag_expression.is_valid());

}

void GUI_TagEdit::apply_tag(int idx){

	if(!check_idx(idx)) return;

	QMap<QString, QString> tag_cap_map = _tag_expression.get_tag_val_map();
	MetaData md = _tag_edit->get_metadata(idx);

	for(const QString& tag : tag_cap_map.keys()){

		QString cap = tag_cap_map[tag];

		if(tag.compare(TAG_TITLE) == 0){
			md.title = cap;
		}

		else if(tag.compare(TAG_ALBUM) == 0){
			md.album = cap;
		}

		else if(tag.compare( TAG_ARTIST) == 0){
			md.artist = cap;
		}

		else if(tag.compare(TAG_TRACK_NUM) == 0){
			md.track_num = cap.toInt();
		}

		else if(tag.compare(TAG_YEAR) == 0){
			md.year = cap.toInt();
		}

		else if(tag.compare(TAG_DISC) == 0){
			md.discnumber = cap.toInt();
		}
	}

	_tag_edit->update_track(idx, md);

	if(idx == _cur_idx){
		// force gui update
		track_idx_changed();
	}
}

void GUI_TagEdit::apply_tag_clicked(){
	apply_tag(_cur_idx);

}

void GUI_TagEdit::apply_tag_all_clicked(){

	QList<int> not_valid;
	QString not_valid_str = tr("Cannot apply tag for") + "<br /><br /> ";
	MetaDataList v_md = _tag_edit->get_all_metadata();
	int n_tracks = v_md.size();

	for(int i=0; i<n_tracks; i++){

		_tag_expression.update_tag(le_tag->text(), v_md[i].filepath() );

		if(!_tag_expression.is_valid()){
			not_valid << i;
			not_valid_str += QString::number(i+1) + "/" + QString::number(n_tracks) + " " + v_md[i].title + " " + tr("by") + " " + v_md[i].artist + "<br />";
		}
	}

	not_valid_str += "<br />" + tr("Ignore these tracks?");

	/***
	 * TODO: Message Box
	 */

	int ret = QMessageBox::Yes;
	if(! not_valid.isEmpty() ){
		ret = QMessageBox::warning(this, tr("Warning"), not_valid_str, QMessageBox::Yes, QMessageBox::No);
	}

	if(ret == QMessageBox::Yes){

		for(int i=0; i<n_tracks; i++){
			if(not_valid.contains(i)) continue;

			_tag_expression.update_tag(le_tag->text(), v_md[i].filepath() );
			apply_tag(i);
		}
	}
}

void GUI_TagEdit::tag_selection_changed(){

	_tag_selection_start = le_tag->selectionStart();
	_tag_selection_len = le_tag->selectedText().size();

}

bool GUI_TagEdit::replace_selected_tag_text(Tag t, bool b){

	if(_tag_selection_start < 0 && b) {
		qDebug() << "Nothing selected...";
		return false;
	}

	QString text = le_tag->text();

	// replace the string by a tag
	if(b){

		ReplacedString selected_text = text.mid( _tag_selection_start, _tag_selection_len );
		if(!_tag_expression.check_tag(t, selected_text)) return false;

		text.replace( _tag_selection_start, _tag_selection_len, t );
		le_tag->setText(text);

		_tag_str_map[t] = selected_text;
	}

	// replace tag by the original string
	else{

		text.replace(t, _tag_str_map[t]);
		le_tag->setText(text);

		_tag_str_map.remove(t);
	}


	if(check_idx(_cur_idx)){

		// fetch corresponding filepath and update the tag expression
		MetaData md = _tag_edit->get_metadata(_cur_idx);
		_tag_expression.update_tag(text, md.filepath() );

		set_tag_colors(_tag_expression.is_valid());
	}

	return true;
}

void GUI_TagEdit::btn_title_checked(bool b){
	if(!replace_selected_tag_text(TAG_TITLE, b)){
		btn_title->setChecked(false);
	}
}

void GUI_TagEdit::btn_artist_checked(bool b){
	if(!replace_selected_tag_text(TAG_ARTIST, b)){
		btn_artist->setChecked(false);
	}
}

void GUI_TagEdit::btn_album_checked(bool b){
	if(!replace_selected_tag_text(TAG_ALBUM, b)){
		btn_album->setChecked(false);
	}
}
void GUI_TagEdit::btn_track_nr_checked(bool b){
	if(!replace_selected_tag_text(TAG_TRACK_NUM, b)){
		btn_track_nr->setChecked(false);
	}
}
void GUI_TagEdit::btn_disc_nr_checked(bool b){
	if(!replace_selected_tag_text(TAG_DISC, b)){
		btn_disc_nr->setChecked(false);
	}
}
void GUI_TagEdit::btn_year_checked(bool b){
	if(!replace_selected_tag_text(TAG_YEAR, b)){
		btn_year->setChecked(false);
	}
}
