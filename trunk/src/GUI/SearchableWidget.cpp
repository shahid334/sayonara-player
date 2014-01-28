
#include "GUI/SearchableWidget.h"


SearchableTableView::SearchableTableView(QWidget* parent) : QTableView(parent){
	_mini_searcher = new MiniSearcher(this, MiniSearcherBothButtons);
    _abstr_model = 0;
	_cur_row = -1;

	connect(_mini_searcher, SIGNAL(sig_text_changed(QString)), this, SLOT(edit_changed(QString)));
	connect(_mini_searcher, SIGNAL(sig_find_next_row()), this, SLOT(fwd_clicked()));
	connect(_mini_searcher, SIGNAL(sig_find_prev_row()), this, SLOT(bwd_clicked()));

}

SearchableTableView::~SearchableTableView(){
    delete _mini_searcher;
}

void SearchableTableView::setAbstractModel(AbstractSearchTableModel* model){
     _abstr_model = model;
	 _mini_searcher->setExtraTriggers(_abstr_model->getExtraTriggers());
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

	bool shift_pressed = e->modifiers() & Qt::ShiftModifier;
	if(shift_pressed) return;

	bool was_initialized = _mini_searcher->isInitialized();
	bool initialized = _mini_searcher->check_and_init(e);

	if(e->key() == Qt::Key_Tab && !was_initialized) return;

	if(initialized || was_initialized || shift_pressed) {
		_mini_searcher->keyPressEvent(e);
		e->setAccepted(false);
		return;
	}



	QTableView::keyPressEvent(e);
	e->setAccepted(true);
}


void SearchableTableView::edit_changed(QString str){

	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getFirstRowIndexOf(str);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->selectRow(_cur_row);
}


void SearchableTableView::fwd_clicked(){
	QString str = _mini_searcher->getCurrentText();
	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getNextRowIndexOf(str, _cur_row + 1);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->selectRow(_cur_row);
}


void SearchableTableView::bwd_clicked(){

	QString str = _mini_searcher->getCurrentText();
	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getPrevRowIndexOf(str, _cur_row -1);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->selectRow(_cur_row);
}



/**************************************************
 * LIST
 **************************************************/

SearchableListView::SearchableListView(QWidget* parent) : QListView(parent){
	_mini_searcher = new MiniSearcher(this, MiniSearcherBothButtons);
    _abstr_model = 0;
	_cur_row = -1;

	connect(_mini_searcher, SIGNAL(sig_text_changed(QString)), this, SLOT(edit_changed(QString)));
	connect(_mini_searcher, SIGNAL(sig_find_next_row()), this, SLOT(fwd_clicked()));
	connect(_mini_searcher, SIGNAL(sig_find_prev_row()), this, SLOT(bwd_clicked()));

}

SearchableListView::~SearchableListView(){
    delete _mini_searcher;
}


void SearchableListView::setAbstractModel(AbstractSearchListModel* model){
     _abstr_model = model;
	 _mini_searcher->setExtraTriggers(_abstr_model->getExtraTriggers());
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
	bool initialized = _mini_searcher->check_and_init(e);
	if(e->key() == Qt::Key_Tab && !was_initialized) return;

	if(initialized || was_initialized) {
		_mini_searcher->keyPressEvent(e);
		e->setAccepted(false);
		return;
	}


    QListView::keyPressEvent(e);
	e->setAccepted(true);
}

void SearchableListView::edit_changed(QString str){

	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getFirstRowIndexOf(str);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->select_row(_cur_row);
}

void SearchableListView::fwd_clicked(){
	QString str = _mini_searcher->getCurrentText();
	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getNextRowIndexOf(str, _cur_row + 1);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	QList<int> rows;
	rows << idx.row();
	this->scrollTo(idx);
	this->select_rows(rows);
}

void SearchableListView::bwd_clicked(){

	QString str = _mini_searcher->getCurrentText();
	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getPrevRowIndexOf(str, _cur_row - 1);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	QList<int> rows;
	rows << idx.row();
	this->scrollTo(idx);
	this->select_rows(rows);

}

