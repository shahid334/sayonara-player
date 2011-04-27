/*
 * GUI_Library_windowed.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#include "GUI/Library/GUI_Library_windowed.h"
#include "GUI/Library/LibraryItemModelTracks.h"
#include <GUI/Library/LibraryItemModelAlbums.h>
#include <GUI/Library/LibraryItemModelArtists.h>
#include "ui_GUI_Library_windowed.h"
#include <QDebug>
#include <QPoint>
#include <QMouseEvent>
#include <vector>
#include <HelperStructs/Helper.h>
#include <HelperStructs/MetaData.h>

using namespace std;

GUI_Library_windowed::GUI_Library_windowed(QWidget* parent) : QWidget(parent) {
	this->ui = new Ui::Library_windowed();
	this->ui->setupUi(this);

	this->_track_model = new LibraryItemModelTracks();
	this->_album_model = new LibraryItemModelAlbums();
	this->_artist_model = new LibraryItemModelArtists();

	this->ui->tb_title->setModel(this->_track_model);
	this->ui->lv_album->setModel(this->_album_model);
	this->ui->lv_artist->setModel(this->_artist_model);

	this->ui->gridLayout->setRowStretch(0, 0);
	this->ui->gridLayout->setRowStretch(1, 0);
	this->ui->gridLayout->setRowStretch(3, 2);
	this->ui->gridLayout->setRowStretch(4, 3);

	this->ui->lv_album->setAlternatingRowColors(true);
	this->ui->lv_artist->setAlternatingRowColors(true);

	connect(this->ui->tb_title, SIGNAL(	pressed ( const QModelIndex & )), this, SLOT(track_pressed(const QModelIndex&)));
	connect(this->ui->lv_artist, SIGNAL( clicked ( const QModelIndex & )), this, SLOT(artist_changed(const QModelIndex&)));
	connect(this->ui->lv_album, SIGNAL( clicked ( const QModelIndex & )), this, SLOT(album_changed(const QModelIndex&)));
	connect(this->ui->btn_clear, SIGNAL(clicked()), this, SLOT(clear_button_pressed()));

}

GUI_Library_windowed::~GUI_Library_windowed() {
	// TODO Auto-generated destructor stub
}


void GUI_Library_windowed::fill_library_tracks(vector<MetaData>& v_metadata){

	_v_metadata.clear();
	_v_metadata = v_metadata;

	this->_track_model->removeRows(0, this->_track_model->rowCount());
	this->_track_model->insertRows(0, v_metadata.size());

	for(uint i=0; i<v_metadata.size(); i++){

		QModelIndex idx = _track_model->index(i, 0);

		MetaData md = v_metadata.at(i);


		QStringList list;
		list.push_back(md.title);
		list.push_back(md.artist);
		list.push_back(md.album);

		int min, sec;
		Helper::cvtSecs2MinAndSecs(md.length_ms/1000, &min, &sec);
		QString length = QString::fromStdString(Helper::cvtNum2String(min, 2)) + ":" + QString::fromStdString(Helper::cvtNum2String(sec, 2));
		list.push_back(length);
		list.push_back(QString::number(md.year));

		this->_track_model->setData(idx, list, Qt::EditRole);
	}


}


void GUI_Library_windowed::fill_library_albums(vector<Album>& albums){

	_v_albums.clear();
	_v_albums = albums;

	this->_album_model->removeRows(0, this->_album_model->rowCount());
	this->_album_model->insertRows(0, albums.size());

	for(uint i=0; i<albums.size(); i++){
		QModelIndex idx = this->_album_model->index(i, 0);

		QString albumname = albums.at(i).name;
		if(albums.at(i).name.isEmpty()) albumname = "<Unknown>";

		QString data = albumname + ", " +
						QString::number(albums.at(i).num_songs) +
						" track";

		if(albums.at(i).num_songs != 1) data += "s";

		if(albums.at(i).year != 0) data += " (" + QString::number(albums.at(i).year) + ")";
		this->_album_model->setData(idx, data, Qt::EditRole );
	}

}




void GUI_Library_windowed::fill_library_artists(vector<Artist>& artists){

	_v_artists.clear();
	_v_artists = artists;

	this->_artist_model->removeRows(0, this->_artist_model->rowCount());
	this->_artist_model->insertRows(0, artists.size());

	for(uint i=0; i<artists.size(); i++){
		QModelIndex idx = this->_artist_model->index(i, 0);

		QString artistname = artists.at(i).name;
		if(artists.at(i).name.isEmpty()) artistname = "<Unknown>";

		QString data = artistname + ", " +
						QString::number(artists.at(i).num_songs) +
						" track";

		if(artists.at(i).num_songs != 1) data += "s";

		this->_artist_model->setData(idx, data, Qt::EditRole );
	}

}


void GUI_Library_windowed::resizeEvent(QResizeEvent* e){

	Q_UNUSED(e);

	QSize tmpSize = this->ui->tb_title->size();
	int width = tmpSize.width();
	if(width > 550){
		this->ui->tb_title->setColumnWidth(0, width * 0.35);
		this->ui->tb_title->setColumnWidth(1, width * 0.23);
		this->ui->tb_title->setColumnWidth(2, width * 0.23);
		this->ui->tb_title->setColumnWidth(3, width * 0.08);
		this->ui->tb_title->setColumnWidth(4, width * 0.08);
		this->ui->tb_title->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}

	else {
		this->ui->tb_title->setColumnWidth(0, width * 0.4);
		this->ui->tb_title->setColumnWidth(1, width * 0.3);
		this->ui->tb_title->setColumnWidth(2, width * 0.3);
		this->ui->tb_title->setColumnWidth(3, 80);
		this->ui->tb_title->setColumnWidth(4, 80);
		this->ui->tb_title->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);


	}

}




void GUI_Library_windowed::track_pressed(const QModelIndex& idx){

	Q_UNUSED(idx);

	QDrag* drag = new QDrag(this);
	QMimeData* mime = new QMimeData();

	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows(0);

	QList<QVariant> list2send;

	for(int i=0; i<idx_list.size(); i++){
		int row = idx_list.at(i).row();

		QStringList metadata = _v_metadata.at(row).toStringList();
		list2send.push_back(metadata);
	}

	mime->setProperty("data", (QVariant) list2send);
	drag->setMimeData(mime);

	Qt::DropAction dropAction = drag->exec();
	Q_UNUSED(dropAction);

}


void GUI_Library_windowed::album_changed(const QModelIndex& idx){
	emit album_changed_signal(_v_albums.at(idx.row()).id);

}

void GUI_Library_windowed::artist_changed(const QModelIndex& idx){
	emit artist_changed_signal(_v_artists.at(idx.row()).id);

}

void GUI_Library_windowed::clear_button_pressed(){
	emit clear_signal();
}
