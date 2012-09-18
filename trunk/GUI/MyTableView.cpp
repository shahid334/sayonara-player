/* MyTableView.cpp */

/* Copyright (C) 2011  Lucio Carreras
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


/*
 * MyTableView.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: luke
 */

#include "GUI/MyTableView.h"
#include <QWidget>
#include <QTableView>
#include <QMouseEvent>
#include <QDebug>


#include "HelperStructs/CustomMimeData.h"



MyTableView::MyTableView(QWidget* parent) : QTableView(parent) {
	_parent = parent;
	_qDrag = 0;

}

MyTableView::~MyTableView() {

	/*if(_qDrag) delete _qDrag;
	_qDrag = 0;*/

}



void MyTableView::mousePressEvent(QMouseEvent* event){

	QPoint pos_org = event->pos();
	QPoint pos = QWidget::mapToGlobal(pos_org);

	switch(event->button()){
		case Qt::LeftButton:

			QTableView::mousePressEvent(event);

			if(event->pos().y() > this->model()->rowCount() * 20) {
				_drag = false;
				break;
			}

			else {
				_drag_pos = event->pos();
				_drag = true;
			}

			break;

		case Qt::RightButton:
			_drag = false;

			QTableView::mousePressEvent(event);
			pos.setY(pos.y() + 35);
			pos.setX(pos.x() + 10);
			emit context_menu_emitted(pos);
			break;

		case Qt::MidButton:
			_drag = false;

			QTableView::mousePressEvent(event);

			emit sig_middle_button_clicked(pos);
			break;

		default:
			_drag = false;
			break;
	}

}

void MyTableView::mouseMoveEvent(QMouseEvent* event){

	QPoint pos = event->pos();
	int distance =  abs(pos.x() - _drag_pos.x()) +	abs(pos.y() - _drag_pos.y());

	if (_drag && _qDrag && distance > 20) {
		_qDrag->exec(Qt::ActionMask);
	}

}


void MyTableView::mouseReleaseEvent(QMouseEvent* event){

	switch (event->button()) {

		case Qt::LeftButton:

			QTableView::mouseReleaseEvent(event);
			event->accept();

			_drag = false;
			break;

		default: break;
	}
}


void MyTableView::set_mime_data(CustomMimeData* data){

	_qDrag = new QDrag(this);
	_qDrag->setMimeData(data);

	if (data) _drag = true;
	else _drag = false;
}



