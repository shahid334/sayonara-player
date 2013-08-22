/* GUI_Spectrum.h */

/* Copyright (C) 2013  Lucio Carreras
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
    explicit GUI_Spectrum(QString name, QString action_text, QWidget *parent=0);

    static QString getVisName(){ return tr("&Spectrum"); }
    virtual QAction* getAction();

signals:
    void sig_show(bool);
    void closeEvent();

protected:

    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent *e);

public slots:
    void set_spectrum(QList<float>&);
    void psl_stop();

private slots:
    void timed_out();

private:
    Ui::GUI_Spectrum* ui;


    QColor _white;
    QColor _red, _red_dark;
    QColor _blue;
    QColor _green, _green_dark;
    QColor _yellow, _yellow_dark;
    QColor _black;

    int _cur_col;

    QList<float> _spec;

    QList< QMap<int, QColor> > _maps_col_active;
    QList< QList< QMap<int, QColor> > > _scheme_fading_rect_color;

    QMap<int, QColor> _map_col_inactive;
    int** _steps;

    QTimer* _timer;
    bool    _timer_stopped;


};

#endif // GUI_SPECTRUM_H
