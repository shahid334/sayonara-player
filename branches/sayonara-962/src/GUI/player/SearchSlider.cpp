/* SearchSlider.cpp

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Sep 14, 2012 
 *
 */
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QAbstractSlider>
#include "GUI/player/SearchSlider.h"

#include <cmath>


SearchSlider::SearchSlider(QWidget* parent) : QSlider(parent) {
	_searching = false;
    _old_value = 0;
}

SearchSlider::~SearchSlider() {

}

bool SearchSlider::isSearching() {
	return _searching;
}


bool SearchSlider::event(QEvent* e) {



	double percent;
    int cur_val = this->value() * 1.0;
    QMouseEvent* mouseEvent;
    QWheelEvent* wheelEvent;

    mouseEvent = dynamic_cast<QMouseEvent*>( e );
    if(mouseEvent && mouseEvent->button() == Qt::MiddleButton) {
        e->ignore();
        return true;
    }



    switch(e->type()) {

        case QEvent::MouseButtonDblClick:
        e->accept();
        break;

		case QEvent::MouseButtonPress:


            if(!isEnabled()) break;

            mouseEvent = (QMouseEvent*) e;


            e->accept();

            _searching = true;


			if(this->orientation() == Qt::Horizontal){
				percent = (mouseEvent->x() * 1.0) / this->width();
			}

			else{
				percent = 1.0 - ((mouseEvent->y() * 1.0) / this->height());
			}

            if (percent < 0) percent = 0;
			if (percent > 1.0) percent = 1.0;
			this->setValue( (int) (percent * maximum()) );

			emit searchSliderPressed(percent);

			break;

		case QEvent::MouseMove:
            if(!isEnabled()) break;

			mouseEvent = (QMouseEvent*) e;
            e->accept();
			if(this->orientation() == Qt::Horizontal){
				percent = (mouseEvent->x() * 1.0) / this->width();
			}

			else{
				percent = 1.0 - ((mouseEvent->y() * 1.0) / this->height());
			}

			if (percent < 0) percent = 0;
			if (percent > 1.0) percent = 1.0;
			this->setValue( (int) (percent * maximum()) );

			if(_searching){
				emit searchSliderMoved( this->value() );
			}

			break;

        case QEvent::MouseTrackingChange:
            e->accept();
            break;

		case QEvent::MouseButtonRelease:

            if(!isEnabled()) break;

			mouseEvent = (QMouseEvent*) e;
            e->accept();

			if(this->orientation() == Qt::Horizontal){
				percent = (mouseEvent->x() * 1.0) / this->width();
			}

			else{
				percent = 1.0 - ((mouseEvent->y() * 1.0) / this->height());
			}

			if (percent < 0) percent = 0;
			if (percent > 1.0) percent = 1.0;

			this->setValue( (int) (percent * maximum()) );
			emit searchSliderReleased( this->value() );

			_searching = false;
			break;

        case QEvent::Wheel:

            if(!isEnabled()) break;

            e->accept();
            wheelEvent = (QWheelEvent*) e;
			percent = (cur_val * 1.0) / maximum();
			percent += ( wheelEvent->delta() / abs(wheelEvent->delta()) ) * 0.03;

			this->setValue( (int) (percent * maximum()) );
			emit searchSliderMoved( this->value() );

            _searching = false;
            break;

		default:
            QSlider::event(e);
            e->accept();

			break;
	}


	return true;
}

