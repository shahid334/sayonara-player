/* SearchableWidget.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef SEARCHABLEWIDGET_H
#define SEARCHABLEWIDGET_H

#include <QList>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTableView>
#include <QListView>
#include <QFocusEvent>
#include "GUI/MiniSearcher.h"
#include "GUI/library/model/LibraryItemModel.h"
#include "GUI/AbstractSearchModel.h"


class MiniSearcher;

class SearchableTableView : public QTableView {
    Q_OBJECT

signals:
    void sig_mouse_moved();
    void sig_mouse_pressed();
    void sig_mouse_released();
    void sig_focus_out();
    void sig_key_pressed(QKeyEvent*);


private slots:
    void edit_changed(QString str);
	void fwd_clicked();
	void bwd_clicked();

private:
	MiniSearcher*               _mini_searcher;
    AbstractSearchTableModel*   _abstr_model;
	int							_cur_row;

public:
    SearchableTableView(QWidget* parent=0);
    virtual ~SearchableTableView();
    void setAbstractModel(AbstractSearchTableModel* model);

protected:

    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
	void keyPressEvent(QKeyEvent *);

};


class SearchableListView : public QListView {
    Q_OBJECT

signals:
    void sig_mouse_moved();
    void sig_mouse_pressed();
    void sig_mouse_released();
    void sig_focus_out();
    void sig_key_pressed(QKeyEvent*);

private slots:
    void edit_changed(QString str);
	void fwd_clicked();
	void bwd_clicked();

private:

	MiniSearcher*               _mini_searcher;
	AbstractSearchListModel*	_abstr_model;
	int							_cur_row;

public:
    SearchableListView(QWidget* parent=0);
    virtual ~SearchableListView();
    void setAbstractModel(AbstractSearchListModel* model);


protected:

    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);

    virtual void select_rows(QList<int> rows)=0;
    virtual void select_row(int row)=0;
};





#endif // SEARCHABLEWIDGET_H
