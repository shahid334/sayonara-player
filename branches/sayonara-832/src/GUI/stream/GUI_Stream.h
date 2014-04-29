/* GUI_Stream.h */

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


#ifndef GUI_STREAM_H_
#define GUI_STREAM_H_

#include "GUI/ui_GUI_Stream.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "HelperStructs/Helper.h"

#include <QWidget>
#include <QMap>



class GUI_Stream : public PlayerPlugin, private Ui::GUI_Stream {

	Q_OBJECT

signals:
    void sig_create_playlist(MetaDataList&, bool);
    void sig_play_track(int, qint32, bool);
	void sig_close_event();

private slots:
	void listen_clicked();
	void combo_index_changed(int idx);
	void combo_text_changed(const QString& text);
	void url_text_changed(const QString& text);
	void delete_clicked();
	void save_clicked();

private:
    void play_stream(QString url, QString station_name);

public slots:
    void changeSkin(bool);
    void language_changed();


public:
    GUI_Stream(QString name, QString action_text, QWidget *parent = 0);
	virtual ~GUI_Stream();

    static QString getVisName(){return tr("&Webstreams");}
    virtual QAction* getAction();



private:
	Ui::GUI_Stream* ui;

	void init_gui();
	QMap<QString, QString> _stations;
	int _cur_station;
	QString _cur_station_name;
	QString _cur_station_adress;

	void setup_stations(const QMap<QString, QString>&);


};

#endif /* GUI_STREAM_H_ */
