/*
 * MyListView.h
 *
 *  Created on: Jun 26, 2011
 *      Author: luke
 */

#ifndef MYLISTVIEW_H_
#define MYLISTVIEW_H_

#include <QTableView>
#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QPoint>
#include <QDrag>

class MyTableView : public QTableView{

	Q_OBJECT

public:
	MyTableView(QWidget* parent=0);
	virtual ~MyTableView();

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

private:
	bool	_drag;
	QPoint	_drag_pos;


public:
	QDrag*	qDrag;
};

#endif /* MYLISTVIEW_H_ */
