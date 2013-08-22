/* GUI_LevelPainter.h */

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



#ifndef GUI_LEVELPAINTER_H
#define GUI_LEVELPAINTER_H

#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/ui_GUI_LevelPainter.h"
#include <QCloseEvent>
#include <QShowEvent>
#include <QPaintEvent>
#include <QAction>
#include <QColor>

class GUI_LevelPainter : public PlayerPlugin, private Ui::GUI_LevelPainter
{
    Q_OBJECT
public:
    explicit GUI_LevelPainter(QString name, QString action_text, QWidget *parent=0);

    static QString getVisName(){ return tr("Le&vel"); }
    virtual QAction* getAction();

signals:
    void sig_show(bool);
    void closeEvent();

protected:

    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent* e);

public slots:
    void set_level(float, float);

private:
    Ui::GUI_LevelPainter* ui;

    float _level_l, _level_l_old;
    float _level_r, _level_r_old;

    QColor _white;
    QColor _red, _red_dark;
    QColor _blue;
    QColor _green, _green_dark;
    QColor _yellow, _yellow_dark;
    QColor _black;

    QColor _col_r;
    QColor _col_l;

    int get_last_bright_light(float level);


    
};

#endif // GUI_LEVELPAINTER_H
