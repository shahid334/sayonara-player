
#include "GUI/SearchableWidget.h"


SearchableTableView::SearchableTableView(QWidget* parent) : QTableView(parent){
    _mini_searcher = new MiniSearcher(this);


    _abstr_model = 0;

    connect(_mini_searcher, SIGNAL(textChanged(QString)), this, SLOT(edit_changed(QString)));

}

SearchableTableView::~SearchableTableView(){
    delete _mini_searcher;
}



void SearchableTableView::setAbstractModel(AbstractSearchTableModel* model){
     _abstr_model = model;
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

    bool was_initialized = _mini_searcher->isInitialized();

    _mini_searcher->check_and_init(e);


    e->setAccepted(false);
    if(_mini_searcher->isInitialized()
            || was_initialized) return;


    QTableView::keyPressEvent(e);
    e->setAccepted(true);
}


void SearchableTableView::edit_changed(QString str){

    qDebug() << "Strring = " << str;
    if(str.size() == 0) {
        return;
    }


    if(!_abstr_model) {
        qDebug() << "Abstract model not there";
        return;
    }
    qDebug() << "Abstract Model there";

    QModelIndex idx = _abstr_model->getFirstRowIndexOf(str);

    if(!idx.isValid()) return;

    qDebug() << "Idx row = " << idx.row();

    this->scrollTo(idx);
    this->selectRow(idx.row());
}







/**************************************************
 * LIST
 **************************************************/

SearchableListView::SearchableListView(QWidget* parent) : QListView(parent){
    _mini_searcher = new MiniSearcher(this);
    QList<QChar> keys;
    keys << '#' << '$' << ':' << '.' << ',';
    _mini_searcher->register_extra_keys(keys);
    _abstr_model = 0;

    connect(_mini_searcher, SIGNAL(textChanged(QString)), this, SLOT(edit_changed(QString)));

}

SearchableListView::~SearchableListView(){
    delete _mini_searcher;
}



void SearchableListView::setAbstractModel(AbstractSearchListModel* model){
     _abstr_model = model;
}



void SearchableListView::mouseMoveEvent(QMouseEvent *e){
    emit sig_mouse_moved();
    QListView::mouseMoveEvent(e);

}

void SearchableListView::mousePressEvent(QMouseEvent *e){

    emit sig_mouse_pressed();
    QListView::mousePressEvent(e);
}


void SearchableListView::mouseReleaseEvent(QMouseEvent *e){
    emit sig_mouse_released();
    QListView::mouseReleaseEvent(e);
}

void SearchableListView::keyPressEvent(QKeyEvent *e){

    bool was_initialized = _mini_searcher->isInitialized();

    _mini_searcher->check_and_init(e);


    e->setAccepted(false);
    if(_mini_searcher->isInitialized()
            || was_initialized) return;


    QListView::keyPressEvent(e);
    e->setAccepted(true);
}



void SearchableListView::edit_changed(QString str){

    if(str.size() == 0) {
        return;
    }


    if(!_abstr_model) return;

    QModelIndex idx = _abstr_model->getFirstRowIndexOf(str);

    if(!idx.isValid()) return;

    this->scrollTo(idx);
    this->select_row(idx.row());
}

