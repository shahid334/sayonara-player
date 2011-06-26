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
#include <QPixmap>

MyTableView::MyTableView(QWidget* parent) : QTableView(parent) {
	_parent = parent;
	qDrag = new QDrag(_parent);

}

MyTableView::~MyTableView() {


}



void MyTableView::mousePressEvent(QMouseEvent* event){



	switch(event->button()){
		case Qt::LeftButton:

			_drag  = true;
			_drag_pos = event->pos();

			QTableView::mousePressEvent(event);
			break;

		default: break;
	}
}

void MyTableView::mouseMoveEvent(QMouseEvent* event){


	QPoint pos = event->pos();

	qDebug() << 	abs(pos.x() - _drag_pos.x()) + abs(pos.y() - _drag_pos.y());

	if(_drag &&
		abs(pos.x() - _drag_pos.x()) + abs(pos.y() - _drag_pos.y()) > 20){
		qDrag->exec(Qt::MoveAction);

	}

}


void MyTableView::mouseReleaseEvent(QMouseEvent* event){

	switch(event->button()){
		case Qt::LeftButton:

			QTableView::mouseReleaseEvent(event);
			event->accept();

			_drag = false;

			break;
		case Qt::RightButton:

		break;

		default: break;
	}
}


void MyTableView::set_mime_data(QMimeData* data, QPixmap* pixmap){


	this->qDrag = new QDrag(_parent);
	if(pixmap)
		this->qDrag->setPixmap(*pixmap);



	this->qDrag->setMimeData(data);
}
