/*
 * MyListView.h
 *
 *  Created on: Jun 27, 2011
 *      Author: luke
 */

#ifndef MYLISTVIEW_H_
#define MYLISTVIEW_H_

#include <QListView>
#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QPoint>
#include <QDrag>

class MyListView :public QListView{
public:
	MyListView(QWidget* parent=0);
	virtual ~MyListView();


	void set_mime_data(QMimeData* data);

	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);

	private:
		bool	_drag;
		QPoint	_drag_pos;
		QWidget* _parent;
		QDrag*	_qDrag;

};

#endif /* MYLISTVIEW_H_ */
