/*
 * GUI_TagEdit.cpp
 *
 *  Created on: May 24, 2011
 *      Author: luke
 */

#include "GUI/tagedit/GUI_TagEdit.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/id3.h"
#include "ui_GUI_TagEdit.h"

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>

#include <vector>

using namespace std;


GUI_TagEdit::GUI_TagEdit() {
	this->ui = new Ui::GUI_TagEdit();
	this->ui->setupUi(this);

	_cur_idx = -1;

	_db = CDatabaseConnector::getInstance();

	init();

	close();

	connect(this->ui->pb_next_track, SIGNAL(released()), this, SLOT(next_button_clicked()));
	connect(this->ui->pb_prev, SIGNAL(released()), this, SLOT(prev_button_clicked()));
	connect(this->ui->bb_ok_cancel, SIGNAL(accepted()), this, SLOT(ok_button_clicked()));
	connect(this->ui->bb_ok_cancel, SIGNAL(rejected()), this, SLOT(cancel_button_clicked()));
	connect(this->ui->btn_all_album, SIGNAL(released()), this, SLOT(all_albums_clicked()));
	connect(this->ui->btn_all_artist, SIGNAL(released()), this, SLOT(all_artists_clicked()));
	connect(this->ui->btn_all_year, SIGNAL(released()), this, SLOT(all_year_clicked()));

}




GUI_TagEdit::~GUI_TagEdit() {
	// TODO Auto-generated destructor stub
}


void GUI_TagEdit::init(){

	_vec_albums.clear();
	_vec_artists.clear();

	_db->getAllAlbums(_vec_albums);
	_db->getAllArtists(_vec_artists);

	_max_album_id = -1;
	_max_artist_id = -1;


	_max_album_id = _db->getMaxAlbumID() + 1;
	_max_artist_id = _db->getMaxArtistID() + 1;

}


void GUI_TagEdit::change_meta_data(const vector<MetaData>& vec){

	this->init();

	this->ui->pb_progress->hide();
	_cur_idx = -1;
	_lst_new_albums.clear();
	_lst_new_artists.clear();
	_vec_tmp_metadata.clear();

	if(vec.size() <= 0) return;

	this->_vec_org_metadata = vec;
	for(uint i=0; i<_vec_org_metadata.size(); i++){
		_vec_tmp_metadata.push_back(_vec_org_metadata[i]);
		_lst_new_albums.push_back(_vec_org_metadata[i].album);
		_lst_new_artists.push_back(_vec_org_metadata[i].artist);
	}

	show();
	_cur_idx = 0;
	this->ui->pb_next_track->setEnabled( (vec.size() > 1) );
	this->ui->pb_prev->setEnabled(false);

	this->show_metadata();
}


void GUI_TagEdit::prev_button_clicked(){



	save_metadata();

	if(_cur_idx > 0){
		_cur_idx --;
		if(_cur_idx == 0){
			this->ui->pb_prev->setEnabled(false);
		}

		this->ui->pb_next_track->setEnabled(true);
	}


	show_metadata();

}


void GUI_TagEdit::next_button_clicked(){


	save_metadata();

	if(_cur_idx < (int) _vec_tmp_metadata.size() -1 ){
		_cur_idx ++;
		if(_cur_idx == (int) _vec_tmp_metadata.size() -1){
			this->ui->pb_next_track->setEnabled(false);
		}
	}

	this->ui->pb_prev->setEnabled(true);

	show_metadata();
}


void GUI_TagEdit::ok_button_clicked(){
	if(_cur_idx == -1) _cur_idx = 0;
	save_metadata();
	QList<Album> v_album;
	QList<Artist> v_artist;
	check_for_new_album_and_artist(v_album, v_artist);
	bool b = store_to_database(v_album, v_artist) ;
	if(b) close();


}

void GUI_TagEdit::cancel_button_clicked(){
	this->close();
}


void GUI_TagEdit::all_albums_clicked(){
	for(uint i=0; i<_vec_tmp_metadata.size(); i++){
		_vec_tmp_metadata[i].album = this->ui->le_album->text();
		_lst_new_albums[i] = _vec_tmp_metadata[i].album;
	}
}
void GUI_TagEdit::all_artists_clicked(){
	for(uint i=0; i<_vec_tmp_metadata.size(); i++){
		_vec_tmp_metadata[i].artist = this->ui->le_artist->text();
		_lst_new_artists[i] = _vec_tmp_metadata[i].artist;
	}
}
void GUI_TagEdit::all_genre_clicked(){
	/*for(int i=0; i<_vec_tmp_metadata.size(); i++){
		_vec_tmp_metadata[i].ge = this->ui->le_album->text();
	}*/
}
void GUI_TagEdit::all_year_clicked(){
	for(uint i=0; i<_vec_tmp_metadata.size(); i++){
		_vec_tmp_metadata[i].year = this->ui->sb_year->value();
	}
}



void GUI_TagEdit::album_changed(QString text){
	Q_UNUSED(text);
}


void GUI_TagEdit::artist_changed(QString text){
	Q_UNUSED(text);

}


void GUI_TagEdit::show_metadata(){
	this->ui->le_album->setText(_vec_tmp_metadata[_cur_idx].album);
	this->ui->le_artist->setText(_vec_tmp_metadata[_cur_idx].artist);
	this->ui->le_title->setText(_vec_tmp_metadata[_cur_idx].title);
	this->ui->sb_track_num->setValue(_vec_tmp_metadata[_cur_idx].track_num);
	this->ui->sb_year->setValue(_vec_tmp_metadata[_cur_idx].year);

	this->ui->lab_filepath->setText(_vec_org_metadata[_cur_idx].filepath);
		this->ui->lab_track_num->setText("Track " + QString::number(_cur_idx+1) + "/" + QString::number(_vec_org_metadata.size()));


}


void GUI_TagEdit::save_metadata(){

	_lst_new_albums[_cur_idx] = this->ui->le_album->text();
	_lst_new_artists[_cur_idx] = this->ui->le_artist->text();

	_vec_tmp_metadata[_cur_idx].album = this->ui->le_album->text();
	_vec_tmp_metadata[_cur_idx].artist = this->ui->le_artist->text();
	_vec_tmp_metadata[_cur_idx].title = this->ui->le_title->text();
	_vec_tmp_metadata[_cur_idx].track_num = this->ui->sb_track_num->value();
	_vec_tmp_metadata[_cur_idx].year = this->ui->sb_year->value();

	_vec_tmp_metadata[_cur_idx].print();
	//if(_vec_tmp_metadata[_cur_idx].year < 1000) _vec_tmp_metadata[_cur_idx].year = 1000;
}


void GUI_TagEdit::check_for_new_album_and_artist(QList<Album>& v_album, QList<Artist>& v_artist){


	/* create lists with all albums and artists for the tracks
	 * If a track has a new album/artist create new IDs for it */
	for(uint track = 0; track<_vec_org_metadata.size(); track++){

			QString new_album_name =  _lst_new_albums[track];
			QString new_artist_name = _lst_new_artists[track];

			bool insert = true;
			for(int i=0; i<v_album.size(); i++){
				//qDebug() << "New album: compare " << v_album[i] << " with " << new_album_name;
				if(v_album[i].name == new_album_name) {
					insert = false;
					break;
				}
			}

			if(insert){
				Album album;
				album.name = new_album_name;
				album.id = _max_album_id;
				_max_album_id++;
				v_album.push_back(album);
			}

			insert = true;
			for(int i=0; i<v_artist.size(); i++){
				if(v_artist[i].name == new_artist_name) {
					insert = false;
					break;
				}
			}

			if(insert){
				Artist artist;
				artist.name = new_artist_name;
				artist.id = _max_artist_id;
				_max_artist_id++;
				v_artist.push_back(artist);
			}

	}


	for(uint track = 0; track<_vec_org_metadata.size(); track++){


		QString new_album_name =  _lst_new_albums[track];
		QString new_artist_name = _lst_new_artists[track];



		bool album_found = false;
		int album_id = -1;
		/* check the track, if it has a NEW album */

		for(uint i=0; i<_vec_albums.size(); i++){

			// album is already in db
			if(new_album_name == _vec_albums[i].name){
				album_found = true;
				album_id = _vec_albums[i].id;

				// delete the album out of the list, we do not have to insert it
				for(int j=0; j<v_album.size(); j++){
					if(v_album[j].name == new_album_name){
						v_album.removeAt(j);
						break;
					}
				}

				break;
			}
		}

		if(!album_found){

			for(int i=0; i<v_album.size(); i++){
				if(v_album[i].name == new_album_name){
					album_id = v_album[i].id;
					break;
				}
			}

			qDebug() << "New album has to be inserted " << album_id;

		}

		_vec_tmp_metadata[track].album_id = album_id;


		bool artist_found = false;
		int artist_id = -1;
		for(uint i=0; i<_vec_artists.size(); i++){
			if(new_artist_name == _vec_artists[i].name){
				artist_found = true;
				artist_id = _vec_artists[i].id;


				for(int j=0; j<v_artist.size(); j++){
					if(v_artist[j].name == new_artist_name){
						v_artist.removeAt(j);
						break;
					}
				}


				break;
			}
		}

		if(!artist_found){
			for(int i=0; i<v_artist.size(); i++){
				if(v_artist[i].name == new_artist_name){
					artist_id = v_artist[i].id;
					break;
				}
			}

			qDebug() << "New artist has to be inserted " << artist_id;

		}

		_vec_tmp_metadata[track].artist_id = artist_id;



	}
}


void GUI_TagEdit::change_mp3_file(MetaData& md){

	ID3::setMetaDataOfFile(md);
}


bool GUI_TagEdit::store_to_database(QList<Album>& new_albums, QList<Artist>& new_artists){

	if(new_albums.size() > 0 || new_artists.size() > 0){
		 QMessageBox msgBox;
		 msgBox.setText("You are about to insert<br /><b>" + QString::number(new_albums.size()) + "</b> new album(s) and<br /><b>" + QString::number(new_artists.size()) + "</b> new artist(s).");
		 msgBox.setInformativeText("Proceed?");
		 msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		 msgBox.setDefaultButton(QMessageBox::No);
		 int ret = msgBox.exec();

		 if(ret != QMessageBox::Yes) return false;
	}



	for(int i=0; i<new_albums.size(); i++){
		qDebug() << "New album: " << new_albums[i].name;
		_db->insertAlbumIntoDatabase(new_albums[i]);
	}

	for(int i=0; i<new_artists.size(); i++){
		_db->insertArtistIntoDatabase(new_artists[i]);
	}

	this->ui->pb_progress->setVisible(true);

	for(uint i=0; i<_vec_tmp_metadata.size(); i++){

		this->ui->pb_progress->setValue( (int)(i * 100.0 / _vec_tmp_metadata.size()));
		CDatabaseConnector::getInstance()->updateTrack(_vec_tmp_metadata[i]);
		change_mp3_file(_vec_tmp_metadata[i]);

	}

	this->ui->pb_progress->setVisible(false);


	emit id3_tags_changed();
	emit id3_tags_changed(_vec_tmp_metadata);


	return true;



}
