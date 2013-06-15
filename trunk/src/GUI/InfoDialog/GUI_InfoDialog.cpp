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
#include "GUI/alternate_covers/GUI_Alternate_Covers.h"
#include "StreamPlugins/LastFM/LFMTrackChangedThread.h"
#include "CoverLookup/CoverFetchThread.h"
#include "LyricLookup/LyricLookup.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"

#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QImage>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QScrollBar>
#include <QCloseEvent>
#include <QPainter>
#include <QDateTime>



#define INFO_MODE_SINGLE 0
#define INFO_MODE_MULTI 1


GUI_InfoDialog::GUI_InfoDialog(QWidget* parent, GUI_TagEdit* tag_edit) : QDialog(parent){

    this->ui = new Ui::InfoDialog();
    this->ui->setupUi(this);

    _db = CDatabaseConnector::getInstance();

    _mode = INFO_MODE_TRACKS;
    _diff_mode = INFO_MODE_SINGLE;
    _class_name = QString("InfoDialog");

    _initialized = false;

    ui_tag_edit = tag_edit;

    if(ui_tag_edit)
        ui->tab_widget->addTab(ui_tag_edit, tr("Edit"));


    _lfm_thread = new LFMTrackChangedThread(_class_name);
    _lfm_thread->setUsername(CSettingsStorage::getInstance()->getLastFMNameAndPW().first);

    _initialized = true;


    _lyric_thread = new LyricLookupThread();
    _lyric_server = 0;
    _lyrics_visible = true;

    _cover_lookup = new CoverLookup();
    _alternate_covers = new GUI_Alternate_Covers(this, _class_name );

    _tag_edit_visible = true;

    QStringList server_list = _lyric_thread->getServers();
    foreach(QString server, server_list){

        ui->combo_servers->addItem(server);
    }

    ui->combo_servers->setCurrentIndex(0);

    //this->setModal(true);


    connect( _lfm_thread, SIGNAL(sig_corrected_data_available(const QString&)),
             this, SLOT(psl_corrected_data_available(const QString&)));

    connect( _lfm_thread, SIGNAL(sig_album_info_available(const QString&)),
             this, SLOT(psl_album_info_available(const QString&)));

    connect( _lfm_thread, SIGNAL(sig_artist_info_available(const QString&)),
             this, SLOT(psl_artist_info_available(const QString&)));


    connect(_cover_lookup, SIGNAL(sig_covers_found(const QStringList&, QString)),
            this, 			SLOT(psl_cover_available(const QStringList&, QString)));

    connect(_lyric_thread, SIGNAL(finished()), this, SLOT(psl_lyrics_available()));
    connect(_lyric_thread, SIGNAL(terminated()), this, SLOT(psl_lyrics_available()));

    if(ui_tag_edit){
        connect(ui_tag_edit, SIGNAL(sig_cancelled()), this, SLOT(close()));
        connect(ui_tag_edit, SIGNAL(sig_success(bool)), this, SLOT(psl_id3_success(bool)));
    }

    connect(ui->combo_servers, 	SIGNAL(currentIndexChanged(int)),
            this, 					SLOT(psl_lyrics_server_changed(int)));

    connect(ui->btn_image, SIGNAL(clicked()), this, SLOT(cover_clicked()));
    connect(_alternate_covers, SIGNAL(sig_covers_changed(QString, QString)), this, SLOT(alternate_covers_available(QString, QString)));

    connect(_alternate_covers, SIGNAL(sig_no_cover()),
            this,				SLOT(no_cover_available()));

    this->ui->btn_image->setStyleSheet("QPushButton:hover {background-color: transparent;}");

    ui->tab_widget->removeTab(2);
    hide();

}

GUI_InfoDialog::~GUI_InfoDialog() {
	// TODO Auto-generated destructor stub
}

void GUI_InfoDialog::changeSkin(bool dark){

    _dark = dark;
}

void GUI_InfoDialog::language_changed(){

    MetaDataList v_md = _v_md;
    ui->tab_widget->removeTab(2);
    this->ui->retranslateUi(this);
    ui->tab_widget->addTab(ui_tag_edit, tr("Edit"));
    setMode(_mode);
    setMetaData(v_md);

    _alternate_covers->language_changed();

}




void GUI_InfoDialog::psl_lyrics_server_changed(int idx){
	_lyric_server = idx;
	prepare_lyrics();
}


void GUI_InfoDialog::prepare_lyrics(){

	_lyric_thread->prepare_thread(_artist_name, _title, _lyric_server);
	_lyric_thread->start();
}



void GUI_InfoDialog::psl_lyrics_available(){


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
    text = BOLD(tr("Loved: ")) + (loved ? tr("yes") : tr("no"));
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
    qint64 filesize = 0;

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

		filepath = QDir(md.filepath.left(last_sep)).absolutePath();
		if( !pathlist.contains(filepath) )
			pathlist << filepath;

        filesize += md.filesize;
	}

	n_artists = map_artists.keys().size();
	if(n_artists == 1){


		tooltip = "";
		_diff_mode = INFO_MODE_SINGLE;
		int artist_id =map_artists.keys().at(0);
        Artist artist;
        _db->getArtistByID(artist_id, artist);
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
            Artist artist;
            _db->getArtistByID(artist_id, artist);

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


	info = BOLD(tr("#Albums") + ":&nbsp;") + QString::number(n_albums) + CAR_RET;
	info += BOLD(tr("#Tracks") + ":&nbsp;") +  QString::number(n_songs) + CAR_RET;
	info += BOLD(tr("Playing time") + ":&nbsp;") + Helper::cvtMsecs2TitleLengthString(time_msec) + CAR_RET;
	if(n_artists > 1)
		info += BOLD(tr("#Artists") + ":&nbsp;") + QString::number(n_artists) + CAR_RET;
    info+= BOLD(tr("Filesize") + ":&nbsp;") + Helper::calc_filesize_str(filesize) + CAR_RET;

	paths = BOLD(tr("LIBRARY") + " = ") + Helper::createLink(library_path, library_path, false) + CAR_RET + CAR_RET;

	foreach(QString path, pathlist){

        QString tmppath = QString("file://" + path);
		//path.replace(library_path, BOLD("${ML}"));
        if(library_path.size() > 0)
            path.replace(library_path, ".");

		path = Helper::createLink(path, tmppath, false);
		paths += (path + CAR_RET);
	}

    this->ui->lab_heading->setText(Helper::split_string_to_widget(header, ui->lab_heading));
	this->ui->lab_heading->setToolTip(tooltip);
	this->ui->lab_info->setText(info);
	this->ui->lab_paths->setOpenExternalLinks(true);
	this->ui->lab_paths->setText(paths);
	this->ui->lab_playcount->setText("");
}

void GUI_InfoDialog::prepare_albums(){

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
    qint64 filesize = 0;

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

		filepath = filepath = QDir(md.filepath.left(last_sep)).absolutePath();
		if( !pathlist.contains(filepath) )
			pathlist << filepath;

        filesize += md.filesize;
	}

	n_albums = map_albums.keys().size();
	if(n_albums == 0) return;

	if(n_albums == 1){

		tooltip = "";
		_diff_mode = INFO_MODE_SINGLE;
		int album_id = map_albums.keys().at(0);


        Album album;
        _db->getAlbumByID(album_id, album);
		_album_name = album.name;

        QString artist_name;
        if(album.is_sampler){
                _artist_name = Helper::get_album_major_artist(album.id);
                artist_name = tr("Various artists");
        }

        else{
            _artist_name = album.artists[0];
            artist_name = _artist_name;
        }

		n_songs = album.num_songs;
        header =  Helper::split_string_to_widget(album.name, ui->lab_heading) + " <font size=\"small\">"+ CAR_RET + tr("by ") + artist_name + "</font>";

		info = BOLD(tr("#Tracks") + ":&nbsp;") +  QString::number(album.num_songs) + CAR_RET;
		info += BOLD(tr("Playing time") + ":&nbsp;") + Helper::cvtMsecs2TitleLengthString(album.length_sec * 1000) + CAR_RET;
		if(album.year != 0)
			info += BOLD(tr("Year") + ":&nbsp;") + QString::number(album.year) + CAR_RET;
        info += BOLD(tr("Sampler?") + ":&nbsp;") + ((album.is_sampler) ? tr("yes") : tr("no")) + CAR_RET;
        info += BOLD(tr("Filesize") + ":&nbsp;") + Helper::calc_filesize_str(filesize) + CAR_RET;
	}


	else if(n_albums > 1){
		tooltip = "";
		_diff_mode = INFO_MODE_MULTI;
		int header_entries = 0;
         _artist_name = "";

		foreach(int album_id, map_albums.keys()){
            Album album;
            _db->getAlbumByID(album_id, album);
            QString artist_name = ((album.artists.size() > 1) ? tr("Various artists") : album.artists[0]);

            if(header_entries < 5)
                header += Helper::split_string_to_widget(album.name, ui->lab_heading) + "<font size=\"small\"> " +tr("by") + artist_name + "</font>" + CAR_RET;
			else if(header_entries == 5) header += "...";

			tooltip += album.name + " (" + QString::number(map_albums[album_id]) + ")" + CAR_RET;
			n_songs += album.num_songs;
			header_entries++;
		}

		info = BOLD(tr("#Tracks") + ":&nbsp;") + QString::number(n_songs);
        info += BOLD(tr("Filesize") + ":&nbsp;") + Helper::calc_filesize_str(filesize) + CAR_RET;
	}

	else return;

	paths = BOLD(tr("LIBRARY") + " = ") + Helper::createLink(library_path, library_path, false) + CAR_RET + CAR_RET;

	foreach(QString path, pathlist){
        QString tmppath = QString("file://" + path);
		//path.replace(library_path, BOLD("${ML}"));
        if(library_path.size() > 0)
            path.replace(library_path, ".");
		path = Helper::createLink(path, tmppath, false);
		paths += (path + CAR_RET);
	}


    this->ui->lab_heading->setText(header);
	this->ui->lab_heading->setToolTip(tooltip);
	this->ui->lab_info->setText(info);
	this->ui->lab_paths->setOpenExternalLinks(true);
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
    QString library_path = CSettingsStorage::getInstance()->getLibraryPath();

    qint64 filesize = 0;

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

        if(Helper::is_www(md.filepath)){
            filepath = md.filepath;
        }
        else
            filepath = QDir(md.filepath.left(last_sep)).absolutePath();

		if( !pathlist.contains(filepath) )
			pathlist << filepath;

        filesize += md.filesize;
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
				count = tr("1st");
				break;
			case 2:
				count = tr("2nd");
				break;
			case 3:
				count = tr("3rd");
				break;
			default:
				count = QString::number(md.track_num) + tr("th");
				break;
		}
		info = count + tr(" track on ") + md.album + CAR_RET;
		info+= BOLD(tr("Artist") + ":&nbsp;") + md.artist + CAR_RET;
		info+= BOLD(tr("Length") + ":&nbsp;") + Helper::cvtMsecs2TitleLengthString(md.length_ms) + CAR_RET;
		info+= BOLD(tr("Year") + ":&nbsp;") + QString::number(md.year) + CAR_RET;
        info+= BOLD(tr("Bitrate") + ":&nbsp;") + QString::number(md.bitrate) + CAR_RET;
        info+= BOLD(tr("Genre") + ":&nbsp;") + md.genres.join("<br />") + CAR_RET;
        info+= BOLD(tr("Filesize") + ":&nbsp;") + Helper::calc_filesize_str(filesize) + CAR_RET;
	}

	else if(n_tracks > 1){

		_diff_mode = INFO_MODE_MULTI;

		header = tr("Various tracks");

		info+= BOLD(tr("#Tracks") + ":&nbsp;") + QString::number(_v_md.size()) + CAR_RET;

        if(n_albums > 1){
            info+= BOLD(tr("#Albums") + ":&nbsp;") + QString::number(n_albums) + CAR_RET;
            _album_name = "Various";
        }

        else{
            info+= BOLD(tr("on ") + ":&nbsp;") + _v_md[0].album + CAR_RET;
            _album_name = _v_md[0].album;
        }

        if(n_artists > 1){
            info+= BOLD(tr("#Artists") + ":&nbsp;") + QString::number(n_artists) + CAR_RET;
            _artist_name = "Various";
        }

        else{
            info+= BOLD(tr("by ") + ":&nbsp;") + _v_md[0].artist + CAR_RET;
            _artist_name = _v_md[0].artist;
        }

		info+= BOLD(tr("Length") + ":&nbsp;") + Helper::cvtMsecs2TitleLengthString(time_msec) + CAR_RET;
        info+= BOLD(tr("Filesize") + ":&nbsp;") + Helper::calc_filesize_str(filesize) + CAR_RET;
	}

    paths = BOLD(tr("LIBRARY") + "= ") + Helper::createLink(library_path, library_path, false) + CAR_RET + CAR_RET;

    foreach(QString path, pathlist){

        QString tmppath = QString("file://") + path;
        //path.replace(library_path, BOLD("${ML}"));
        if(library_path.size() > 0)
            path.replace(library_path, ".");
        path = Helper::createLink(path, tmppath, false);
        paths += (path + CAR_RET);
    }

    this->ui->lab_heading->setText(Helper::split_string_to_widget(header, ui->lab_heading));
	this->ui->lab_heading->setToolTip(tooltip);
	this->ui->lab_info->setText(info);
	this->ui->lab_paths->setText(paths);
	this->ui->lab_playcount->setText("");
}


void GUI_InfoDialog::psl_cover_available(const QStringList& cover_paths, QString call_id){

    if(cover_paths.size() == 0) return;
    if(call_id != _call_id) return;
    this->ui->btn_image->setIcon(QIcon(cover_paths[0]));
}

void GUI_InfoDialog::prepare_cover(){

    QStringList cover_paths;
    cover_paths << Helper::getIconPath() + "logo.png";

    _call_id = QString("InfoDialog") + QDateTime::currentDateTimeUtc().toString();
    psl_cover_available(cover_paths, _call_id);

	switch(_diff_mode){

		case INFO_MODE_SINGLE:

			if(_mode == INFO_MODE_ARTISTS){

                int artist_id = _db->getArtistID(_artist_name);
                if(artist_id >= 0)
                    _cover_lookup->fetch_cover_artist(artist_id, _call_id);

                else{
                    Artist artist;
                    artist.name = _artist_name;
                    _cover_lookup->fetch_cover_artist(artist, _call_id);
                }
			}

			else if(_mode == INFO_MODE_ALBUMS || _mode == INFO_MODE_TRACKS){
                Album album;
                album.name = _album_name;
                album.artists << _artist_name;

                if(_album_name.isEmpty() && _artist_name.isEmpty()) return;
                _cover_lookup->fetch_cover_album(album, _call_id);
			}

			break;


		case INFO_MODE_MULTI:
            if(_mode == INFO_MODE_TRACKS){
                Album album;
                album.name = _album_name;
                album.artists << _artist_name;

                if(_album_name.isEmpty() && _artist_name.isEmpty()) return;
                _cover_lookup->fetch_cover_album(album, _call_id);
            }

            break;

        default:
			return;
	}
}

void GUI_InfoDialog::prepare_lfm_info(){

	if(!CSettingsStorage::getInstance()->getLastFMActive()){
        this->ui->lab_playcount->setText(tr("Last.fm not active"));
		return;
	}

	if(_diff_mode != INFO_MODE_SINGLE) {
	    this->ui->lab_playcount->setText("");
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

void GUI_InfoDialog::setMetaData(const MetaDataList& v_md){
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

    ui->tab_widget->setTabEnabled(2, _tag_edit_visible);
    if(!_tag_edit_visible && tab == TAB_EDIT){
        tab = TAB_INFO;
    }

    ui->tab_widget->setCurrentIndex(tab);
}

void GUI_InfoDialog::psl_id3_success(bool b){
    if(b) {
        hide();
        close();
    }
	else{
		QMessageBox::warning ( this,
				tr("Error"),
				tr("ID3 tags could not be changed"));
	}
}


void GUI_InfoDialog::cover_clicked(){

    switch(_diff_mode){

        case INFO_MODE_SINGLE:
            if(_mode == INFO_MODE_ALBUMS || _mode == INFO_MODE_TRACKS){
                qDebug() << "album name = " << _album_name << ", " << _artist_name;
                _alternate_covers->start(_artist_name + " " + _album_name, Helper::get_cover_path(_artist_name, _album_name));
            }

            else if(_mode == INFO_MODE_ARTISTS){

                _alternate_covers->start(_artist_name, Helper::get_artist_image_path(_artist_name));
            }

            break;

        case INFO_MODE_MULTI:
        default:
            return;
    }

	this->setFocus();
}



void GUI_InfoDialog::no_cover_available(){
    //prepare_cover();
    this->ui->btn_image->setIcon(QIcon(Helper::getIconPath() + "/logo.png"));
}



void GUI_InfoDialog::alternate_covers_available(QString caller_class, QString cover_path){

    if(caller_class != _class_name) return;

    bool is_mode_single = (_diff_mode == INFO_MODE_SINGLE);
    bool is_mode_tracks_or_albums = ( _mode == INFO_MODE_ALBUMS || _mode == INFO_MODE_TRACKS);

    if( is_mode_single &&
        is_mode_tracks_or_albums){

        MetaData md;
        md.album = _album_name;
        md.artist = _artist_name;
        Album album = Helper::get_album_from_metadata(md);
        QStringList lst;

        // calc all cover paths for album
        foreach(QString artist, album.artists){
            lst << Helper::get_cover_path(artist, album.name);
        }

        // copy cover
        QString major_artist_cover_path = Helper::get_cover_path(Helper::get_album_major_artist(album.id), album.name);
        lst << major_artist_cover_path;


        if(lst.contains(cover_path)){

            QFile cover_file(cover_path);
            // copy to all cover paths
            foreach(QString path, lst){

                // no need for the cover path
                if(!path.compare(cover_path)) continue;

                if(QFile::exists(path)) QFile::remove(path);
                cover_file.copy(path);
            }
        }
    }

    prepare_cover();
}


void GUI_InfoDialog::init(){
}


void GUI_InfoDialog::set_tag_edit_visible(bool b){
    _tag_edit_visible = b;
}

void GUI_InfoDialog::closeEvent(QCloseEvent* e){
    _tag_edit_visible = true;
    e->accept();

}
