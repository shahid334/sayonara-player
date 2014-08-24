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
#include "HelperStructs/Style.h"
#include "HelperStructs/MetaDataInfo.h"

#include <QWidget>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QScrollBar>
#include <QCloseEvent>
#include <QPainter>
#include <QDateTime>



GUI_InfoDialog::GUI_InfoDialog(QWidget* parent, GUI_TagEdit* tag_edit) : QDialog(parent) {

    this->ui = new Ui::InfoDialog();
    this->ui->setupUi(this);

    _db = CDatabaseConnector::getInstance();

    _class_name = QString("InfoDialog");

    _initialized = false;

    ui_tag_edit = tag_edit;

	if(ui_tag_edit){
		ui->tab_widget->addTab(ui_tag_edit, tr("Edit"));
	}

    _lfm_thread = new LFMTrackChangedThread(_class_name);
    _lfm_thread->setUsername(CSettingsStorage::getInstance()->getLastFMNameAndPW().first);

    _initialized = true;

    _lyric_thread = new LyricLookupThread();
    _lyric_server = 0;
    _lyrics_visible = true;

	_cover_lookup = new CoverLookup(this);
    _alternate_covers = new GUI_Alternate_Covers(this, _class_name );

    _tag_edit_visible = true;

    QStringList server_list = _lyric_thread->getServers();
    foreach(QString server, server_list) {

        ui->combo_servers->addItem(server);
    }

    ui->combo_servers->setCurrentIndex(0);

	connect( _lfm_thread,	SIGNAL(sig_corrected_data_available(const QString&)),
			 this,			SLOT(psl_corrected_data_available(const QString&)));


	connect(_cover_lookup,	SIGNAL(sig_cover_found(const QString&)),
			this, 			SLOT(psl_cover_available(const QString&)));

	connect(_cover_lookup,	SIGNAL(sig_finished(bool)),
			this, 			SLOT(psl_cover_lookup_finished(bool)));

	connect(_alternate_covers,	SIGNAL(sig_cover_changed(bool)),
			this,				SLOT(psl_alternate_cover_available(bool)));

    connect(_lyric_thread, SIGNAL(finished()), this, SLOT(psl_lyrics_available()));
    connect(_lyric_thread, SIGNAL(terminated()), this, SLOT(psl_lyrics_available()));
	connect(ui->tab_widget, SIGNAL(currentChanged(int)), this, SLOT(psl_tab_index_changed(int)));

    if(ui_tag_edit) {
        connect(ui_tag_edit, SIGNAL(sig_cancelled()), this, SLOT(close()));
        connect(ui_tag_edit, SIGNAL(sig_success(bool)), this, SLOT(psl_id3_success(bool)));
    }

    connect(ui->combo_servers, 	SIGNAL(currentIndexChanged(int)),
            this, 					SLOT(psl_lyrics_server_changed(int)));

    connect(ui->btn_image, SIGNAL(clicked()), this, SLOT(cover_clicked()));

    this->ui->btn_image->setStyleSheet("QPushButton:hover {background-color: transparent;}");

    ui->tab_widget->removeTab(2);
    hide();

}

GUI_InfoDialog::~GUI_InfoDialog() {

}

void GUI_InfoDialog::changeSkin(bool dark) {

    _dark = dark;
}

void GUI_InfoDialog::language_changed() {

    MetaDataList v_md = _v_md;
    ui->tab_widget->removeTab(2);
    this->ui->retranslateUi(this);
    ui->tab_widget->addTab(ui_tag_edit, tr("Edit"));
    setMetaData(v_md);

    _alternate_covers->language_changed();

}


void GUI_InfoDialog::psl_lyrics_server_changed(int idx) {
	_lyric_server = idx;
	_lyric_thread->prepare_thread(_lyric_server);
	_lyric_thread->start();
}


void GUI_InfoDialog::prepare_lyrics() {

	if(_v_md.size() == 1){
		_lyric_thread->set_artist_and_title(_v_md[0].artist, _v_md[0].title);
		ui->tab_widget->setTabEnabled(1, true);
	}

	else {
		_lyric_thread->set_artist_and_title(0, 0);
		ui->tab_widget->setTabEnabled(1, false);
	}

	_lyric_thread->prepare_thread(_lyric_server);
	_lyric_thread->start();
}


void GUI_InfoDialog::psl_lyrics_available() {

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

	if(!_lyrics_visible){
		this->ui->tab_widget->setTabEnabled(TAB_LYRICS, true);
	}

	_lyrics_visible = true;
}


void GUI_InfoDialog::prepare_info() {


	MetaDataInfo* info = 0;

	switch (_mode){
		case InfoDialogMode_Artists:
			info = new ArtistInfo(0, _v_md);
			break;
		case InfoDialogMode_Albums:
			info = new AlbumInfo(0, _v_md);
			break;
		default:
			info = new MetaDataInfo(0, _v_md);
			break;
	}

	this->ui->lab_heading->setText(info->get_header());
	this->ui->lab_info->setText(info->get_info_as_string());
	this->ui->lab_subheader->setText(info->get_subheader());
	this->ui->lab_paths->setOpenExternalLinks(true);
	this->ui->lab_paths->setText(info->get_paths_as_string());

	_cl = info->get_cover_location();
	prepare_cover(_cl);
	prepare_lyrics();

	if(_cl.valid){
		_cover_artist = info->get_cover_artist();
		_cover_album = info->get_cover_album();
	}

	else {
		_cover_artist.clear();
		_cover_album.clear();
	}

	delete info;
}

void GUI_InfoDialog::psl_cover_available(const QString& cover_path) {

	this->ui->btn_image->setIcon(QIcon(cover_path));
	this->ui->btn_image->update();

	qDebug() << "Cover available: " << !this->isHidden();
}

void GUI_InfoDialog::psl_alternate_cover_available(bool b){

	if(!b) {
		QString sayonara_logo = Helper::getIconPath() + "logo.png";
		this->ui->btn_image->setIcon(QIcon(sayonara_logo));
	}

	else{
		CoverLocation cl = _alternate_covers->get_target_filename();
		psl_cover_available(cl.cover_path);
	}
}

void GUI_InfoDialog::psl_cover_lookup_finished(bool b) {

	if(!b) {
		QString sayonara_logo = Helper::getIconPath() + "logo.png";
		this->ui->btn_image->setIcon(QIcon(sayonara_logo));
	}

	// if successful we already got a cover
}

void GUI_InfoDialog::prepare_cover(const CoverLocation& cover_location) {

	_cover_lookup->set_big(true);
	_cover_lookup->fetch_cover(cover_location);
}


void GUI_InfoDialog::setInfoMode(InfoDialogMode mode){
	_mode = mode;
}

void GUI_InfoDialog::setMetaData(const MetaDataList& v_md) {
	if(ui_tag_edit){
		ui_tag_edit->change_meta_data(v_md);
	}

	_v_md = v_md;
	prepare_info();
}


void GUI_InfoDialog::psl_tab_index_changed(int tab){

	ui->ui_info_widget->hide();
	ui->ui_lyric_widget->hide();
	if(ui_tag_edit){
		ui_tag_edit->hide();
	}

	switch(tab){

		case TAB_EDIT:
			ui->tab_widget->setCurrentWidget(ui_tag_edit);
			ui_tag_edit->show();
			break;
		case TAB_LYRICS:
			ui->tab_widget->setCurrentWidget(ui->ui_lyric_widget);
			ui->ui_lyric_widget->show();
			break;
		default:
			ui->tab_widget->setCurrentWidget(ui->ui_info_widget);
			ui->ui_info_widget->show();

			break;
	}

	psl_cover_available(_cl.cover_path);


}

void GUI_InfoDialog::show(int tab) {

	QWidget::show();

	if(tab > 2 || tab < 0) tab = TAB_INFO;

    ui->tab_widget->setTabEnabled(2, _tag_edit_visible);

	if(!_tag_edit_visible && tab == TAB_EDIT) {
        tab = TAB_INFO;
    }

	if(!ui_tag_edit && tab == TAB_EDIT){
		tab = TAB_INFO;
	}

	if(!_lyrics_visible){
		ui->tab_widget->setTabEnabled(TAB_LYRICS, false);
	}

	psl_tab_index_changed(tab);
}

void GUI_InfoDialog::psl_id3_success(bool b) {
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


void GUI_InfoDialog::cover_clicked() {

	this->setFocus();

	if(_cover_artist.size() > 0 && _cover_album.size() > 0){
		_alternate_covers->start(_cover_album, _cover_artist);
	}

	else if(_cover_artist.size() > 0){
		_alternate_covers->start(_cover_artist);
	}

	else if(_cover_album.size() > 0){
		_alternate_covers->start(_cover_album, "Various artists");
	}
}

void GUI_InfoDialog::no_cover_available() {
    this->ui->btn_image->setIcon(QIcon(Helper::getIconPath() + "/logo.png"));
}

void GUI_InfoDialog::init() {
}

void GUI_InfoDialog::set_tag_edit_visible(bool b) {
    _tag_edit_visible = b;
}

void GUI_InfoDialog::closeEvent(QCloseEvent* e) {
    _tag_edit_visible = true;
    e->accept();
}
