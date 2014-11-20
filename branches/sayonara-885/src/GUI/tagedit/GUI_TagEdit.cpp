/* GUI_TagEdit.cpp */

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
 * GUI_TagEdit.cpp
 *
 *  Created on: May 24, 2011
 *      Author: luke
 */

#include "GUI/tagedit/GUI_TagEdit.h"
#include "GUI/ui_GUI_TagEdit.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"

#include <QMessageBox>
#include <QCompleter>
#include <QDir>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif



GUI_TagEdit::GUI_TagEdit(QWidget* parent) :
	SayonaraWidget(parent),
	Ui::GUI_TagEdit()
{
	setupUi(this);

    QCompleter* completer;
    QStringList complete_list;

    _parent = parent;

    _cur_idx = -1;
    _db = CDatabaseConnector::getInstance();

    complete_list << TAG_TITLE;
    complete_list << TAG_ARTIST;
    complete_list << TAG_ALBUM;
    complete_list << TAG_TRACK_NUM;
    complete_list << TAG_YEAR;
    complete_list << TAG_DISC;
    completer = new QCompleter(complete_list);


	le_tag_from_path->setCompleter(completer);
	le_tag_from_path->setVisible(false);
	cb_tag_all->setVisible(false);
	btn_tag_apply->setVisible(false);
	btn_tag_help->setVisible(false);
	btn_tag_undo->setVisible(false);
	label_8->setVisible(false);
	label_10->setVisible(false);
	line->setVisible(false);
    lab_icon->setPixmap(Helper::getPixmap("id3.png", QSize(50, 50), false));

	connect(pb_next_track, SIGNAL(released()), this, SLOT(next_button_clicked()));
	connect(pb_prev, SIGNAL(released()), this, SLOT(prev_button_clicked()));
	connect(pb_ok, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
	connect(pb_cancel, SIGNAL(clicked()), this, SLOT(cancel_button_clicked()));
	connect(le_tag_from_path, SIGNAL(textChanged ( const QString &)), this, SLOT(tag_from_path_text_changed(const QString&)));

	connect(btn_tag_help, SIGNAL(clicked()), this, SLOT(help_tag_clicked()));
	connect(btn_tag_undo, SIGNAL(clicked()), this, SLOT(undo_tag_clicked()));
	connect(btn_tag_apply, SIGNAL(clicked()), this, SLOT(apply_tag_clicked()));

    init();
    hide();
}



GUI_TagEdit::~GUI_TagEdit() {

}

void GUI_TagEdit::changeSkin(bool dark) {

}

void GUI_TagEdit::language_changed() {
	retranslateUi(this);
}


void GUI_TagEdit::init() {

    _max_album_id = -1;
    _max_artist_id = -1;

    _max_album_id = _db->getMaxAlbumID() + 1;
    _max_artist_id = _db->getMaxArtistID() + 1;

    if(_parent) {
		resize(_parent->size());
    }

}
void GUI_TagEdit::change_meta_data(const MetaData& md) {
    MetaDataList v_md;
    v_md.push_back(md);
    change_meta_data(v_md);
}


void GUI_TagEdit::change_meta_data(const MetaDataList& vec) {

	init();

    _cur_idx = -1;
    _lst_new_albums.clear();
    _lst_new_artists.clear();
    _vec_tmp_metadata.clear();
    _vec_org_metadata.clear();
    _idx_affected_by_tag.clear();

    if(vec.size() <= 0) return;

    _vec_org_metadata = vec;
    foreach(MetaData md, _vec_org_metadata) {
        _vec_tmp_metadata.push_back(md);
        _lst_new_albums.push_back(md.album);
        _lst_new_artists.push_back(md.artist);
    }

    _cur_idx = 0;

    show_win();

	pb_progress->hide();

	le_tag_from_path->setText("");
	btn_tag_undo->setEnabled(false);
	btn_tag_apply->setEnabled(false);

	pb_next_track->setEnabled( (vec.size() > 1) );
	pb_prev->setEnabled(false);

    show_metadata();
}


void GUI_TagEdit::prev_button_clicked() {

    save_metadata();

    if(_cur_idx > 0) {
        _cur_idx --;
        if(_cur_idx == 0) {
			pb_prev->setEnabled(false);
        }

    }

	pb_next_track->setEnabled(true);
    show_metadata();
}


void GUI_TagEdit::next_button_clicked() {

    save_metadata();
    int tmp_md_size = _vec_tmp_metadata.size();

    if(_cur_idx < tmp_md_size -1 ) {
        _cur_idx ++;

        if(_cur_idx == tmp_md_size -1) {
			pb_next_track->setEnabled(false);
        }
    }

	pb_prev->setEnabled(true);

    show_metadata();
}

void GUI_TagEdit::clear_checkboxes() {

	cb_album_all->setChecked(false);
	cb_artist_all-> setChecked(false);
	cb_genre_all->setChecked(false);
	cb_year_all->setChecked(false);
	cb_discnumber_all->setChecked(false);
}


void GUI_TagEdit::ok_button_clicked() {
    if(_cur_idx == -1) _cur_idx = 0;
    save_metadata();

    QList<Album> v_album;
    QList<Artist> v_artist;

    // stores only artists/albums if track is in database
    check_for_new_album_and_artist(v_album, v_artist);

    bool b = store_to_database(v_album, v_artist);

	pb_progress->hide();
    clear_checkboxes();
    
    MetaDataList v_md2send = _vec_tmp_metadata;

    emit id3_tags_changed();
    emit id3_tags_changed(v_md2send);

    if(b)
        emit sig_success(b);

    if(!_parent) {
        hide();
        close();
    }
}


void GUI_TagEdit::cancel_button_clicked() {

    clear_checkboxes();
    emit sig_cancelled();

    if(!_parent) {
        hide();
        close();
    }
}


void GUI_TagEdit::all_albums_clicked() {
    for(int i=0; i<_vec_tmp_metadata.size(); i++) {
		_vec_tmp_metadata[i].album = le_album->text();
        _lst_new_albums[i] = _vec_tmp_metadata[i].album;
    }
}

void GUI_TagEdit::all_artists_clicked() {
    for(int i=0; i<_vec_tmp_metadata.size(); i++) {
		_vec_tmp_metadata[i].artist = le_artist->text();
        _lst_new_artists[i] = _vec_tmp_metadata[i].artist;
    }
}

void GUI_TagEdit::all_genre_clicked() {

    for(int i=0; i<_vec_tmp_metadata.size(); i++) {
        _vec_tmp_metadata[i].genres.clear();

        QStringList genres;
		genres = le_genres->text().split(QRegExp(",|\\|/|;|-"));

        _vec_tmp_metadata[i].genres = genres;
    }
}
void GUI_TagEdit::all_year_clicked() {
    for(int i=0; i<_vec_tmp_metadata.size(); i++) {
		_vec_tmp_metadata[i].year = sb_year->value();
    }
}

void GUI_TagEdit::all_discnumber_clicked() {
    for(int i=0; i<_vec_tmp_metadata.size(); i++) {
        _vec_tmp_metadata[i].discnumber = sb_discnumber->value();
    }
}

void GUI_TagEdit::all_tag_clicked() {

}

void GUI_TagEdit::album_changed(QString text) {
    Q_UNUSED(text);
}

void GUI_TagEdit::artist_changed(QString text) {
    Q_UNUSED(text);

}


void GUI_TagEdit::show_metadata() {

    const MetaData& md = _vec_tmp_metadata[_cur_idx];

	le_album->setText(md.album);
	le_artist->setText(md.artist);
	le_title->setText(md.title);
	sb_track_num->setValue(md.track_num);
	sb_year->setValue(md.year);
	le_genres->setText(md.genres.join(","));
	sb_discnumber->setValue(md.discnumber);

	lab_filepath->clear();
	lab_filepath->insertPlainText(QDir(md.filepath).absolutePath());
	lab_track_num->setText(tr("Track ") + QString::number(_cur_idx+1) + "/" + QString::number(_vec_org_metadata.size()));

	tag_from_path_text_changed(le_tag_from_path->text());
	btn_tag_undo->setEnabled(_idx_affected_by_tag[_cur_idx]);
}


void GUI_TagEdit::save_metadata() {
	if ( cb_album_all->isChecked() )
        all_albums_clicked();
    

	if (cb_artist_all->isChecked())
        all_artists_clicked();
    
	if (cb_year_all->isChecked())
        all_year_clicked();
    
	if (cb_genre_all->isChecked())
        all_genre_clicked();
    
	if(cb_discnumber_all->isChecked())
	all_discnumber_clicked();
    
	if(cb_tag_all->isChecked())
        all_tag_clicked();
    

	_lst_new_albums[_cur_idx] = le_album->text();
	_lst_new_artists[_cur_idx] = le_artist->text();

	_vec_tmp_metadata[_cur_idx].album = le_album->text();
	_vec_tmp_metadata[_cur_idx].artist = le_artist->text();
	_vec_tmp_metadata[_cur_idx].title = le_title->text();
	_vec_tmp_metadata[_cur_idx].track_num = sb_track_num->value();
	_vec_tmp_metadata[_cur_idx].year = sb_year->value();
	_vec_tmp_metadata[_cur_idx].genres = le_genres->text().split(QRegExp(",|\\|/|;|-"));
	_vec_tmp_metadata[_cur_idx].discnumber = sb_discnumber->value();

}





void GUI_TagEdit::check_for_new_album_and_artist(QList<Album>& v_album, QList<Artist>& v_artist) {


    /* create lists with all albums and artists for the tracks
     * If a track has a new album/artist create new IDs for it */
    for(int track = 0; track<_vec_org_metadata.size(); track++) {

            const MetaData& md = _db->getTrackByPath(_vec_org_metadata[track].filepath);
            if( md.id < 0) {
                continue;
            }

            QString new_album_name =  _lst_new_albums[track];
            QString new_artist_name = _lst_new_artists[track];

            bool insert = true;
            // search if new album is already in list
            for(int i=0; i<v_album.size(); i++) {
                if(v_album[i].name == new_album_name) {
                    insert = false;
                    break;
                }
            }

	    // insert new album
            if(insert) {
                Album album;
                album.name = new_album_name;
                album.id = _max_album_id;
                _max_album_id++;
                v_album.push_back(album);
            }

            // search if new artist is already in list
            insert = true;
            for(int i=0; i<v_artist.size(); i++) {
                if(v_artist[i].name == new_artist_name) {
                    insert = false;
                    break;
                }
            }

	    // insert new artist
            if(insert) {
                Artist artist;
                artist.name = new_artist_name;
                artist.id = _max_artist_id;
                _max_artist_id++;
                v_artist.push_back(artist);
            }
    }

    AlbumList vec_albums;
    ArtistList vec_artists;

    _db->getAllAlbums(vec_albums);
    _db->getAllArtists(vec_artists);

    for(int track = 0; track<_vec_org_metadata.size(); track++) {

        QString new_album_name =  _lst_new_albums[track];
        QString new_artist_name = _lst_new_artists[track];

        bool album_found = false;
        int album_id = -1;

        /* check the track, if it has a NEW album */
        foreach(Album album, vec_albums) {

            // album is already in db
            if(new_album_name == album.name) {
                album_found = true;
                album_id = album.id;

                // delete the album out of the list, we do not have to insert it
                for(int j=0; j<v_album.size(); j++) {
                    if(v_album[j].name == new_album_name) {
                        v_album.removeAt(j);
                        break;
                    }
                }

                break;
            }
        }

        if(!album_found) {

            foreach(Album album, v_album) {
                if(album.name == new_album_name) {
                    album_id = album.id;
                    break;
                }
            }
        }

        _vec_tmp_metadata[track].album_id = album_id;


        bool artist_found = false;
        int artist_id = -1;
        for(int i=0; i<vec_artists.size(); i++) {
            if(new_artist_name == vec_artists[i].name) {
                artist_found = true;
                artist_id = vec_artists[i].id;

                for(int j=0; j<v_artist.size(); j++) {
                    if(v_artist[j].name == new_artist_name) {
                        v_artist.removeAt(j);
                        break;
                    }
                }

                break;
            }
        }

        if(!artist_found) {
            for(int i=0; i<v_artist.size(); i++) {
                if(v_artist[i].name == new_artist_name) {
                    artist_id = v_artist[i].id;
                    break;
                }
            }
        }

        _vec_tmp_metadata[track].artist_id = artist_id;
    }
}


void GUI_TagEdit::change_mp3_file(MetaData& md) {

    ID3::setMetaDataOfFile(md);
}


bool GUI_TagEdit::store_to_database(QList<Album>& new_albums, QList<Artist>& new_artists) {


    if(new_albums.size() > 0 || new_artists.size() > 0) {
         QMessageBox msgBox(this);
	 QString text = tr("You are about to insert<br /><b>%1</b> new album(s) and <b>%2</b> new artist(s)")
			.arg(new_albums.size())
			.arg(new_artists.size());

         msgBox.setText(text);
         msgBox.setInformativeText(tr("Proceed?"));
         msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
         msgBox.setDefaultButton(QMessageBox::Yes);
         int ret = msgBox.exec();

         if(ret != QMessageBox::Yes) return false;
    }

    foreach(Album album, new_albums) {
        _db->insertAlbumIntoDatabase(album);
    }


    foreach(Artist artist, new_artists) {
        _db->insertArtistIntoDatabase(artist);
    }

	pb_progress->setVisible(true);

    MetaDataList v_md_tmp;
    int v_md_size = _vec_tmp_metadata.size();

    for(int i=0; i<v_md_size; i++) {

        MetaData& md = _vec_tmp_metadata[i];
		pb_progress->setValue( (int)(i * 100.0 / v_md_size) );

        change_mp3_file(md);
        if(!md.is_extern){
            v_md_tmp.push_back(md);
        }

        usleep(10000);
    }

    return CDatabaseConnector::getInstance()->storeMetadata(v_md_tmp);
}


 bool  GUI_TagEdit::is_valid_tag_str(QString str) {
     return (has_tag(str) && (!has_open_tag(str)));
 }

 bool GUI_TagEdit::has_open_tag(QString str) {

     int open = str.count('<');
     int close = str.count('>');

     return (open > close);

 }

 bool GUI_TagEdit::has_tag(QString str) {

     return str.contains(TAG_TITLE) ||
             str.contains(TAG_ALBUM) ||
             str.contains(TAG_ARTIST) ||
             str.contains(TAG_TRACK_NUM) ||
             str.contains(TAG_YEAR) ||
             str.contains(TAG_DISC);
 }


bool GUI_TagEdit::remove_aftertag_str(QString& str, QString aftertag, bool looking_for_num) {

    if(aftertag.size() == 0 && !looking_for_num ) return true;


    for(int i=str.size()-1; i>=0; i--) {

        bool cond = true;
        if(looking_for_num)
            cond = ((i > aftertag.size()) && (str[i - aftertag.size()].isDigit()));


        if(str.endsWith(aftertag) && cond) {
            str = str.left(str.size() - aftertag.size());
            return true;
        }

        str = str.left(i);
        if(str.size() == 0) break;
    }

    return false;

}


bool GUI_TagEdit::calc_tag(int idx, MetaData& md) {

	QString str = le_tag_from_path->text();
    QString tag_str = str;

    md = _vec_org_metadata[idx];

    if(!is_valid_tag_str(str)) {
        return false;
    }

    QString path = QDir(_vec_org_metadata[idx].filepath).absolutePath();
    QString path_copy; // we wait until extension is removed

    // get && remove extensions
    int cur_pos=0;
    for(int i=path.length() - 1; i>=0; i--) {
        if(path.at(i) == '.') {
            cur_pos = i;
            break;
        }
    }

    path = path.left(cur_pos);
    path_copy = path;

    QMap<QString, int> start_positions_path;
    QMap<QString, int> end_positions_path;

    QMap<int, QString> tag_positions;
    tag_positions[tag_str.indexOf(TAG_TITLE)] = TAG_TITLE;
    tag_positions[tag_str.indexOf(TAG_ALBUM)] = TAG_ALBUM;
    tag_positions[tag_str.indexOf(TAG_ARTIST)] = TAG_ARTIST;
    tag_positions[tag_str.indexOf(TAG_TRACK_NUM)] = TAG_TRACK_NUM;
    tag_positions[tag_str.indexOf(TAG_YEAR)] = TAG_YEAR;
    tag_positions[tag_str.indexOf(TAG_DISC)] = TAG_DISC;

    int min_of_idx = 100000;
    foreach(int i, tag_positions.keys())
        if(i < min_of_idx && i >= 0) min_of_idx = i;


    QString tmp_str;
    bool success = true;
    QString last_tag;


    for(int i=tag_str.size(); i>=0; i--) {

        // we are only interested in an i where the tag is
        if(!tag_positions.contains(i)) continue;

        // e.g. title, artist, album, year
        int tag_key = i;
        QString tag_value = tag_positions[i];




        bool looking_for_num = false;
        if(tag_value == TAG_YEAR || tag_value == TAG_TRACK_NUM || tag_value == TAG_DISC)
            looking_for_num = true;

        // found a tag at i. We are only interested in
        // everything what happens right of the tag
        tmp_str = tag_str.right(tag_str.size() - i);
        QString after_tag = "";

        // remove delim signs after the tag string
        after_tag = tmp_str.right(tmp_str.size() - tag_positions[i].length());


        // remove everything after tag if it's like int the tag string
        if(!remove_aftertag_str(path, after_tag, looking_for_num)) {
            success = false;
            break;
        }

        // save position where the tag ends in path
        end_positions_path[tag_value] = path.size() - 1;
        start_positions_path[tag_value] = 0;
        start_positions_path[last_tag] = path.size() + after_tag.size();


        // tag does not interest anymore. Remove
        tag_str = tag_str.left(i);

        if(tag_key == min_of_idx) {
            start_positions_path[tag_value] = path.lastIndexOf(tag_str) + tag_str.length();
            break;
        }

        last_tag = tag_value;
    }

    if(!success) {
        return false;
    }


    for(int i=0; i<path_copy.size(); i++) {
        if(!end_positions_path.values().contains(i)) continue;

        QString final_key = end_positions_path.key(i);
        QString final_value = path_copy.mid(start_positions_path[final_key], end_positions_path[final_key] - start_positions_path[final_key] + 1);

        if(final_key == TAG_TITLE) {
           md.title = final_value;
        }

        else if(final_key == TAG_ALBUM) {
            md.album = final_value;
        }

        else if(final_key == TAG_ARTIST) {
            md.artist = final_value;
        }

        else if(final_key == TAG_YEAR) {
            md.year = final_value.toInt();
        }

        else if(final_key == TAG_TRACK_NUM) {
            md.track_num = final_value.toInt();
        }

        else if(final_key == TAG_DISC) {
            md.discnumber = final_value.toInt();
        }
    }

    return true;
}

void GUI_TagEdit::tag_from_path_text_changed(const QString& str) {

    MetaData md;

    if(calc_tag(_cur_idx, md)) {
		le_tag_from_path->setStyleSheet("");
		btn_tag_apply->setEnabled(true);
    }

    else {
		le_tag_from_path->setStyleSheet("color: red;");
		btn_tag_apply->setEnabled(false);
    }
}

void GUI_TagEdit::help_tag_clicked() {

    QString info_str = tr("Here you can setup an expression for fast tagging") + "<br />" +
            "<br />" +
            tr("Valid macros are:") + CAR_RET +
            "&lt;ar&gt;" + tr("for artist") + CAR_RET +
            "&lt;al&gt;" + tr("for album") + CAR_RET +
            "&lt;t&gt;" +  tr("for title") + CAR_RET +
            "&lt;nr&gt;" + tr("for track number") + CAR_RET +
            "&lt;y&gt;" +  tr("for year") + CAR_RET +
            "&lt;d&gt;" +  tr("for discnumber") + CAR_RET +
            "<br /><br />" +
            tr("Example: ") +  CAR_RET +
            tr("Your mp3 files have no or incomplete metadata, but all MP3 files in this folder look like this:") + CAR_RET +
            "/home/lucky/Sound/Megadeth/Megadeth - 1997 - Cryptic writings/01 - Trust.mp3" + CAR_RET +
            tr("Then your expression should look like this:") + CAR_RET +
            "<b>/&lt;ar&gt; - &lt;y&gt; - &lt;al&gt;/&lt;nr&gt; - &lt;t&gt;</b>" + CAR_RET + CAR_RET +
            tr("Note that no tag may appear twice. If you want to ignore something you have to write it like") + CAR_RET +
            "/home/lucky/Sound/Megadeth/Megadeth - 1997 - Cryptic writings/01 - 1997_Trust.mp3" + CAR_RET +
            "<b>/&lt;ar&gt; - &lt;y&gt; - &lt;al&gt;/&lt;tn&gt; - 1997_&lt;t&gt;</b>";

    QMessageBox::information(this, tr("How to use"), info_str);


}

void GUI_TagEdit::undo_tag_clicked() {

	if(!cb_tag_all->isChecked()) {
        _vec_tmp_metadata[_cur_idx] = _vec_org_metadata[_cur_idx];
        _idx_affected_by_tag[_cur_idx] = false;
        _lst_new_albums[_cur_idx] = _vec_tmp_metadata[_cur_idx].album;
        _lst_new_artists[_cur_idx] = _vec_tmp_metadata[_cur_idx].artist;
    }

    else{
        for(int i=0; i<_vec_org_metadata.size(); i++) {
            if(_idx_affected_by_tag[i] == true) {
                _vec_tmp_metadata[i] = _vec_org_metadata[i];
                _idx_affected_by_tag[i] = false;
                _lst_new_albums[i] = _vec_tmp_metadata[i].album;
                _lst_new_artists[i] = _vec_tmp_metadata[i].artist;
            }
        }
    }

    show_metadata();
}

void GUI_TagEdit::apply_tag_clicked() {

	if(!cb_tag_all->isChecked()) {
        MetaData md;
        _idx_affected_by_tag[_cur_idx] = calc_tag(_cur_idx, md);
        _vec_tmp_metadata[_cur_idx] = md;
        _lst_new_albums[_cur_idx] = _vec_tmp_metadata[_cur_idx].album;
        _lst_new_artists[_cur_idx] = _vec_tmp_metadata[_cur_idx].artist;
    }

    else{
        for(int i=0; i<_vec_org_metadata.size(); i++) {
            MetaData md;
            if(calc_tag(i, md)) {
                _vec_tmp_metadata[i] = md;
                _idx_affected_by_tag[i] = true;
                _lst_new_albums[i] = _vec_tmp_metadata[i].album;
                _lst_new_artists[i] = _vec_tmp_metadata[i].artist;

            }

            else{
                _idx_affected_by_tag[i] = false;
            }
        }
    }

    show_metadata();
}


void  GUI_TagEdit::show_win() {

    show();
}
