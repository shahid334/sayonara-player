
#include <GUI/SearchableWidget.h>

SearchableTableView::SearchableTableView(QWidget* parent) : QTableView(parent){


}

void SearchableTableView::mouseMoveEvent(QMouseEvent *e){
    emit sig_mouse_moved();
    QTableView::mouseMoveEvent(e);

}

void SearchableTableView::mousePressEvent(QMouseEvent *e){
    emit sig_mouse_pressed();
    QTableView::mousePressEvent(e);
}


void SearchableTableView::mouseReleaseEvent(QMouseEvent *e){
    emit sig_mouse_released();
    QTableView::mouseReleaseEvent(e);
}

void SearchableTableView::keyPressEvent(QKeyEvent *e){

    emit sig_key_pressed(e);
    QTableView::keyPressEvent(e);
}
