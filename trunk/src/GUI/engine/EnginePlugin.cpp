/* EnginePlugin.cpp */

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




#include "GUI/engine/EnginePlugin.h"

EnginePlugin::EnginePlugin(QString name, QWidget* parent) :
    PlayerPlugin(name, parent)
{
    _ecsc = new EngineColorStyleChooser(minimumWidth(), minimumHeight());
	_ui_style_settings = new GUI_StyleSettings(this);

    _timer = new QTimer();
    _timer->setInterval(30);
    _timer_stopped = true;

    connect(_timer, SIGNAL(timeout()), this, SLOT(timed_out()));
	connect(_ui_style_settings, SIGNAL(sig_style_update()), this, SLOT(sl_update_style()));
}

EnginePlugin::~EnginePlugin(){
    disconnect(_btn_config, SIGNAL(clicked()), this, SLOT(config_clicked()));
    disconnect(_btn_prev, SIGNAL(clicked()), this, SLOT(prev_clicked()));
    disconnect(_btn_next, SIGNAL(clicked()), this, SLOT(next_clicked()));
    disconnect(_btn_close, SIGNAL(clicked()), this, SLOT(close()));

    delete _btn_config;
    delete _btn_prev;
    delete _btn_next;
    delete _btn_close;
	delete _ui_style_settings;
	delete _ecsc;
}


void EnginePlugin::init_buttons(){
    _btn_config->setGeometry(10, 10, 20, 20);
    _btn_prev->setGeometry(35, 10, 20, 20);
    _btn_next->setGeometry(60, 10, 20, 20);
    _btn_close->setGeometry(85, 10, 20, 20);

    connect(_btn_config, SIGNAL(clicked()), this, SLOT(config_clicked()));
    connect(_btn_prev, SIGNAL(clicked()), this, SLOT(prev_clicked()));
    connect(_btn_next, SIGNAL(clicked()), this, SLOT(next_clicked()));
    connect(_btn_close, SIGNAL(clicked()), this, SLOT(close()));

    _btn_config->hide();
    _btn_prev->hide();
    _btn_next->hide();
    _btn_close->hide();
}



void EnginePlugin::config_clicked(){
	_ui_style_settings->show(_cur_style_idx);
}

void EnginePlugin::next_clicked(){
    int n_styles = _ecsc->get_num_color_schemes();
    _cur_style_idx = (_cur_style_idx + 1) % n_styles;

	sl_update_style();
}


void EnginePlugin::prev_clicked(){
    int n_styles = _ecsc->get_num_color_schemes();
    _cur_style_idx = (_cur_style_idx - 1);
    if(_cur_style_idx < 0){
        _cur_style_idx = n_styles - 1;
    }

	sl_update_style();
}



void EnginePlugin::showEvent(QShowEvent *e){
    PlayerPlugin::showEvent(e);
    update();
}


void EnginePlugin::closeEvent(QCloseEvent *e) {
    PlayerPlugin::closeEvent(e);
    update();
}

void EnginePlugin::resizeEvent(QResizeEvent* e){

	sl_update_style();

	QSize new_size = e->size();

	if(!_btn_config) return;

	if(new_size.height() >= 30){
		_btn_config->setGeometry(10, 10, 20, 20);
		_btn_prev->setGeometry(35, 10, 20, 20);
		_btn_next->setGeometry(60, 10, 20, 20);
		_btn_close->setGeometry(85, 10, 20, 20);

		QFont font = _btn_config->font();
		font.setPointSize(8);
		_btn_config->setFont(font);
		_btn_prev->setFont(font);
		_btn_next->setFont(font);
		_btn_close->setFont(font);
	}

	else {
		_btn_config->setGeometry(10, 5, 15, 15);
		_btn_prev->setGeometry(30, 5, 15, 15);
		_btn_next->setGeometry(50, 5, 15, 15);
		_btn_close->setGeometry(70, 5, 15, 15);

		QFont font = _btn_config->font();
		font.setPointSize(6);
		_btn_config->setFont(font);
		_btn_prev->setFont(font);
		_btn_next->setFont(font);
		_btn_close->setFont(font);
	}

}


void EnginePlugin::mousePressEvent(QMouseEvent *e) {

    switch(e->button()){
        case Qt::LeftButton:
            next_clicked();
            break;
        case Qt::MidButton:
            close();
            break;
        case Qt::RightButton:
			_ui_style_settings->show(_cur_style_idx);
			break;
        default:
            break;
    }
}


void EnginePlugin::enterEvent(QEvent* e){
    PlayerPlugin::enterEvent(e);
    _btn_config->show();
    _btn_prev->show();
    _btn_next->show();
    _btn_close->show();

}

void EnginePlugin::leaveEvent(QEvent* e){
    PlayerPlugin::leaveEvent(e);
    _btn_config->hide();
    _btn_prev->hide();
    _btn_next->hide();
    _btn_close->hide();
}

void EnginePlugin::played(){

}

void EnginePlugin::paused(){

}

void EnginePlugin::stopped(){

	_timer->start();
	_timer_stopped = false;
}
