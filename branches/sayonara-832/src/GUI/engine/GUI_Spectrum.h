/* GUI_Spectrum.h */

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



#ifndef GUI_SPECTRUM_H
#define GUI_SPECTRUM_H

#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/engine/EngineColorStyleChooser.h"
#include "GUI/ui_GUI_Spectrum.h"
#include <QCloseEvent>
#include <QShowEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QAction>
#include <QColor>
#include <QList>
#include <QMap>
#include <QTimer>

class GUI_Spectrum : public PlayerPlugin, private Ui::GUI_Spectrum
{
    Q_OBJECT
public:
	explicit GUI_Spectrum(QString name, QWidget *parent=0);


signals:
    void sig_show(bool);
    void closeEvent();
    void sig_right_clicked(int);

protected:

    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent *e);

public slots:
    void set_spectrum(QList<float>&);
    void psl_stop();
    void psl_style_update();

private slots:
    void timed_out();

private:
    Ui::GUI_Spectrum* ui;

    QList<float> _spec;

    int** _steps;
    int   _cur_style_idx;
    ColorStyle _cur_style;


    QTimer* _timer;
    bool    _timer_stopped;
    EngineColorStyleChooser* _ecsc;
    bool _update_running;

    void resize_steps(int bins, int rects);

};

#endif // GUI_SPECTRUM_H
