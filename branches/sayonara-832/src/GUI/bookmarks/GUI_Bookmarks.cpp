/* GUI_Bookmarks.cpp */

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



#include "HelperStructs/Helper.h"
#include "GUI/bookmarks/GUI_Bookmarks.h"

#include <QMap>

#define TXT_NO_BOOKMARK "--:--"

GUI_Bookmarks::GUI_Bookmarks(QString name, QWidget *parent) :
	PlayerPlugin(name, parent)
{

	ui = new Ui::GUI_Bookmarks();
	ui->setupUi(this);

	_db = CDatabaseConnector::getInstance();

	_cur_time = -1;

	ui->btn_delete->setIcon(QIcon(Helper::getIconPath() + "delete.png"));
	ui->btn_delete_all->setIcon(QIcon(Helper::getIconPath() + "delete.png"));
	ui->btn_new->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	ui->btn_last->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));
	ui->btn_next->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
	ui->lab_logo->setPixmap(QPixmap(Helper::getIconPath() + "bookmarks.png"));

	connect(ui->btn_new, SIGNAL(clicked()), this, SLOT(new_clicked()));
	connect(ui->btn_delete, SIGNAL(clicked()), this, SLOT(del_clicked()));
	connect(ui->btn_delete_all, SIGNAL(clicked()), this, SLOT(del_all_clicked()));
	connect(ui->btn_last, SIGNAL(clicked()), this, SLOT(prev_clicked()));
	connect(ui->btn_next, SIGNAL(clicked()), this, SLOT(next_clicked()));
	connect(ui->cb_bookmarks, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));

	disable_next();
	disable_prev();
}


void GUI_Bookmarks::disable_prev() {
	_last_idx = -1;
	ui->btn_last->setEnabled( false );
	ui->lab_last->setText( TXT_NO_BOOKMARK );
}

void GUI_Bookmarks::disable_next() {
	_next_idx = -1;
	ui->btn_next->setEnabled(false);
	ui->lab_next->setText( TXT_NO_BOOKMARK );
}

void GUI_Bookmarks::enable_prev(int idx) {

	quint32 key;
	QString val, str;
	long ms;

	_last_idx = idx;

	key = _bookmarks.keys()[ idx ];
	val = _bookmarks.value(key);

	ms = (long) (key * 1000);
	str = Helper::cvtMsecs2TitleLengthString(ms, true, false);

	ui->lab_last->setText( str );
	ui->btn_last->setEnabled( true);
}

void GUI_Bookmarks::enable_next(int idx) {

	quint32 key;
	QString val, str;
	long ms;

	_next_idx = idx;
	key = _bookmarks.keys()[ idx ];
	val = _bookmarks.value(key);

	ms = (long) (key * 1000);
	str = Helper::cvtMsecs2TitleLengthString(ms, true, false);

	ui->lab_next->setText(str);
	ui->btn_next->setEnabled(true);


}

void GUI_Bookmarks::calc_prev(quint32 time_s) {

	int i = -1;

	quint32 key = -1;
	QString val = "";



	for(i=_bookmarks.keys().size()-1; i>=0; i--) {
		key = _bookmarks.keys()[i];
		val = _bookmarks.value(key);

		qint32 difference = time_s - key;
		if(difference < 0) continue;


		// possible to go to last index
		if(difference > 0) {
			enable_prev(i);
			return;
		}

		// more than last
		else if(i > 0) {
			enable_prev(i-1);
			return;
		}

		else if(i==0) {
			enable_prev(i);
			return;
		}

	}

	disable_prev();
}


void GUI_Bookmarks::calc_next(quint32 time_s) {

	int i = -1;

	quint32 key = -1;
	QString val = "";



	for(i=0; i<_bookmarks.keys().size(); i++) {
		key = _bookmarks.keys()[i];
		val = _bookmarks.value(key);

		if(key > time_s) {
			enable_next(i);

			return;
		}
	}


	disable_next();
}


void GUI_Bookmarks::track_changed(const MetaData& md) {

	bool success;

	disconnect(ui->cb_bookmarks, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));

	ui->btn_delete->setEnabled(false);
	ui->btn_delete_all->setEnabled(false);

	ui->cb_bookmarks->clear();
	_bookmarks.clear();
	_bookmarks.setInsertInOrder(true);

	disable_next();
	disable_prev();

	_md = md;

	success = _db->searchBookmarks(md.id, _bookmarks);
	if(!success || _bookmarks.size() == 0 || md.id < 0) {
		connect(ui->cb_bookmarks, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));
		return;
	}

	foreach( quint32 bm, _bookmarks.keys() ) {

		long ms = (long) (bm * 1000);
		QString str = Helper::cvtMsecs2TitleLengthString(ms, true, false);

		ui->cb_bookmarks->addItem(str, bm);

	}

	calc_next(0);
	calc_prev(0);

	ui->cb_bookmarks->setCurrentIndex(0);
	ui->btn_delete->setEnabled(true);
	ui->btn_delete_all->setEnabled(true);

	connect(ui->cb_bookmarks, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));
}


void GUI_Bookmarks::pos_changed_s(quint32 new_time) {

	if(!isVisible()) return;

	_cur_time = new_time;

	if(ui->cb_loop->isChecked()) {
		if(_last_idx >= 0 && _next_idx >= 0) {

			if(new_time == _bookmarks.keys()[_next_idx]) {
				prev_clicked();
				return;
			}
		}
	}

	calc_prev(new_time);
	calc_next(new_time);
}


void GUI_Bookmarks::combo_changed(int cur_idx) {

	quint32 bm = _bookmarks.keys()[cur_idx];

	ui->btn_delete->setEnabled(true);
	ui->btn_delete_all->setEnabled(true);

	calc_prev(bm);
	calc_next(bm);

	emit sig_bookmark( bm );
}


void GUI_Bookmarks::next_clicked() {

	if(_next_idx >= 0) {
		int cur_idx = ui->cb_bookmarks->currentIndex();
		ui->cb_bookmarks->setCurrentIndex(_next_idx);

		if(cur_idx == _next_idx)
			emit sig_bookmark(_bookmarks.keys()[cur_idx]);
	}
}

void GUI_Bookmarks::prev_clicked() {

	if(_last_idx >= 0) {
		int cur_idx = ui->cb_bookmarks->currentIndex();
		ui->cb_bookmarks->setCurrentIndex(_last_idx);

		if(cur_idx == _last_idx)
			emit sig_bookmark(_bookmarks.keys()[cur_idx]);
	}
}

void GUI_Bookmarks::new_clicked() {
	bool success;
	QString cur_text;
	int cur_idx;
	int i;

	cur_text = ui->cb_bookmarks->currentText();
	success = _db->insertBookmark(_md.id, _cur_time, cur_text);

	if(success) {
		_bookmarks.insert(_cur_time, cur_text);
		disconnect(ui->cb_bookmarks, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));
		ui->cb_bookmarks->clear();

		cur_idx = -1;
		i = 0;
		foreach( quint32 bm, _bookmarks.keys() ) {

			long ms = (long) (bm * 1000);
			QString str = Helper::cvtMsecs2TitleLengthString(ms, true, false);

			ui->cb_bookmarks->addItem(str, bm);

			if(bm == _cur_time)
				cur_idx = i;

			i++;

			calc_next(_cur_time);
			calc_prev(_cur_time);
		}

		if( cur_idx != -1 ) ui->cb_bookmarks->setCurrentIndex(cur_idx);

		ui->btn_delete->setEnabled(true);
		ui->btn_delete_all->setEnabled(true);

		connect(ui->cb_bookmarks, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));

	}

}


void GUI_Bookmarks::del_clicked() {

	disconnect(ui->cb_bookmarks, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));

	quint64 bm;
	int cur_idx;
	bool success;

	cur_idx = ui->cb_bookmarks->currentIndex();
	bm = ui->cb_bookmarks->itemData(cur_idx).toULongLong();

	success = _db->removeBookmark(_md.id, bm);

	if(success) {
		ui->cb_bookmarks->removeItem(cur_idx);
		if(cur_idx > ui->cb_bookmarks->count() - 1) {
			cur_idx --;
		}

		ui->cb_bookmarks->setCurrentIndex(cur_idx);
		_bookmarks.remove(bm);
		if(_bookmarks.size() == 0) {
			ui->btn_delete->setEnabled(false);
			ui->btn_delete_all->setEnabled(false);
		}

		calc_next(_cur_time);
		calc_prev(_cur_time);
	}




	connect(ui->cb_bookmarks, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));

	calc_prev(_cur_time);
	calc_next(_cur_time);

}

void GUI_Bookmarks::del_all_clicked() {

	bool success;

	disconnect(ui->cb_bookmarks, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));

	success = _db->removeAllBookmarks(_md.id);

	if(success) {

		ui->cb_bookmarks->clear();
		_bookmarks.clear();

		disable_prev();
		disable_next();

		ui->btn_delete->setEnabled(false);
		ui->btn_delete_all->setEnabled(false);

		calc_next(_cur_time);
		calc_prev(_cur_time);
	}

	connect(ui->cb_bookmarks, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));
}
