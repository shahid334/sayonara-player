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
#include "GUI/engine/EngineColorStyleChooser.h"
#include <QCloseEvent>
#include <QShowEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QAction>
#include <QColor>
#include <QTimer>

class GUI_LevelPainter : public PlayerPlugin, private Ui::GUI_LevelPainter
{
    Q_OBJECT
public:
    explicit GUI_LevelPainter(QString name, QString action_text, QWidget *parent=0);

    static QString getVisName(){ return tr("Le&vel"); }
    virtual QAction* getAction();

signals:
    void sig_show(bool);
    void sig_right_clicked(int);
    void closeEvent();

protected:

    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);

public slots:
    void set_level(float, float);
    void psl_stop();
    void psl_style_update(bool inner=false);


private slots:
    void timed_out();

private:
    Ui::GUI_LevelPainter* ui;

    float _level[2];

    EngineColorStyleChooser* _ecsc;
    ColorStyle _cur_style;
    int _cur_style_idx;

    int** _steps;
    QTimer* _timer;
    bool    _timer_stopped;

    void resize_steps(int n_rects);
    void reload();


    
};

#endif // GUI_LEVELPAINTER_H
