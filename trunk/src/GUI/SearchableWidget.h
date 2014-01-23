#ifndef SEARCHABLEWIDGET_H
#define SEARCHABLEWIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTableView>


class SearchableTableView : public QTableView {
    Q_OBJECT

public:
    SearchableTableView(QWidget* parent=0);

signals:
    void sig_mouse_moved();
    void sig_mouse_pressed();
    void sig_mouse_released();
    void sig_key_pressed(QKeyEvent*);

protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);

};



#endif // SEARCHABLEWIDGET_H
