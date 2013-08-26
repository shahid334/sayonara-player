/* GUI_LevelPainter.cpp */

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



#include "GUI_LevelPainter.h"
#include <QPainter>
#include <QBrush>
#include <QDebug>



GUI_LevelPainter::GUI_LevelPainter(QString name, QString action_text, QWidget *parent) :
    PlayerPlugin(name, action_text, parent)
{
    ui = new Ui::GUI_LevelPainter();
    ui->setupUi(this);


    _ecsc = new EngineColorStyleChooser(this->minimumWidth(), this->minimumHeight());
    _cur_style_idx = 0;
    _cur_style = _ecsc->get_color_scheme_level(_cur_style_idx);

    int n_rects = _cur_style.n_rects;

    _steps = new int*[2];
    for(int b=0; b<2; b++){
        _steps[b] = new int[n_rects];
        for(int r=0; r<n_rects; r++){
            _steps[b][r] = 0;
        }
    }

    _timer = new QTimer();
    _timer->setInterval(30);
    _timer_stopped = true;
    connect(_timer, SIGNAL(timeout()), this, SLOT(timed_out()));
}


void GUI_LevelPainter::set_level(float level_l, float level_r){

    if(!_timer_stopped) {
        _timer->stop();
        _timer_stopped = true;
    }

    _level[0] = level_l;
    _level[1] = level_r;

    this->update();
}

void
GUI_LevelPainter::mousePressEvent(QMouseEvent *e){

    int n_styles = _ecsc->get_num_color_schemes();


    if(e->button() == Qt::LeftButton){
        _cur_style_idx = (_cur_style_idx +  1) % n_styles;
    }

    else if (e->button() == Qt::RightButton)
       emit sig_right_clicked(_cur_style_idx);

    else if (e->button() == Qt::MidButton){
        close();
        return;
    }

    _cur_style = _ecsc->get_color_scheme_level(_cur_style_idx);
    resize_steps(_cur_style.n_rects);
}

void GUI_LevelPainter::paintEvent(QPaintEvent* e){

    QPainter painter(this);

    int n_rects = _cur_style.n_rects;
    int border_x = _cur_style.hor_spacing;
    int border_y = _cur_style.ver_spacing;
    int n_fading_steps = _cur_style.n_fading_steps;
    int h_rect = 6;
    int w_rect = _cur_style.rect_width;

    int y = 10;
    int num_zero = 0;
    for(int c=0; c<2; c++){

        float level = (_level[c] + 45.0f) / 50.0f; // scaled from 0 - 1
        if(level < 0) level = 0;

        int n_colored_rects = n_rects * level;

        for(int r=0; r<n_rects; r++){
            int x = r * (w_rect + border_x);
            QRect rect(x, y, w_rect, h_rect);
            if(r < n_colored_rects){
                painter.fillRect(rect, _cur_style.style[r].value(-1) );
                _steps[c][r] = n_fading_steps - 1;
            }
            else{
                painter.fillRect(rect, _cur_style.style[r].value(_steps[c][r]) );
                if(_steps[c][r] > 0) _steps[c][r] -= 1;
                if(_steps[c][r] == 0) num_zero ++;

            }
        }

        if(num_zero == 2 * n_rects){
            _timer->stop();
            _timer_stopped = true;
        }

        y+= h_rect + border_y;
    }
}

QAction* GUI_LevelPainter::getAction(){
    PlayerPlugin::calc_action(this->getVisName());
    return _pp_action;
}


void GUI_LevelPainter::showEvent(QShowEvent * e){

    e->accept();

    _ecsc->reload(this->width(), this->height());
    _cur_style = _ecsc->get_color_scheme_level(_cur_style_idx);
    resize_steps(_cur_style.n_rects);


    emit sig_show(true);
}

void GUI_LevelPainter::closeEvent(QCloseEvent *e){
    PlayerPlugin::closeEvent(e);
    emit sig_show(false);
}


void GUI_LevelPainter::psl_stop(){

    _timer->start();
    _timer_stopped = false;

}

void GUI_LevelPainter::timed_out(){

    for(int i=0; i<2; i++)
        _level[i] -= 2.0f;

    update();
}


void GUI_LevelPainter::psl_style_update(){
    _ecsc->reload(this->width(), this->height());
    _cur_style = _ecsc->get_color_scheme_level(_cur_style_idx);
    resize_steps(_cur_style.n_rects);

}

void GUI_LevelPainter::resize_steps(int n_rects){

    for(int i=0; i<2; i++){
        delete[] _steps[i];
        _steps[i] = new int[n_rects];
        for(int j=0; j<n_rects; j++){
            _steps[i][j] = 0;
        }
    }

}
