/* GUI_AudioConverter.h */

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



#ifndef GUI_AUDIOCONVERTER_H
#define GUI_AUDIOCONVERTER_H

#include "HelperStructs/globals.h"
#include "HelperStructs/CSettingsStorage.h"
#include "GUI/ui_GUI_AudioConvert.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include <QAction>


class GUI_AudioConverter : public PlayerPlugin, private Ui::GUI_AudioConvert
{
	Q_OBJECT

signals:
	void sig_active();
	void sig_inactive();

public:
	explicit GUI_AudioConverter(QString name, QString action_text, QWidget *parent=0);
	virtual QAction* getAction();
	static QString getVisName(){ return tr("&mp3 Converter");}
	
public slots:

private slots:
	void rb_cbr_toggled(bool b);
	void rb_vbr_toggled(bool b);
	void quality_changed(int index);
	void cb_active_toggled(bool b);

	
private:
	CSettingsStorage* _settings;
	Ui::GUI_AudioConvert* ui;
	LameBitrate _bitrate;

	void fill_cbr();
	void fill_vbr();
};

#endif // GUI_AUDIOCONVERTER_H
