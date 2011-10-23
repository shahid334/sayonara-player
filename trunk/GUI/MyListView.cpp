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
#include <QDrag>

MyListView::MyListView(QWidget* parent) :
		QListView(parent) {
	_parent = parent;
	_qDrag = new QDrag(this);

}

MyListView::~MyListView() {

}

void MyListView::mousePressEvent(QMouseEvent* event) {

	switch (event->button()) {
	case Qt::LeftButton:

		QListView::mousePressEvent(event);

		if ((this->model()->rowCount()) * 33 > event->pos().y())
			_drag_pos = event->pos();

		else {
			_drag_pos.setY(-10);
			_drag = false;
		}

		break;

	default:
		_drag = false;
		break;
	}
}

void MyListView::mouseMoveEvent(QMouseEvent* event) {

	QPoint pos = event->pos();

	if (_qDrag && _drag
			&& abs(pos.x() - _drag_pos.x()) + abs(pos.y() - _drag_pos.y()) > 20) {
		try {
			_qDrag->exec(Qt::ActionMask);
		}

		catch (...) {
			qDebug() << "Cannot drag";
		}

	}
}

void MyListView::mouseReleaseEvent(QMouseEvent* event) {

	switch (event->button()) {
	case Qt::LeftButton:

		QListView::mouseReleaseEvent(event);
		event->accept();

		_drag = false;

		break;
	case Qt::RightButton:

		break;

	default:
		break;
	}
}

void MyListView::set_mime_data(QMimeData* data) {
	try {
		_qDrag = new QDrag(this);
		_qDrag->setMimeData(data);

		if (data)
			_drag = true;
		else
			_drag = false;
	}

	catch (QString& e) {
		qDebug() << Q_FUNC_INFO << " Mime Data Exception";
	}
}
