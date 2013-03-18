/* GUI_RadioWidget.h */

/* Copyright (C) 2011  Lucio Carreras
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


/*
 * GUIRadioWidget.h
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#ifndef GUIRADIOWIDGET_H_
#define GUIRADIOWIDGET_H_

#include "GUI/ui_GUI_LFMRadioWidget.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include <QWidget>

class GUI_LFMRadioWidget : public PlayerPlugin, private Ui::GUI_LFMRadioWidget{

Q_OBJECT

public:
	GUI_LFMRadioWidget(QString name, QString action_text, QChar shortcut, QWidget *parent = 0);
	virtual ~GUI_LFMRadioWidget();


	signals:
		void listen_clicked(const QString&, int artist);
		void close_event();

    public slots:
        void changeSkin(bool);


	private slots:
		void start_listen();
        void mode_index_changed(int i);


	private:

        Ui::GUI_LFMRadioWidget* _ui;
		QStringList _friends;

};

#endif /* GUIRADIOWIDGET_H_ */
