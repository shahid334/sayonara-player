/*
 * MyListView.h
 *
 *  Created on: Jun 26, 2011
 *      Author: luke
 */

#ifndef MYTABLEVIEW_H_
#define MYTABLEVIEW_H_

#include <QTableView>
#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QPoint>
#include <QDrag>
#include <QPixmap>


class MyTableView : public QTableView{

	Q_OBJECT

public:
	MyTableView(QWidget* parent=0);
	virtual ~MyTableView();

	void set_mime_data(QMimeData* data, QPixmap* pixmap =0);

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

private:
	bool	_drag;
	QPoint	_drag_pos;
	QWidget* _parent;



public:
	QDrag*	qDrag;

};

#endif /* MYLISTVIEW_H_ */
