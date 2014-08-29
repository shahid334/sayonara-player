/* GUI_Podcasts.cpp */

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



#include "GUI/Podcasts/GUI_Podcasts.h"
#include "GUI/ui_GUI_Podcasts.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/PodcastParser/PodcastParser.h"

#include <QIcon>
#include <QPixmap>
#include <QMap>
#include <QInputDialog>
#include <QMessageBox>


GUI_Podcasts::GUI_Podcasts(QString name, QWidget *parent) :
	PlayerPlugin(name, parent),
	Ui::GUI_Podcasts()
{

	setupUi(this);

    init_gui();

    _cur_podcast = -1;

    QMap<QString, QString> data;
    CDatabaseConnector::getInstance()->getAllPodcasts(data);
	if(data.size() > 0){
		setup_podcasts(data);
	}

    btn_listen->setIcon(Helper::getIcon("play.png"));

	connect(btn_listen, SIGNAL(clicked()), this, SLOT(listen_clicked()));
	connect(btn_save, SIGNAL(clicked()), this, SLOT(save_clicked()));
	connect(btn_delete, SIGNAL(clicked()), this, SLOT(delete_clicked()));
	connect(combo_podcasts, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_index_changed(int)));
	connect(combo_podcasts, SIGNAL(editTextChanged(const QString&)), this, SLOT(combo_text_changed(const QString&)));

	connect(le_url, SIGNAL(textEdited(const QString&)), this, SLOT(url_text_changed(const QString&)));

    hide();
}


GUI_Podcasts::~GUI_Podcasts() {

}

void GUI_Podcasts::language_changed() {
	retranslateUi(this);
}


void GUI_Podcasts::listen_clicked() {

    QString url;
    QString name;

    if(_cur_podcast == -1) {
		url = le_url->text();
        name = "Podcast";
    }

    else{
        qDebug() << "Emit: " << _cur_podcast_name << ": " << _cur_podcast_adress;
        url = _cur_podcast_adress;
        name = _cur_podcast_name;
    }

    if(url.size() > 5) {

       play_podcasts(url, name);
    }
}


void GUI_Podcasts::setup_podcasts(const QMap<QString, QString>& podcasts) {

    _podcasts = podcasts;
    if(podcasts.size() > 0) {
        _cur_podcast = -1;
    }

	combo_podcasts->clear();

    _cur_podcast_adress = "";
    _cur_podcast_name = "";
    _cur_podcast = 0;

    _podcasts[""] = "";

    for(QMap<QString, QString>::iterator it = _podcasts.begin(); it != _podcasts.end(); it++) {
		combo_podcasts->addItem(it.key(), it.value());
    }

	btn_listen->setEnabled(false);
	btn_save->setEnabled(false);
	btn_delete->setEnabled(false);
}


void GUI_Podcasts::init_gui() {
    btn_delete->setIcon(Helper::getIcon("delete.png"));
    btn_save->setIcon(Helper::getIcon("save.png"));
    lab_icon->setPixmap(Helper::getPixmap("podcast.png", lab_icon->size(), false));
}


void GUI_Podcasts::combo_index_changed(int idx) {

    _cur_podcast = idx;
	_cur_podcast_name = combo_podcasts->itemText(_cur_podcast);

    QString adress = _podcasts[_cur_podcast_name];
    if(adress.size() > 0) {
        _cur_podcast_adress = adress;
		le_url->setText(_cur_podcast_adress);
    }

    if(idx == 0) {
		le_url->setText("");
    }


	btn_delete->setEnabled(idx > 0);
	btn_save->setEnabled(false);
	btn_listen->setEnabled(le_url->text().size() > 5);
	combo_podcasts->setToolTip(_cur_podcast_adress);
}


void GUI_Podcasts::combo_text_changed(const QString& text) {
    _cur_podcast = -1;


    bool name_there = false;
	for(int i=0; i<combo_podcasts->count(); i++) {
		QString str = combo_podcasts->itemText(i);
        if(!str.compare(text, Qt::CaseSensitive)) {
            name_there = true;
            break;
        }
    }


	btn_delete->setEnabled(name_there);
	btn_save->setEnabled(!name_there && (text.size() > 0));
	btn_listen->setEnabled(le_url->text().size() > 5);
	combo_podcasts->setToolTip("");
}

void GUI_Podcasts::url_text_changed(const QString& text) {

    QString key = _podcasts.key(text);

    if(! key.isEmpty() ) {

		int idx = combo_podcasts->findText(key, Qt::MatchCaseSensitive);
        if(idx != -1) {
			combo_podcasts->setCurrentIndex(idx);
            _cur_podcast = idx;
			btn_save->setEnabled(false);
			btn_delete->setEnabled(true);
        }
    }

    // new adress
    else{

		btn_delete->setEnabled(false);

        bool save_enabled =
				combo_podcasts->currentText().size() > 0 &&
				le_url->text().size() > 5 &&
                _cur_podcast == -1;

		btn_save->setEnabled(save_enabled);
		btn_listen->setEnabled(text.size() > 5);
        if(_cur_podcast != -1) {
            _cur_podcast = -1;
			combo_podcasts->setEditText(tr("new"));
            _cur_podcast = -1;
        }
    }

}


void GUI_Podcasts::delete_clicked() {
    if(_cur_podcast == -1) return;

    CDatabaseConnector* db = CDatabaseConnector::getInstance();
    QMessageBox msgBox(this);
    QString ask = tr("Really wanna delete %1?").arg(_cur_podcast_name);
    msgBox.setText(ask );
    msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msgBox.setModal(true);
    msgBox.setIcon(QMessageBox::Information);
    Helper::set_deja_vu_font(&msgBox);

    int ret = msgBox.exec();
    if(ret == QMessageBox::Yes) {
        if(db->deletePodcast(_cur_podcast_name)) {
            qDebug() << _cur_podcast_name << "successfully deleted";
            QMap<QString, QString> map;
            if(db->getAllPodcasts(map)) {
                setup_podcasts(map);
            }
        }
    }

    _cur_podcast = -1;
}


void GUI_Podcasts::save_clicked() {
    CDatabaseConnector* db = CDatabaseConnector::getInstance();
	QString name = combo_podcasts->currentText();
	QString url = le_url->text();

    bool success = false;
    if(name.size() > 0 && url.size() > 0) {
        success = db->addPodcast(name, url);
    }

    if(success) {
        QMap<QString, QString> map;
        if(db->getAllPodcasts(map)) {
            setup_podcasts(map);
        }
    }


    _cur_podcast = -1;
	le_url->setText(url);
    url_text_changed(url);
}



void  GUI_Podcasts::play_podcasts(QString url, QString name) {

    MetaDataList v_md;

    // playlist radio
    qDebug() << "is podcast file? ";
    QString content;
    if(Helper::is_podcastfile(url, &content)) {
        qDebug() << "true";

        MetaDataList v_md_tmp;
        if(Podcast::parse_podcast_xml_file_content(content, v_md_tmp) > 0) {

            foreach(MetaData md, v_md_tmp) {

                md.radio_mode = RadioModeStation;
                if(md.title.size() == 0) {
                    if(name.size() > 0)
                        md.title = name;
                    else md.title = "Podcast";
                }

                v_md.push_back(md);
            }
        }

        else {
            qDebug() << "could not extract metadata";

        }
    }
    else {
        qDebug() << url << " is no podcast file";
    }

    if(v_md.size() == 0) return;

    emit sig_create_playlist(v_md, true);
    emit sig_play_track(0, 0, true);

}
