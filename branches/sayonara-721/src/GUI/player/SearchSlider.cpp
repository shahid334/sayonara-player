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
}

SearchSlider::~SearchSlider() {
	// TODO Auto-generated destructor stub
}

bool SearchSlider::isSearching(){
	return _searching;
}


bool SearchSlider::event(QEvent* e){



	int percent;
    int cur_val = this->value() * 1.0;
    QMouseEvent* mouseEvent;
    QWheelEvent* wheelEvent;


    switch(e->type()){
        case QEvent::MouseTrackingChange:
        break;

        case QEvent::MouseButtonDblClick:
        break;

		case QEvent::MouseButtonPress:
            if(!isEnabled()) break;
			_searching = true;

			e->ignore();
			mouseEvent = (QMouseEvent*) e;

			if(this->orientation() == Qt::Horizontal)
				percent = (mouseEvent->x() * 100) / this->width();
			else
				percent = 100 - (mouseEvent->y() * 100 / this->height());

            if (percent < 0) percent = 0;
            if (percent > 100) percent = 100;
			this->setValue(percent);

			emit searchSliderPressed(percent);

			break;

		case QEvent::MouseMove:
            if(!isEnabled()) break;
			e->ignore();
			mouseEvent = (QMouseEvent*) e;

			if(this->orientation() == Qt::Horizontal)
				percent = (mouseEvent->x() * 100) / this->width();
			else
				percent = 100 - (mouseEvent->y() * 100 / this->height());

            if (percent < 0) percent = 0;
            if (percent > 100) percent = 100;
			this->setValue(percent);

			if(_searching)
				emit searchSliderMoved(percent);

			break;

		case QEvent::MouseButtonRelease:

            if(!isEnabled()) break;
			e->ignore();
			mouseEvent = (QMouseEvent*) e;

			if(this->orientation() == Qt::Horizontal)
				percent = (mouseEvent->x() * 100) / this->width();
			else
				percent = 100 - (mouseEvent->y() * 100 / this->height());

            if (percent < 0) percent = 0;
            if (percent > 100) percent = 100;

			this->setValue(percent);

			emit searchSliderReleased(percent);
			_searching = false;
			break;

        case QEvent::Wheel:

        if(!isEnabled()) break;
            if(this->orientation() == Qt::Horizontal){
                e->ignore();
                break;
            }
            e->ignore();

            wheelEvent = (QWheelEvent*) e;

            percent = cur_val + (wheelEvent->delta() / abs(wheelEvent->delta()) * 3);

            emit searchSliderMoved(percent);
            _searching = false;
            break;

		default:

            QSlider::event(e);
			break;
	}


	return true;
}

