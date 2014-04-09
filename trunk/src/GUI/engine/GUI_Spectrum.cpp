/* GUI_Spectrum.cpp */

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



#include "GUI/engine/GUI_Spectrum.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CSettingsStorage.h"

#include <QPainter>
#include <QList>
#include <QDebug>
#include <cmath>
#include <QTimer>

float log_lu[1100];




GUI_Spectrum::GUI_Spectrum(QString name, QString action_text, QWidget *parent) :
    PlayerPlugin(name, action_text, parent)
{

    _cur_style_idx = CSettingsStorage::getInstance()->getSpectrumStyle();
    ui = new Ui::GUI_Spectrum();
    ui->setupUi(this);
    _timer = new QTimer();
    _timer->setInterval(30);
    _timer_stopped = true;
    connect(_timer, SIGNAL(timeout()), this, SLOT(timed_out()));

    _ecsc = new EngineColorStyleChooser(this->minimumWidth(), this->minimumHeight());
    _cur_style = _ecsc->get_color_scheme_spectrum(_cur_style_idx);

    _update_running = false;

    for(int i=0; i<N_BINS; i++){

        _spec << 0.0f;
    }

    for(int i=0; i<1100; i++){
        log_lu[i] = log( (i * 1.0f) / 10.0f );
    }

    _steps = new int*[N_BINS];
    for(int i=0; i<N_BINS; i++){
        _steps[i] = new int[_cur_style.n_rects];
        for(int j=0; j<_cur_style.n_rects; j++){
            _steps[i][j] = 0;
        }
    }


    this->update();

}


void
GUI_Spectrum::mousePressEvent(QMouseEvent *e){

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

    _cur_style = _ecsc->get_color_scheme_spectrum(_cur_style_idx);
    resize_steps(N_BINS, _cur_style.n_rects);

    CSettingsStorage::getInstance()->setSpectrumStyle(_cur_style_idx);

}

void
GUI_Spectrum::set_spectrum(QList<float>& lst){
    if(!_timer_stopped) _timer->stop();

    _spec = lst;
    this->update();
}


void
GUI_Spectrum::paintEvent(QPaintEvent *e){

    if(_update_running) return;
     QPainter painter(this);

    float widget_height = (float) this->height();

     int n_rects = _cur_style.n_rects;
     int n_fading_steps = _cur_style.n_fading_steps;
     int h_rect = widget_height / (n_rects) - _cur_style.ver_spacing;
     int border_y = _cur_style.ver_spacing;
     int border_x = _cur_style.hor_spacing;


    int x=3;
    int ninety = (_spec.size() * 500) / 1000;
    int offset = 0;
    if(ninety == 0) return;

    int w_bin = ((this->width() + 10) / (ninety - offset)) - border_x;



    int n_zero = 0;



    // run through all bins
    for(int i=offset; i<ninety + 1; i++){


        float f = _spec[i] * log_lu[ i*10 + 54] * 0.60f;

        // if this is one bar, how tall would it be?
        int h =  f * widget_height;

        // how many colored rectangles would fit into this bar?
        int colored_rects = h / (h_rect + border_y)  -1 ;
        if (colored_rects < 0) colored_rects = 0;

        // we start from bottom with painting
        int y = widget_height - h_rect;

        // run vertical
        for(int r=0; r<n_rects; r++){

            QColor col;

            // 100%
            if( r < colored_rects){
                col = _cur_style.style[r].value(-1);
                _steps[i][r] = n_fading_steps;
            }

            // fading out
            else{
                col = _cur_style.style[r].value(_steps[i][r]);

                if(_steps[i][r] > 0) _steps[i][r]--;
                else n_zero++;
            }


            QRect rect(x, y, w_bin, h_rect);
            painter.fillRect(rect, col);

            y -= (h_rect + border_y);
        }

        x += w_bin + border_x;
    }

    if(n_zero == (ninety - offset) * n_rects && _timer->isActive()){
        _timer->stop();
        _timer_stopped = true;
    }
}


QAction* GUI_Spectrum::getAction(){
    PlayerPlugin::calc_action(this->getVisName());
    return _pp_action;
}


void GUI_Spectrum::showEvent(QShowEvent * e){
    Q_UNUSED(e);
    this->update();
    emit sig_show(true);
}

void GUI_Spectrum::closeEvent(QCloseEvent *e){
    PlayerPlugin::closeEvent(e);
    this->update();
    emit sig_show(false);
}

void GUI_Spectrum::psl_stop(){

    _timer->start();
    _timer_stopped = false;
}

void GUI_Spectrum::timed_out(){


    for(int i=0; i<N_BINS; i++){


            _spec[i] -= 0.024f;

    }

    update();
}


void GUI_Spectrum::resize_steps(int bins, int rects){

    for(int b=0; b<N_BINS; b++){
        delete _steps[b];
        _steps[b] = 0;
    }

    delete[] _steps;

    _steps = new int*[N_BINS];
    for(int i=0; i<N_BINS; i++){
        _steps[i] = new int[rects];
        for(int r=0; r<rects; r++){
            _steps[i][r] = 0;
        }
    }
}


void GUI_Spectrum::psl_style_update(){

   _ecsc->reload(this->width(), this->height());

   _cur_style = _ecsc->get_color_scheme_spectrum(_cur_style_idx);
   resize_steps(N_BINS, _cur_style.n_rects);


   _update_running = false;
   this->update();
}
