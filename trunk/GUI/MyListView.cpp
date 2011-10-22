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

	qDebug() << Q_FUNC_INFO << " start";

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

	qDebug() << Q_FUNC_INFO << " end";
}

void MyListView::mouseMoveEvent(QMouseEvent* event) {

	qDebug() << Q_FUNC_INFO << " start";

	QPoint pos = event->pos();

	qDebug() << "Event pos: "
			<< abs(pos.x() - _drag_pos.x()) + abs(pos.y() - _drag_pos.y());

	if (_qDrag && _drag
			&& abs(pos.x() - _drag_pos.x()) + abs(pos.y() - _drag_pos.y()) > 20) {
		try {
			qDebug() << "Exec";
			_qDrag->exec(Qt::ActionMask);

			qDebug() << "Exec end";
		}

		catch (...) {
			qDebug() << "Cannot drag";
		}

	}

	qDebug() << Q_FUNC_INFO << " end";

}

void MyListView::mouseReleaseEvent(QMouseEvent* event) {

	qDebug() << Q_FUNC_INFO << " start";

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

	qDebug() << Q_FUNC_INFO << " end";
}

void MyListView::set_mime_data(QMimeData* data) {
	qDebug() << Q_FUNC_INFO << " start";

	try {
		_qDrag = new QDrag(this);
		_qDrag->setMimeData(data);

		if (data)
			_drag = true;
		else
			_drag = false;

		qDebug() << Q_FUNC_INFO << " end";
	}

	catch (QString& e) {
		qDebug() << "ich kanns abfangen";
	}
}
