/* EnginePlugin.h */

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



#ifndef ENGINEPLUGIN_H
#define ENGINEPLUGIN_H

#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/engine/GUI_StyleSettings.h"
#include "GUI/engine/EngineColorStyleChooser.h"

#include <QAction>
#include <QColor>
#include <QTimer>
#include <QPushButton>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QShowEvent>
	

class EnginePlugin : public PlayerPlugin {

	Q_OBJECT

protected:


	QPushButton* _btn_config;
	QPushButton* _btn_prev;
	QPushButton* _btn_next;
	QPushButton* _btn_close;

	GUI_StyleSettings*			_ui_style_settings;
    EngineColorStyleChooser*    _ecsc;
    ColorStyle                  _cur_style;
    int                         _cur_style_idx;

    QTimer*     _timer;
    int         _timer_stopped;

    void init_buttons();

    virtual void showEvent(QShowEvent* e);
    virtual void closeEvent(QCloseEvent* e);
    virtual void resizeEvent(QResizeEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void enterEvent(QEvent* e);
    virtual void leaveEvent(QEvent* e);


protected slots:

    virtual void config_clicked();
    virtual void next_clicked();
    virtual void prev_clicked();

	virtual void timed_out()=0;


	virtual void played();
	virtual void paused();
	virtual void stopped();


public slots:
	virtual void sl_update_style()=0;


public:
    EnginePlugin(QString name, QWidget* parent=0);
    virtual ~EnginePlugin();

};

#endif // ENGINEPLUGIN_H


