#ifndef SEARCHABLEWIDGET_H
#define SEARCHABLEWIDGET_H

#include <QList>
#include <QWidget>
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


protected slots:
    void edit_changed(QString str);

private:
    MiniSearcher*                   _mini_searcher;
    AbstractSearchTableModel*   _abstr_model;

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


protected slots:
    void edit_changed(QString str);

private:

    AbstractSearchListModel*  _abstr_model;

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
    MiniSearcher*             _mini_searcher;

};





#endif // SEARCHABLEWIDGET_H
