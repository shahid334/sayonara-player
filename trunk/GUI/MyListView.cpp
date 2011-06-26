/*
 * MyListView.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: luke
 */

#include "GUI/MyListView.h"
#include <QWidget>
#include <QListView>
#include <QMouseEvent>
#include <QDebug>

MyListView::MyListView(QWidget* parent) : QListView(parent) {
	_parent = parent;
	qDrag = new QDrag(_parent);

}

MyListView::~MyListView() {


}



void MyListView::mousePressEvent(QMouseEvent* event){



	switch(event->button()){
		case Qt::LeftButton:

			_drag  = true;
			_drag_pos = event->pos();

			QListView::mousePressEvent(event);
			break;

		default: break;
	}
}

void MyListView::mouseMoveEvent(QMouseEvent* event){


	QPoint pos = event->pos();

	qDebug() << 	abs(pos.x() - _drag_pos.x()) + abs(pos.y() - _drag_pos.y());

	if(_drag &&
		abs(pos.x() - _drag_pos.x()) + abs(pos.y() - _drag_pos.y()) > 20){
		qDrag->exec(Qt::MoveAction);

	}

}


void MyListView::mouseReleaseEvent(QMouseEvent* event){

	switch(event->button()){
		case Qt::LeftButton:

			QListView::mouseReleaseEvent(event);
			event->accept();

			_drag = false;

			break;
		case Qt::RightButton:

		break;

		default: break;
	}
}


void MyListView::set_mime_data(QMimeData* data){

	this->qDrag = new QDrag(_parent);
	this->qDrag->setMimeData(data);
}
