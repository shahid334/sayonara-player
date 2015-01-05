/* GUI_Stream.cpp */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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


#include "GUI/ui_GUI_Stream.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/stream/GUI_Stream.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/PlaylistParser.h"
#include "HelperStructs/Style.h"
#include "DatabaseAccess/CDatabaseConnector.h"


#include <QIcon>
#include <QMap>
#include <QInputDialog>
#include <QMessageBox>
#include <QPixmap>


GUI_Stream::GUI_Stream(QString name, QWidget *parent) :
	PlayerPlugin(name, parent),
	Ui::GUI_Stream() {

	setupUi(this);

	init_gui();
	_cur_station = -1;

	QMap<QString, QString> data;
	CDatabaseConnector::getInstance()->getAllStreams(data);
	if(data.size() > 0)
		setup_stations(data);

    btn_listen->setIcon(Helper::getIcon("play.png"));

	connect(btn_listen, SIGNAL(clicked()), this, SLOT(listen_clicked()));
	connect(btn_save, SIGNAL(clicked()), this, SLOT(save_clicked()));
	connect(btn_delete, SIGNAL(clicked()), this, SLOT(delete_clicked()));
	connect(combo_stream, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_index_changed(int)));
	connect(combo_stream, SIGNAL(editTextChanged(const QString&)), this, SLOT(combo_text_changed(const QString&)));

	connect(le_url, SIGNAL(textEdited(const QString&)), this, SLOT(url_text_changed(const QString&)));

    hide();
}


GUI_Stream::~GUI_Stream() {

}


void GUI_Stream::changeSkin(bool dark) {

}

void GUI_Stream::language_changed() {
	retranslateUi(this);
}


void GUI_Stream::listen_clicked() {

	QString url;
	QString name;

	if(_cur_station == -1) {
		url = le_url->text();
        name = tr("Radio");
	}

	else{
		url = _cur_station_adress;
		name = _cur_station_name;
	}

    url = url.trimmed();
	if(url.size() > 5) {

        play_stream(url, name);
	}
}


void GUI_Stream::setup_stations(const QMap<QString, QString>& radio_stations) {

	_stations = radio_stations;
	if(radio_stations.size() > 0) {
		_cur_station = -1;
	}

	combo_stream->clear();

	_cur_station_adress = "";
	_cur_station_name = "";
	_cur_station = 0;

	_stations[""] = "";

    QMap<QString, QString>::iterator it;
    for(it = _stations.begin(); it != _stations.end(); it++) {
		combo_stream->addItem(it.key(), it.value());
	}

	btn_listen->setEnabled(false);
	btn_save->setEnabled(false);
	btn_delete->setEnabled(false);
}


void GUI_Stream::init_gui() {
    btn_delete->setIcon(Helper::getIcon("delete.png"));
    btn_save->setIcon(Helper::getIcon("save.png"));
    lab_icon->setPixmap(Helper::getPixmap("radio.png", QSize(50, 50), false));
}


void GUI_Stream::combo_index_changed(int idx) {

	_cur_station = idx;
	_cur_station_name = combo_stream->itemText(_cur_station);

	QString adress = _stations[_cur_station_name];
	if(adress.size() > 0) {
		_cur_station_adress = adress;
		le_url->setText(_cur_station_adress);
	}

	if(idx == 0) {
		le_url->setText("");
	}

	btn_delete->setEnabled(idx > 0);
	btn_save->setEnabled(false);
	btn_listen->setEnabled(le_url->text().size() > 5);
	combo_stream->setToolTip(_cur_station_adress);
}


void GUI_Stream::combo_text_changed(const QString& text) {
	_cur_station = -1;

    bool name_there = false;
	for(int i=0; i<combo_stream->count(); i++) {
		QString str = combo_stream->itemText(i);
        if( str.compare(text) == 0) {
            name_there = true;
            break;
        }
    }

	btn_delete->setEnabled(name_there);
    btn_save->setEnabled(text.size() > 0);
	btn_listen->setEnabled(le_url->text().size() > 5);
	combo_stream->setToolTip("");
}

void GUI_Stream::url_text_changed(const QString& text) {

    QString key = _stations.key(text.trimmed());

	if(! key.isEmpty() ) {

		int idx = combo_stream->findText(key, Qt::MatchCaseSensitive);
		if(idx != -1) {
			combo_stream->setCurrentIndex(idx);
			_cur_station = idx;
			btn_save->setEnabled(false);
			btn_delete->setEnabled(true);
		}
	}

	// new adress
	else{

		btn_delete->setEnabled(false);

		bool save_enabled =
				combo_stream->currentText().size() > 0 &&
				le_url->text().size() > 5 &&
				_cur_station == -1;

		btn_save->setEnabled(save_enabled);
		btn_listen->setEnabled(text.size() > 5);
		if(_cur_station != -1) {
			_cur_station = -1;
			combo_stream->setEditText(tr("new"));
			_cur_station = -1;
		}
	}
}


void GUI_Stream::delete_clicked() {
	if(_cur_station == -1) return;

	CDatabaseConnector* db = CDatabaseConnector::getInstance();
    QMessageBox msgBox(this);
    QString ask = tr("Really wanna delete %1?").arg(_cur_station_name);
    msgBox.setText(ask );
	msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msgBox.setModal(true);
    msgBox.setIcon(QMessageBox::Information);
	int ret = msgBox.exec();
	if(ret == QMessageBox::Yes) {
		if(db->deleteStream(_cur_station_name)) {
			qDebug() << _cur_station_name << "successfully deleted";
			QMap<QString, QString> map;
			if(db->getAllStreams(map)) {
				setup_stations(map);
			}
		}
	}

	_cur_station = -1;
}


void GUI_Stream::save_clicked() {
	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	QString name = combo_stream->currentText();
	QString url = le_url->text();

	bool success = false;
	if(name.size() > 0 && url.size() > 0) {
		success = db->addStream(name, url);
	}

	if(success) {
		QMap<QString, QString> map;
		if(db->getAllStreams(map)) {
			setup_stations(map);
            for(int i=0; i<combo_stream->count(); i++){
                QString s = combo_stream->itemText(i);
                if(s.compare(name) == 0){
                    combo_stream->setCurrentIndex(i);
                    break;
                }
            }
		}
	}


	_cur_station = -1;
	le_url->setText(url);
    url_text_changed(url);
}


void GUI_Stream::play_stream(QString url, QString name) {

    MetaDataList v_md;

    if(Helper::is_playlistfile(url)) {
        MetaDataList v_md_tmp;
        if(PlaylistParser::parse_playlist(url, v_md_tmp) > 0) {

            foreach(MetaData md, v_md_tmp) {

                if(name.isEmpty()){
                    md.album = url;
                    if(md.title.isEmpty()){
                        md.title = tr("Radio station");
                    }
                }

                else{
                    md.album = name;
                    if(md.title.isEmpty()){
                        md.title = name;
                    }
                }

                if(md.artist.isEmpty()){
                    md.artist = url;
                }

                md.radio_mode = RadioModeStation;
                v_md.push_back(md);
                qDebug() << "Url = " << md.filepath;
            }
        }
    }

    // real stream
    else{

        MetaData md;

        if(name.isEmpty()){
            md.title = tr("Radio station");
            md.album = url;
        }

        else{
            md.title = name;
            md.album = name;
        }

        md.artist = url;
        md.filepath = url;
        md.radio_mode = RadioModeStation;

        v_md.push_back(md);
    }

    emit sig_create_playlist(v_md, true);
}
