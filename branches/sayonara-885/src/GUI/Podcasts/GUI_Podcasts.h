/* GUI_Podcasts.h */

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



#ifndef GUI_PODCASTS_H
#define GUI_PODCASTS_H


#include "GUI/ui_GUI_Podcasts.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "HelperStructs/MetaData.h"

#include <QMap>


class GUI_Podcasts : public PlayerPlugin, private Ui::GUI_Podcasts
{
    Q_OBJECT

signals:
	void sig_create_playlist(const MetaDataList&, bool);
    void sig_play_track(int, qint32, bool);
    void sig_close_event();

private slots:
    void listen_clicked();
    void combo_index_changed(int idx);
    void combo_text_changed(const QString& text);
    void url_text_changed(const QString& text);
    void delete_clicked();
    void save_clicked();


public slots:
    void language_changed();
public:
	GUI_Podcasts(QString name, QWidget *parent = 0);
    virtual ~GUI_Podcasts();


private:

    void init_gui();
    QMap<QString, QString> _podcasts;
    int _cur_podcast;
    QString _cur_podcast_name;
    QString _cur_podcast_adress;

    void setup_podcasts(const QMap<QString, QString>&);
    void play_podcasts(QString url, QString name);



};

#endif // GUI_PODCASTS_H
