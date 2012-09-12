/* GUI_InfoDialog.cpp

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Jul 19, 2012 
 *
 */

#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include "StreamPlugins/LastFM/LFMTrackChangedThread.h"
#include "CoverLookup/CoverFetchThread.h"
#include "LyricLookup/LyricLookup.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"

#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QImage>
#include <QFile>
#include <QDebug>
#include <QMessageBox>



#define INFO_MODE_SINGLE 0
#define INFO_MODE_MULTI 1

#define CAR_RET QString("<br />")
#define BOLD(x) QString("<b>") + x + QString("</b>")

GUI_InfoDialog::GUI_InfoDialog(QWidget* parent, GUI_TagEdit* tag_edit) : QDialog(parent){
	this->ui = new Ui::InfoDialog();
	this->ui->setupUi(this);

	_mode = INFO_MODE_TRACKS;
	_diff_mode = INFO_MODE_SINGLE;
	_class_name = QString("InfoDialog");
	_lfm_thread = new LFMTrackChangedThread(_class_name);
	_lfm_thread->setUsername(CSettingsStorage::getInstance()->getLastFMNameAndPW().first);

	_lyric_thread = new LyricLookupThread();
	_lyric_server = 0;
	QStringList server_list = _lyric_thread->getServers();
	ui->lmb_server_button->setServers(server_list);
	ui->lmb_server_button->setText(server_list[0]);

	_cover_lookup = new CoverLookup(_class_name);
	_db = CDatabaseConnector::getInstance();

	_lyrics_visible = true;

	ui_tag_edit = tag_edit;

	if(ui_tag_edit)
		ui->tab_widget->addTab(ui_tag_edit, "Edit");

	connect( _lfm_thread, SIGNAL(sig_corrected_data_available(const QString&)),
			 this, SLOT(psl_corrected_data_available(const QString&)));

	connect( _lfm_thread, SIGNAL(sig_album_info_available(const QString&)),
			 this, SLOT(psl_album_info_available(const QString&)));

	connect( _lfm_thread, SIGNAL(sig_artist_info_available(const QString&)),
			 this, SLOT(psl_artist_info_available(const QString&)));

	connect(this, SIGNAL(sig_search_cover(const MetaData&)),
			_cover_lookup, SLOT(search_cover(const MetaData&)));

	connect(this, SIGNAL(sig_search_artist_image(const QString&)),
			_cover_lookup, SLOT(search_artist_image(const QString&)));

	connect(_cover_lookup, SIGNAL(sig_cover_found(QString, QString)),
			this, 			SLOT(psl_image_available(QString, QString)));

	connect(_lyric_thread, SIGNAL(finished()), this, SLOT(psl_lyrics_available()));
	connect(_lyric_thread, SIGNAL(terminated()), this, SLOT(psl_lyrics_available()));
	if(ui_tag_edit){
		connect(ui_tag_edit, SIGNAL(sig_cancelled()), this, SLOT(close()));
		connect(ui_tag_edit, SIGNAL(sig_success(bool)), this, SLOT(psl_id3_success(bool)));
	}

	this->connect(	ui->lmb_server_button, 	SIGNAL(sig_server_changed(int)),
					this, 					SLOT(psl_lyrics_server_changed(int)));




}

GUI_InfoDialog::~GUI_InfoDialog() {
	// TODO Auto-generated destructor stub
}



void GUI_InfoDialog::psl_image_available(QString caller_class, QString filename){
	if(_class_name != caller_class) return;
	if(!QFile::exists(filename)) return;

	QSize sz = this->ui->lab_image->size();

	QPixmap pix = QPixmap::fromImage(QImage(filename)).scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	this->ui->lab_image->setPixmap(pix);
}


void GUI_InfoDialog::psl_lyrics_server_changed(int idx){
	_lyric_server = idx;
	QStringList lst = _lyric_thread->getServers();
	this->ui->lmb_server_button->setText(lst[idx]);
	prepare_lyrics();
}


void GUI_InfoDialog::prepare_lyrics(){

	_lyric_thread->prepare_thread(_artist_name, _title, _lyric_server);
	_lyric_thread->start();
}



void GUI_InfoDialog::psl_lyrics_available(){

	if(!this->ui->lmb_server_button->isChecked()){
		this->ui->lmb_server_button->setVisible(true);
		this->ui->lmb_server_button->setChecked(true);
	}

	QString lyrics = _lyric_thread->getFinalLyrics();
	lyrics = lyrics.trimmed();

	int height, width;
	height = ui->te_lyrics->height();
	width = this->ui->tab_2->size().width();
	ui->te_lyrics->resize(width, height);
	ui->te_lyrics->setAcceptRichText(true);
	ui->te_lyrics->setText(lyrics);
	ui->te_lyrics->setLineWrapColumnOrWidth(this->ui->te_lyrics->width());
	ui->te_lyrics->setLineWrapMode(QTextEdit::FixedPixelWidth);
	ui->te_lyrics->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->te_lyrics->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	if(!_lyrics_visible)
		this->ui->tab_widget->setTabEnabled(TAB_LYRICS, true);

	_lyrics_visible = true;
}


void GUI_InfoDialog::psl_corrected_data_available(const QString& target_class){

	MetaData md;
	bool loved;
	bool corrected;

	_lfm_thread->fetch_corrections(md, loved, corrected);
	QString text;
	text = BOLD("Loved: ") + (loved ? "yes" : "no");
	this->ui->lab_playcount->setText(text);
}

void GUI_InfoDialog::psl_album_info_available(const QString& target_class){

	if(target_class != _class_name) return;

	QMap<QString, QString> map;
	_lfm_thread->fetch_album_info(map);

	QString text;
	foreach(QString key, map.keys()){
		QString val = map[key];
		if(val.trimmed().size() == 0) continue;
		text += BOLD(key) + ": " + val + CAR_RET;
	}

	this->ui->lab_playcount->setText(text);
}

void GUI_InfoDialog::psl_artist_info_available(const QString& target_class){
	if(target_class != _class_name) return;

	QMap<QString, QString> map;
	_lfm_thread->fetch_artist_info(map);

	QString text;
	foreach(QString key, map.keys()){
		QString val = map[key];
		if(val.trimmed().size() == 0) continue;
		text += BOLD(key) + ": " + val + CAR_RET;
	}

	this->ui->lab_playcount->setText(text);
}


void GUI_InfoDialog::prepare_artists(){
	int n_artists = 0;
	int n_albums = 0;
	int n_songs = 0;

	qint64 time_msec = 0;

	QMap<int, int> map_artists;

	QString header = "";
	QString info = "";
	QString paths = "";
	QString tooltip;
	QString library_path = CSettingsStorage::getInstance()->getLibraryPath();

	QStringList pathlist;

	foreach(MetaData md, _v_md){
		int artist_id = md.artist_id;
		QString filepath = "";

		if(!map_artists.keys().contains(artist_id)){
			map_artists[artist_id] = 0;
		}

		else{
			map_artists[artist_id] = map_artists[artist_id] + 1;
		}

		time_msec += md.length_ms;

		int last_sep = md.filepath.lastIndexOf("/");
		if(last_sep == -1) last_sep = md.filepath.lastIndexOf("\\");
		if(last_sep == -1 || last_sep >= md.filepath.size()) continue;

		filepath = md.filepath.left(last_sep);
		if( !pathlist.contains(filepath) )
			pathlist << filepath;
	}

	n_artists = map_artists.keys().size();
	if(n_artists == 1){


		tooltip = "";
		_diff_mode = INFO_MODE_SINGLE;
		int artist_id =map_artists.keys().at(0);
		Artist artist = _db->getArtistByID(artist_id);
		_artist_name = artist.name;

		n_albums = artist.num_albums;
		n_songs = artist.num_songs;

		header = artist.name;

	}

	else if(n_artists > 1){
		tooltip = "";
		_diff_mode = INFO_MODE_MULTI;
		int header_entries = 0;
		foreach(int artist_id, map_artists.keys()){
			Artist artist = _db->getArtistByID(artist_id);

			if(header_entries < 5)
				header += artist.name + CAR_RET;
			else if(header_entries == 5) header += "...";

			tooltip += artist.name + " (" + QString::number(map_artists[artist_id]) + ")" + CAR_RET;
			n_albums += artist.num_albums;
			n_songs += artist.num_songs;
			header_entries++;
		}
	}

	else return;


	info = BOLD("#Albums:&nbsp;") + QString::number(n_albums) + CAR_RET;
	info += BOLD("#Tracks:&nbsp;") +  QString::number(n_songs) + CAR_RET;
	info += BOLD("Playing time:&nbsp;") + Helper::cvtMsecs2TitleLengthString(time_msec) + CAR_RET;
	if(n_artists > 1)
		info += BOLD("#Artists:&nbsp;") + QString::number(n_artists) + CAR_RET;


	foreach(QString path, pathlist){
		path.replace(library_path, QString("<b>${ML}</b>"));
		paths += (path + CAR_RET);
	}

	this->ui->lab_heading->setText(header);
	this->ui->lab_heading->setToolTip(tooltip);
	this->ui->lab_info->setText(info);
	this->ui->lab_paths->setText(paths);
	this->ui->lab_playcount->setText("");
}

void GUI_InfoDialog::prepare_albums(){

	QString year_span = "";
	int n_albums = 0;
	int n_songs = 0;

	qint64 time_msec = 0;

	QMap<int, int> map_albums;

	QString header = "";
	QString info = "";
	QString paths = "";
	QString tooltip;
	QString library_path = CSettingsStorage::getInstance()->getLibraryPath();

	QStringList pathlist;

	foreach(MetaData md, _v_md){
		int album_id = md.album_id;
		QString filepath = "";

		if(!map_albums.keys().contains(album_id)){
			map_albums[album_id] = 0;
		}

		else{
			map_albums[album_id] = map_albums[album_id] + 1;
		}

		time_msec += md.length_ms;

		int last_sep = md.filepath.lastIndexOf("/");
		if(last_sep == -1) last_sep = md.filepath.lastIndexOf("\\");
		if(last_sep == -1 || last_sep >= md.filepath.size()) continue;

		filepath = md.filepath.left(last_sep);
		if( !pathlist.contains(filepath) )
			pathlist << filepath;
	}

	n_albums = map_albums.keys().size();
	if(n_albums == 0) return;

	if(n_albums == 1){

		tooltip = "";
		_diff_mode = INFO_MODE_SINGLE;
		int album_id = map_albums.keys().at(0);


		Album album = _db->getAlbumByID(album_id);
		_album_name = album.name;
		_artist_name = album.artists[0];

		n_songs = album.num_songs;
		header = album.name + " <font size=\"small\"> by " + _artist_name + "</font>";

		info = BOLD("#Tracks:&nbsp;") +  QString::number(album.num_songs) + CAR_RET;
		info += BOLD("Playing time:&nbsp;") + Helper::cvtMsecs2TitleLengthString(album.length_sec * 1000) + CAR_RET;
		if(album.year != 0)
			info += BOLD("Year:&nbsp;") + QString::number(album.year) + CAR_RET;
		info += BOLD("Sampler?:&nbsp;") + ((album.is_sampler) ? "yes" : "no");
	}


	else if(n_albums > 1){
		tooltip = "";
		_diff_mode = INFO_MODE_MULTI;
		int header_entries = 0;

		foreach(int album_id, map_albums.keys()){
			Album album = _db->getAlbumByID(album_id);
			QString artist_name = ((album.artists.size() > 1) ? QString("Various") : album.artists[0]);

			if(header_entries < 5)
				header += album.name + " <font size=\"small\"> by " + artist_name + "</font>" + CAR_RET;
			else if(header_entries == 5) header += "...";

			tooltip += album.name + " (" + QString::number(map_albums[album_id]) + ")" + CAR_RET;
			n_songs += album.num_songs;
			header_entries++;
		}

		info = BOLD("#Tracks:&nbsp;") + QString::number(n_songs);
	}

	else return;


	foreach(QString path, pathlist){
		path.replace(library_path, QString("<b>${ML}</b>"));
		paths += (path + CAR_RET);
	}

	this->ui->lab_heading->setText(header);
	this->ui->lab_heading->setToolTip(tooltip);
	this->ui->lab_info->setText(info);
	this->ui->lab_paths->setText(paths);
	this->ui->lab_playcount->setText("");
}

void GUI_InfoDialog::prepare_tracks(){

	int n_tracks = 0;
	int n_albums = 0;
	int n_artists = 0;
	qint64 time_msec = 0;

	QMap<int, int> map_artists;
	QMap<int, int> map_albums;

	QStringList pathlist;
	QString header, info, paths;
	QString tooltip;

	foreach(MetaData md, _v_md){
		int album_id = md.album_id;
		int artist_id = md.artist_id;
		QString filepath = "";

		header += md.title + CAR_RET;

		if(!map_albums.keys().contains(album_id)){
			map_albums[album_id] = 0;
		}

		else{
			map_albums[album_id] = map_albums[album_id] + 1;
		}

		if(!map_albums.keys().contains(artist_id)){
			map_artists[artist_id] = 0;
		}

		else{
			map_artists[artist_id] = map_artists[artist_id] + 1;
		}

		time_msec += md.length_ms;

		int last_sep = md.filepath.lastIndexOf("/");
		if(last_sep == -1) last_sep = md.filepath.lastIndexOf("\\");
		if(last_sep == -1 || last_sep >= md.filepath.size()) continue;

		filepath = md.filepath.left(last_sep);
		if( !pathlist.contains(filepath) )
			pathlist << filepath;
	}


	n_albums = map_albums.keys().size();
	n_artists = map_artists.keys().size();
	n_tracks = _v_md.size();

	if(n_tracks == 1){
		_diff_mode = INFO_MODE_SINGLE;
		MetaData md = _v_md.at(0);
		header = md.title;

		_album_name = md.album;
		_artist_name = md.artist;
		_title = md.title;


		prepare_lyrics();

		int tracknum = md.track_num;
		QString count;
		switch(tracknum){
			case 1:
				count = "1st";
				break;
			case 2:
				count = "2nd";
				break;
			case 3:
				count = "3rd";
				break;
			default:
				count = QString::number(md.track_num) + "th";
				break;
		}
		info = count + " track on " + md.album + CAR_RET;
		info+= BOLD("Artist:&nbsp;") + md.artist + CAR_RET;
		info+= BOLD("Length:&nbsp;") + Helper::cvtMsecs2TitleLengthString(md.length_ms) + CAR_RET;
		info+= BOLD("Year:&nbsp;") + QString::number(md.year) + CAR_RET;
		info+= BOLD("Bitrate:&nbsp;") + QString::number(md.bitrate);
	}

	else if(n_tracks > 1){

		_diff_mode = INFO_MODE_MULTI;
		header = "Various tracks";
		info+= BOLD("#Tracks:&nbsp;") + QString::number(_v_md.size()) + CAR_RET;
		info+= BOLD("#Albums:&nbsp;") + QString::number(n_albums) + CAR_RET;
		info+= BOLD("#Artists:&nbsp;") + QString::number(n_artists) + CAR_RET;
		info+= BOLD("Length:&nbsp;") + Helper::cvtMsecs2TitleLengthString(time_msec) + CAR_RET;
	}

	foreach(QString path, pathlist){
		paths += path + CAR_RET;
	}


	this->ui->lab_heading->setText(header);
	this->ui->lab_heading->setToolTip(tooltip);
	this->ui->lab_info->setText(info);
	this->ui->lab_paths->setText(paths);
	this->ui->lab_playcount->setText("");
}




void GUI_InfoDialog::prepare_cover(){


	psl_image_available(_class_name, Helper::getIconPath() + "append.png");

	switch(_diff_mode){

		case INFO_MODE_SINGLE:

			if(_mode == INFO_MODE_ARTISTS){
				emit sig_search_artist_image(_artist_name);
			}

			else if(_mode == INFO_MODE_ALBUMS || _mode == INFO_MODE_TRACKS){
				MetaData md;
				md.album = _album_name;
				md.artist = _artist_name;
				emit sig_search_cover(md);
			}

			break;

		case INFO_MODE_MULTI:
		default:
			return;
	}
}

void GUI_InfoDialog::prepare_lfm_info(){

	if(_diff_mode != INFO_MODE_SINGLE) {
	    this->ui->lab_playcount->setText("");
	    return;
	}

	switch(_mode){
		case INFO_MODE_ALBUMS:

			_lfm_thread->setArtistName(_artist_name);
			_lfm_thread->setAlbumName(_album_name);
			_lfm_thread->setThreadTask(LFM_THREAD_TASK_FETCH_ALBUM_INFO);
			_lfm_thread->start();
			break;

		case INFO_MODE_ARTISTS:
			_lfm_thread->setArtistName(_artist_name);
			_lfm_thread->setThreadTask(LFM_THREAD_TASK_FETCH_ARTIST_INFO);
			_lfm_thread->start();
			break;

		case INFO_MODE_TRACKS:
			if(_v_md.size() == 0) break;
			_lfm_thread->setTrackInfo(_v_md[0]);
			_lfm_thread->setThreadTask(LFM_THREAD_TASK_FETCH_TRACK_INFO);
			_lfm_thread->start();
			break;
		default: break;
	}

}

void GUI_InfoDialog::setMetaData(const vector<MetaData>& v_md){
	if(ui_tag_edit)
		ui_tag_edit->change_meta_data(v_md);

	_v_md = v_md;
}

void GUI_InfoDialog::setMode(int mode){
	_mode = mode;

	ui->tab_widget->setCurrentIndex(TAB_INFO);

	if(_lyrics_visible)
		this->ui->tab_widget->setTabEnabled(TAB_LYRICS, false);
	_lyrics_visible = false;

	if(ui_tag_edit)
		ui_tag_edit->hide();

	switch(_mode){
		case INFO_MODE_TRACKS:
			prepare_tracks();
			break;

		case INFO_MODE_ARTISTS:
			prepare_artists();
			break;

		case INFO_MODE_ALBUMS:
			prepare_albums();
			break;

		default: break;

	}

	prepare_cover();
	prepare_lfm_info();
}

void GUI_InfoDialog::show(int tab){
	QWidget::show();
	if(tab > 2 || tab < 0) tab = TAB_INFO;
	ui->tab_widget->setCurrentIndex(tab);
}

void GUI_InfoDialog::psl_id3_success(bool b){
	if(b) close();
	else{
		QMessageBox::warning ( this,
				"Error",
				"ID3 tags could not be changed");
	}
}
