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
#include "LyricLookup/LyricLookup.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/MetaDataInfo.h"

#include <QPixmap>
#include <QMessageBox>
#include <QScrollBar>
#include <QCloseEvent>
#include <QPainter>
#include <QDateTime>


GUI_InfoDialog::GUI_InfoDialog(QWidget* parent, GUI_TagEdit* tag_edit) :
	SayonaraDialog(parent),
	Ui::InfoDialog() {

	setupUi(this);

    _db = CDatabaseConnector::getInstance();

    _class_name = QString("InfoDialog");

    _initialized = false;

    ui_tag_edit = tag_edit;

    tab_widget->addTab(ui_tag_edit, tr("Edit"));

    _lfm_thread = new LFMTrackChangedThread(_class_name);

	QStringList user_pw;
	user_pw = _settings->get(Set::LFM_Login);

	if(user_pw.size() > 1){
		_lfm_thread->setUsername(user_pw.first());
	}

    _initialized = true;

    _lyric_thread = new LyricLookupThread();
    _lyric_server = 0;
    _lyrics_visible = true;

	_cover_lookup = new CoverLookup(this);
    _alternate_covers = new GUI_Alternate_Covers(this, _class_name );

    _tag_edit_visible = true;

    QStringList server_list = _lyric_thread->getServers();
    foreach(QString server, server_list) {

		combo_servers->addItem(server);
    }

	combo_servers->setCurrentIndex(0);


	connect(_cover_lookup,	SIGNAL(sig_cover_found(const CoverLocation&)),
			this, 			SLOT(psl_cover_available(const CoverLocation&)));

	connect(_alternate_covers,	SIGNAL(sig_cover_changed(const CoverLocation&)),
			this,				SLOT(psl_cover_available(const CoverLocation&)));

    connect(_lyric_thread, SIGNAL(finished()), this, SLOT(psl_lyrics_available()));
    connect(_lyric_thread, SIGNAL(terminated()), this, SLOT(psl_lyrics_available()));
    connect(tab_widget, SIGNAL(currentChanged(int)), this, SLOT(psl_tab_index_changed(int)));

    connect(ui_tag_edit, SIGNAL(sig_cancelled()), this, SLOT(close()));

	connect(combo_servers, 	SIGNAL(currentIndexChanged(int)),
            this, 					SLOT(psl_lyrics_server_changed(int)));

	connect(btn_image, SIGNAL(clicked()), this, SLOT(cover_clicked()));
    connect(ui_tag_edit, SIGNAL(destroyed()), this, SLOT(psl_tag_edit_deleted()));

	btn_image->setStyleSheet("QPushButton:hover {background-color: transparent;}");

    tab_widget->setCurrentIndex(TAB_INFO);

    hide();
}

GUI_InfoDialog::~GUI_InfoDialog() {

}


void GUI_InfoDialog::psl_tag_edit_deleted(){
    qDebug() << "Tag edit deleted";
}

void GUI_InfoDialog::changeSkin(bool dark) {

    _dark = dark;
}

void GUI_InfoDialog::language_changed() {

    MetaDataList v_md = _v_md;

	retranslateUi(this);

	set_metadata(v_md);
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
		tab_widget->setTabEnabled(1, true);
	}

	else {
		_lyric_thread->set_artist_and_title(0, 0);
		tab_widget->setTabEnabled(1, false);
	}

	_lyric_thread->prepare_thread(_lyric_server);
	_lyric_thread->start();
}


void GUI_InfoDialog::psl_lyrics_available() {

	QString lyrics = _lyric_thread->getFinalLyrics();
	lyrics = lyrics.trimmed();

	int height, width;
	height = te_lyrics->height();
	width = tab_2->size().width();
	te_lyrics->resize(width, height);
	te_lyrics->setAcceptRichText(true);
	te_lyrics->setText(lyrics);
	te_lyrics->setLineWrapColumnOrWidth(te_lyrics->width());
	te_lyrics->setLineWrapMode(QTextEdit::FixedPixelWidth);
	te_lyrics->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	te_lyrics->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	if(!_lyrics_visible){
		tab_widget->setTabEnabled(TAB_LYRICS, true);
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

	lab_heading->setText(info->get_header());
	lab_info->setText(info->get_info_as_string());
	lab_subheader->setText(info->get_subheader());
	lab_paths->setOpenExternalLinks(true);
	lab_paths->setText(info->get_paths_as_string());

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

void GUI_InfoDialog::psl_cover_available(const CoverLocation& cl) {

    QIcon icon(cl.cover_path);
    if(icon.isNull()) return;

    btn_image->setIcon(icon);
	btn_image->update();

    if(sender() == _alternate_covers)
        emit sig_cover_changed(cl);

}




void GUI_InfoDialog::setInfoMode(InfoDialogMode mode){
	_mode = mode;
}

void GUI_InfoDialog::set_metadata(const MetaDataList& v_md) {
	_v_md = v_md;
	prepare_info();
}


void GUI_InfoDialog::psl_tab_index_changed(int tab){

	ui_info_widget->hide();
	ui_lyric_widget->hide();
    ui_tag_edit->hide();

	switch(tab){

		case TAB_EDIT:
			tab_widget->setCurrentWidget(ui_tag_edit);
			ui_tag_edit->show();
			break;

        case TAB_LYRICS:
			tab_widget->setCurrentWidget(ui_lyric_widget);
			ui_lyric_widget->show();
			break;

        default:
			tab_widget->setCurrentWidget(ui_info_widget);
			ui_info_widget->show();

			break;
	}

	psl_cover_available(_cl);
}

void GUI_InfoDialog::show(int tab) {

	QWidget::show();

	if(tab > 2 || tab < 0) tab = TAB_INFO;

	tab_widget->setTabEnabled(2, _tag_edit_visible);

	if(!_tag_edit_visible && tab == TAB_EDIT) {
        tab = TAB_INFO;
    }

	if(!_lyrics_visible){
		tab_widget->setTabEnabled(TAB_LYRICS, false);
	}

	tab_widget->setCurrentIndex(tab);
    psl_tab_index_changed(tab);
}


void GUI_InfoDialog::prepare_cover(const CoverLocation& cover_location) {

	if(!cover_location.valid) return;

	_cover_lookup->fetch_cover(cover_location);
}


void GUI_InfoDialog::cover_clicked() {

	setFocus();

	if(_cover_artist.size() > 0 && _cover_album.size() > 0){
		_alternate_covers->start(_cover_album, _cover_artist, _cl);
	}

	else if(_cover_artist.size() > 0){
		_alternate_covers->start(_cover_artist, _cl);
	}

	else if(_cover_album.size() > 0){
		_alternate_covers->start(_cover_album, "Various artists", _cl);
	}
}

void GUI_InfoDialog::no_cover_available() {
    btn_image->setIcon(Helper::getIcon("logo.png"));
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
