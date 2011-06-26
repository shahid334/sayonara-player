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

MyTableView::MyTableView(QWidget* parent) : QTableView(parent) {

	qDrag = new QDrag(parent);

}

MyTableView::~MyTableView() {


}



void MyTableView::mousePressEvent(QMouseEvent* event){


	switch(event->button()){
		case Qt::LeftButton:
			QTableView::mousePressEvent(event);
			event->accept();
			_drag  = true;
			_drag_pos = event->pos();
			this->setDropIndicatorShown(false);
			break;
		default: break;
	}
}

void MyTableView::mouseMoveEvent(QMouseEvent* event){


	QPoint pos = event->pos();

	qDebug() << 	abs(pos.x() - _drag_pos.x()) + abs(pos.y() - _drag_pos.y());
	if(_drag &&
		abs(pos.x() - _drag_pos.x()) + abs(pos.y() - _drag_pos.y()) > 20){
		this->setDropIndicatorShown(true);
		qDrag->exec();
		qDebug() << "Drag exec";

	}
}


void MyTableView::mouseReleaseEvent(QMouseEvent* event){

	qDrag->mimeData()->clear();

	switch(event->button()){
		case Qt::LeftButton:
			QTableView::mouseReleaseEvent(event);
			this->qDrag->mimeData()->clear();
			this->qDrag->
			event->accept();
			_drag = false;
			break;
		case Qt::RightButton:

		break;

		default: break;
	}


}
