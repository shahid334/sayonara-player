#include "GUI/library/view/LibraryViewAlbum.h"
#include <QHeaderView>

LibraryViewAlbum::LibraryViewAlbum(QWidget *parent) :
	LibraryView(parent)
{
	_discmenu = 0;
	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(timer_timed_out()));
}


template<> void LibraryViewAlbum::fill<AlbumList, Album>(const AlbumList& input_data){

	_discnumbers.clear();
	for(const Album& album: input_data){
		_discnumbers << album.discnumbers;
	}

	LibraryView::fill<AlbumList, Album>(input_data);

}

void LibraryViewAlbum::rc_menu_show(const QPoint & p){
	delete_discmenu();

	LibraryView::rc_menu_show(p);
}

void LibraryViewAlbum::mousePressEvent(QMouseEvent *e){

	LibraryView::mousePressEvent(e);

	if( _timer->isActive() ) return;

	QList<int> selected = _model->get_selected();
	if(selected.size() == 1){

		init_discmenu( this->indexAt(e->pos()) );
	}
}

void LibraryViewAlbum::mouseMoveEvent(QMouseEvent* e){
	QPoint pos = e->pos();
	int distance = abs(pos.x() - _drag_pos.x()) +	abs(pos.y() - _drag_pos.y());

	if (_drag && _qDrag && distance > 20) {
		delete_discmenu();
		_qDrag->exec(Qt::CopyAction);

		_timer->stop();

	}
}


void LibraryViewAlbum::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ){

	if(_cur_filling) return;

	QTableView::selectionChanged(selected, deselected);

	QModelIndexList idxs = this->selectionModel()->selectedIndexes();
	QList<int> idx_list_int;

	for(const QModelIndex& idx : idxs) {

		int row = idx.row();

		if( !idx_list_int.contains(row) ){
			idx_list_int << row;
		}
	}

	_model->set_selected(idx_list_int);

	if(selected.indexes().size() > 0) {
		scrollTo(selected.indexes()[0]);
	}

	_timer->stop();

	if( idx_list_int.size() == 1 ) {


		init_discmenu(idxs[0]);
	}

	emit sig_sel_changed(idx_list_int);
}


/* where to show the popup */
void LibraryViewAlbum::calc_discmenu_point(QModelIndex idx){

	_discmenu_point = QCursor::pos();

	QRect box = this->geometry();
	box.moveTopLeft(this->parentWidget()->mapToGlobal(box.topLeft()));

	if(!box.contains(_discmenu_point)){
		_discmenu_point.setX(box.x() + (box.width() * 2) / 3);
		_discmenu_point.setY(box.y());

		QPoint dmp_tmp = parentWidget()->pos();
		dmp_tmp.setY(dmp_tmp.y() - this->verticalHeader()->sizeHint().height());

		while(idx.row() != indexAt(dmp_tmp).row()){
			  dmp_tmp.setY(dmp_tmp.y() + 10);
			  _discmenu_point.setY(_discmenu_point.y() + 10);
		}
	}
}

/* starts timer, after 500ms the popup is displayed */
void LibraryViewAlbum::init_discmenu(QModelIndex idx){

	int row = idx.row();

	calc_discmenu_point(idx);

	QList<quint8> discnumbers = _discnumbers.at( row );

	if( discnumbers.size() > 1 ) {

		delete_discmenu();

		_discmenu = new DiscPopupMenu(this, discnumbers);
		_timer->start(500);

		connect(_discmenu, SIGNAL(sig_disc_pressed(int)), this, SLOT(disc_pressed(int)));
	}
}


void LibraryViewAlbum::delete_discmenu(){
	if(!_discmenu) {
		return;
	}

	_discmenu->hide();
	_discmenu->close();

	disconnect(_discmenu, SIGNAL(sig_disc_pressed(int)), this, SLOT(disc_pressed(int)));

	delete _discmenu;
	_discmenu = 0;
}

void LibraryViewAlbum::timer_timed_out() {
	show_discmenu();
}


void LibraryViewAlbum::show_discmenu(){
	_timer->stop();

	if(!_discmenu) return;

	_discmenu->popup(_discmenu_point);
}

/* index in popup selected */
void LibraryViewAlbum::disc_pressed(int idx){
	emit sig_disc_pressed(idx);
}


