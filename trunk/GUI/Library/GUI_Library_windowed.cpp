/*
 * GUI_Library_windowed.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#include "GUI/Library/GUI_Library_windowed.h"
#include "GUI/Library/LibraryItemModelTracks.h"
#include <GUI/Library/LibraryItemModelAlbums.h>
#include <GUI/Library/LibraryItemDelegateAlbums.h>
#include <GUI/Library/LibraryItemModelArtists.h>
#include "ui_GUI_Library_windowed.h"
#include <QDebug>
#include <QPoint>
#include <QMouseEvent>
#include <vector>
#include <HelperStructs/Helper.h>
#include <HelperStructs/MetaData.h>
#include <library/CDatabaseConnector.h>


using namespace std;

GUI_Library_windowed::GUI_Library_windowed(QWidget* parent) : QWidget(parent) {



	this->ui = new Ui::Library_windowed();
	this->ui->setupUi(this);

	this->_track_model = new LibraryItemModelTracks();
	this->_album_model = new LibraryItemModelAlbums();
	this->_album_delegate = new LibraryItemDelegateAlbums(this->ui->lv_album);
	this->_artist_model = new LibraryItemModelArtists();
	this->_artist_delegate = new LibraryItemDelegateAlbums(this->ui->lv_artist);

	this->ui->tb_title->setModel(this->_track_model);
	this->ui->lv_album->setModel(this->_album_model);
	this->ui->lv_album->setItemDelegate(this->_album_delegate);
	this->ui->lv_artist->setModel(this->_artist_model);
	this->ui->lv_artist->setItemDelegate(this->_artist_delegate);


	this->ui->lv_album->setDragEnabled(true);
	this->ui->lv_artist->setDragEnabled(true);
	this->ui->tb_title->setDragEnabled(true);

	this->ui->btn_clear->setIcon(QIcon(Helper::getIconPath() + "broom.png"));


	this->ui->lv_artist->setAlternatingRowColors(true);

	connect(this->ui->btn_clear, SIGNAL( clicked()), this, SLOT(clear_button_pressed()));
	connect(this->ui->le_search, SIGNAL( textEdited(const QString&)), this, SLOT(text_line_edited(const QString&)));

	connect(this->ui->lv_album, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(album_chosen(const QModelIndex & )));
	connect(this->ui->lv_artist, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(artist_chosen(const QModelIndex & )));
	connect(this->ui->tb_title, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(track_chosen(const QModelIndex & )));

	connect(this->ui->tb_title, SIGNAL(pressed ( const QModelIndex & )), this, SLOT(track_pressed(const QModelIndex&)));
	connect(this->ui->lv_album, SIGNAL(pressed(const QModelIndex & )), this, SLOT(album_pressed(const QModelIndex & )));
	connect(this->ui->lv_artist, SIGNAL(pressed(const QModelIndex & )), this, SLOT(artist_pressed(const QModelIndex & )));


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


		QStringList list = md.toStringList();

		this->_track_model->setData(idx, list, Qt::EditRole);
	}


}


void GUI_Library_windowed::fill_library_albums(vector<Album>& albums){

	_v_albums.clear();
	_v_albums = albums;

	this->_album_model->removeRows(0, this->_album_model->rowCount());
	this->_album_model->insertRows(0, albums.size());

	for(uint i=0; i<albums.size(); i++){
		QModelIndex idx = this->_album_model->index(i, 1);

		QString albumname = "<b>" + albums.at(i).name;
		if(albums.at(i).name.isEmpty()) albumname = "<b>Unknown";

		QString year = "";
		if(albums.at(i).year != 0) year = "(" + QString::number(albums.at(i).year) + ")";
		albumname += QString(" ") + year + "</b>\n";


		QString data = albumname;

		data +=  QString::number(albums.at(i).num_songs) + " track";
		if(albums.at(i).num_songs != 1) data += "s";
		data += QString(", ") + getTotalTimeString(albums.at(i));

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

		QString artistname = QString("<b>") + artists.at(i).name + "</b>";
		if(artists.at(i).name.isEmpty()) artistname = "<b>Unknown</b>";

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
		this->ui->tb_title->setColumnWidth(0, width * 0.03);
		this->ui->tb_title->setColumnWidth(1, width * 0.35);
		this->ui->tb_title->setColumnWidth(2, width * 0.23);
		this->ui->tb_title->setColumnWidth(3, width * 0.23);
		this->ui->tb_title->setColumnWidth(4, width * 0.08);
		this->ui->tb_title->setColumnWidth(5, width * 0.08);
		this->ui->tb_title->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);


	}

	else {
		this->ui->tb_title->setColumnWidth(0, width * 0.10);
		this->ui->tb_title->setColumnWidth(1, width * 0.25);
		this->ui->tb_title->setColumnWidth(2, width * 0.25);
		this->ui->tb_title->setColumnWidth(3, width * 0.25);
		this->ui->tb_title->setColumnWidth(4, 80);
		this->ui->tb_title->setColumnWidth(5, 80);
		this->ui->tb_title->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);


	}

	this->ui->lv_album->setColumnWidth(0, 20);
	this->ui->lv_album->setColumnWidth(1, this->ui->lv_album->width() - 20);
	this->ui->lv_artist->setColumnWidth(0, 20);
	this->ui->lv_artist->setColumnWidth(1, this->ui->lv_artist->width() - 20);
}


void GUI_Library_windowed::artist_pressed(const QModelIndex& idx){

	int artist_id = _v_artists.at(idx.row()).id;

	vector<MetaData> vec_tracks;
	vector<Album> vec_albums;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	if(this->ui->le_search->text().length() == 0){
		db->getAllTracksByArtist(artist_id, vec_tracks);
		db->getAllAlbumsByArtist(artist_id, vec_albums);
	}
	else {
		db->getAllTracksByArtist(artist_id, vec_tracks, QString("%") + this->ui->le_search->text() + "%");
		db->getAllAlbumsByArtist(artist_id, vec_albums, QString("%") + this->ui->le_search->text() + "%");
	}




	fill_library_albums(vec_albums);
	fill_library_tracks(vec_tracks);

	QDrag* drag = new QDrag(this);
	QMimeData* mime = new QMimeData();

	QList<QVariant> list2send;

		for(int i=0; i<this->_track_model->rowCount(); i++){
			QStringList metadata = _v_metadata.at(i).toStringList();
			list2send.push_back(metadata);
		}

		mime->setProperty("data_type", DROP_TYPE_TRACKS);
		mime->setProperty("data", (QVariant) list2send);
		drag->setMimeData(mime);

		Qt::DropAction dropAction = drag->exec();
		Q_UNUSED(dropAction);
}

void GUI_Library_windowed::album_pressed(const QModelIndex& idx){

	int album_id = _v_albums.at(idx.row()).id;

	vector<MetaData> vec_tracks;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	if(this->ui->le_search->text().length() == 0){
		db->getAllTracksByAlbum(album_id, vec_tracks);
	}

	else {
		db->getAllTracksByAlbum(album_id, vec_tracks, QString("%") + this->ui->le_search->text() + "%");
	}

	fill_library_tracks(vec_tracks);

	QDrag* drag = new QDrag(this);
	QMimeData* mime = new QMimeData();

	QList<QVariant> list2send;

	for(int i=0; i<this->_track_model->rowCount(); i++){
		QStringList metadata = _v_metadata.at(i).toStringList();
		list2send.push_back(metadata);
	}

	mime->setProperty("data_type", DROP_TYPE_TRACKS);
	mime->setProperty("data", (QVariant) list2send);
	drag->setMimeData(mime);

	Qt::DropAction dropAction = drag->exec();
	Q_UNUSED(dropAction);

}

void GUI_Library_windowed::track_pressed(const QModelIndex& idx){

	Q_UNUSED(idx);

	QDrag* drag = new QDrag(this);
	QMimeData* mime = new QMimeData();

	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows(0);

	QList<QVariant> list2send;

	for(int i=0; i<idx_list.size(); i++){
		int row = idx_list.at(i).row();

		qDebug() << "Path? " << _v_metadata.at(row).filepath;
		QStringList metadata = _v_metadata.at(row).toStringList();
		list2send.push_back(metadata);
	}

	mime->setProperty("data_type", DROP_TYPE_TRACKS);
	mime->setProperty("data", (QVariant) list2send);
	drag->setMimeData(mime);

	Qt::DropAction dropAction = drag->exec();
	Q_UNUSED(dropAction);

}



void GUI_Library_windowed::album_chosen(const QModelIndex & idx){

	int album_id = _v_albums.at(idx.row()).id;

	vector<MetaData> vec;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	db->getAllTracksByAlbum(album_id, vec);
	emit album_chosen_signal(vec);
}

void GUI_Library_windowed::artist_chosen(const QModelIndex & idx){

	int artist_id = _v_artists.at(idx.row()).id;

	vector<MetaData> vec;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	db->getAllTracksByArtist(artist_id, vec);
	emit artist_chosen_signal(vec);

}

void GUI_Library_windowed::track_chosen(const QModelIndex& idx){

	vector<MetaData> vec;
	vec.push_back(_v_metadata.at(idx.row()));
	emit track_chosen_signal(vec);
}



void GUI_Library_windowed::clear_button_pressed(){

	this->ui->le_search->clear();
	text_line_edited(" ");
}

void GUI_Library_windowed::text_line_edited(const QString& search){
Q_UNUSED(search);

	QString searchstring = this->ui->le_search->text();
	if(searchstring.length() == 0){

		vector<Album> vec_albums;
		vector<MetaData> vec_tracks;
		vector<Artist> vec_artists;

		CDatabaseConnector* db = CDatabaseConnector::getInstance();
		db->getTracksFromDatabase(vec_tracks);
		db->getAllAlbums(vec_albums);
		db->getAllArtists(vec_artists);
		fill_library_artists(vec_artists);
		fill_library_albums(vec_albums);
		fill_library_tracks(vec_tracks);

		return;

	}

	vector<MetaData> vec_tracks;
	vector<Album> vec_albums;
	vector<Artist> vec_artists;

	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	db->getAllTracksBySearchString(QString("%") + searchstring + "%", vec_tracks);
	fill_library_tracks(vec_tracks);

	db->getAllAlbumsBySearchString(QString("%") + searchstring + "%", vec_albums);
	fill_library_albums(vec_albums);

	db->getAllArtistsBySearchString(QString("%") + searchstring + "%", vec_artists);
	fill_library_artists(vec_artists);


}


void GUI_Library_windowed::change_skin(bool dark){



	if(dark){
		this->ui->lv_album->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0,0,0);");
		this->ui->lv_artist->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0,0,0);");
		this->ui->tb_title->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0,0,0);");
	}

	else {
		this->ui->lv_album->setStyleSheet("");
		this->ui->lv_artist->setStyleSheet("");
		this->ui->tb_title->setStyleSheet("");
	}
}


QString GUI_Library_windowed::getTotalTimeString(Album& album){
	int secs, mins, hrs;
	Helper::cvtSecs2MinAndSecs(album.length_sec, &mins, &secs);
	hrs = mins / 60;
	mins = mins % 60;

	QString str = "";

	if(hrs > 0) str += QString::number(hrs) + "h ";

	str += 	Helper::cvtNum2String(mins) +
			"m " +
			Helper::cvtNum2String(secs) +
			"s";



	return str;


}
