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
#include <QPoint>
MyTableView::MyTableView(QWidget* parent) : QTableView(parent) {
	_parent = parent;
	qDrag = new QDrag(this);

}

MyTableView::~MyTableView() {


}



void MyTableView::mousePressEvent(QMouseEvent* event){

	QPoint pos_org = event->pos();
	QPoint pos = QWidget::mapToGlobal(pos_org);

	switch(event->button()){
		case Qt::LeftButton:

			if(event->pos().y() > this->model()->rowCount() * 20) {
				_drag = false;
				break;
			}

			else {
				_drag_pos = event->pos();
				_drag = true;
			}

			QTableView::mousePressEvent(event);
			break;

		case Qt::RightButton:
			_drag = false;

			QTableView::mousePressEvent(event);
			pos.setY(pos.y() + 35);
			pos.setX(pos.x() + 10);
			emit context_menu_emitted(pos);
			break;

		default: break;
	}
}

void MyTableView::mouseMoveEvent(QMouseEvent* event){


	QPoint pos = event->pos();

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


		default: break;
	}
}


void MyTableView::set_mime_data(QMimeData* data, QPixmap* pixmap){


	this->qDrag = new QDrag(this);
	if(pixmap)
		this->qDrag->setPixmap(*pixmap);

	this->qDrag->setMimeData(data);

	if(data) _drag = true;
	else _drag = false;
}
