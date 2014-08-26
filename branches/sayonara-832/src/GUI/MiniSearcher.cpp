/* MiniSearcher.cpp */

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


#include "MiniSearcher.h"
#include "HelperStructs/Helper.h"

#include <QScrollBar>

MiniSearcherLineEdit::MiniSearcherLineEdit(QWidget* parent) : QLineEdit(parent) {

}

MiniSearcherLineEdit::~MiniSearcherLineEdit() {}

void MiniSearcherLineEdit::keyPressEvent(QKeyEvent *e) {

	int key = e->key();

	switch(key) {

		case Qt::Key_Tab:
			emit sig_tab_pressed();
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_Escape:
		case Qt::Key_Enter:
		case Qt::Key_Return:
			e->setAccepted(false);
			return;

		default:

			QLineEdit::keyPressEvent(e);
			break;
	}
}

bool MiniSearcherLineEdit::event(QEvent * e) {

	if(e->type() == QEvent::FocusOut) {
		emit sig_le_focus_lost();
	}

	return QLineEdit::event(e);
}


MiniSearcher::MiniSearcher(SearchableTableView *parent, MiniSearcherButtons b) :
	QFrame(parent)
{
    _parent = parent;

    connect(parent, SIGNAL(sig_mouse_pressed()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_mouse_moved()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_mouse_released()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_focus_out()), this, SLOT(reset()));
	//connect(parent, SIGNAL(sig_key_pressed(QKeyEvent*)), this, SLOT(key_pressed(QKeyEvent*)));

	initLayout(b);

}

MiniSearcher::MiniSearcher(SearchableListView *parent, MiniSearcherButtons b) :
	QFrame(parent)
{
    _parent = parent;

    connect(parent, SIGNAL(sig_mouse_pressed()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_mouse_moved()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_mouse_released()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_focus_out()), this, SLOT(reset()));
	//connect(parent, SIGNAL(sig_key_pressed(QKeyEvent*)), this, SLOT(key_pressed(QKeyEvent*)));

	initLayout(b);


}

void MiniSearcher::initLayout(MiniSearcherButtons b) {

	bool left, right;
	left=right=false;
	_line_edit = new MiniSearcherLineEdit(this);
	_line_edit->setMaximumWidth(100);
	//this->setFocusProxy(_line_edit);
	connect(_line_edit, SIGNAL(textChanged(QString)), this, SLOT(line_edit_text_changed(QString)));
	connect(_line_edit, SIGNAL(sig_tab_pressed()), this, SLOT(right_clicked()));
	connect(_line_edit, SIGNAL(sig_le_focus_lost()), this, SLOT(line_edit_focus_lost()));

	_layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
	_layout->setContentsMargins(4, 4, 4, 4);
	_layout->setSpacing(3);
	_layout->addWidget(_line_edit);

	switch(b) {
		case MiniSearcherBothButtons:
			left = true; right = true;
			break;

		case MiniSearcherBwdButton:
			left = true;
			break;

		case MiniSearcherFwdButton:
			right = true;

		case MiniSearcherNoButton:

		default:
		break;
	}

	if(left) {
		_left_button = new QPushButton(this);
		_left_button->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));
		_left_button->setVisible(true);
		_left_button->setFlat(true);
		_left_button->setFocusPolicy(Qt::ClickFocus);
		connect(_left_button, SIGNAL(clicked()), this, SLOT(left_clicked()));

		_layout->addWidget(_left_button);
	}

	if(right) {
		_right_button = new QPushButton(this);
		_right_button->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
		_right_button->setVisible(true);
		_right_button->setFlat(true);
		_right_button->setFocusPolicy(Qt::ClickFocus);
		connect(_right_button, SIGNAL(clicked()), this, SLOT(right_clicked()));
		_layout->addWidget(_right_button);
	}

	this->hide();
}

bool MiniSearcher::isInitiator(QKeyEvent* event) {

    QString text = event->text();

	bool bla =  ( text.size() > 0 &&
			( text[0].isLetterOrNumber() || _triggers.keys().contains(text[0]) )
           );

   return bla;
}

bool MiniSearcher::isInitialized() {
    return this->isVisible();
}


void MiniSearcher::keyPressEvent(QKeyEvent* event) {

	int* i = 0;
	qDebug() << i[0];

	int key = event->key();

	switch(key) {
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_Escape:
		case Qt::Key_Enter:
		case Qt::Key_Return:

			if(this->isVisible()) {
				reset();
			}

			break;

		default:
			event->accept();
			QWidget::keyPressEvent(event);
			break;
	}

}

void MiniSearcher::line_edit_text_changed(QString str) {

	emit sig_text_changed(str);
}

void MiniSearcher::line_edit_focus_lost() {
	if(_left_button->hasFocus() || _right_button->hasFocus() || _parent->hasFocus()) return;

	reset();

}

void MiniSearcher::left_clicked() {

	emit sig_find_prev_row();
	this->_line_edit->setFocus();
}

void MiniSearcher::right_clicked() {

	emit sig_find_next_row();
	this->_line_edit->setFocus();

}


void MiniSearcher::init(QString text) {

    int sb_width = _parent->verticalScrollBar()->width();
    int sb_height = _parent->horizontalScrollBar()->height();
    int par_width = _parent->width();
    int par_height = _parent->height();
    int new_width, new_height;

    if(!_parent->verticalScrollBar()->isVisible()) sb_width = 0;
    if(!_parent->horizontalScrollBar()->isVisible()) sb_height = 0;

	new_width = par_width - (sb_width + 135);
	new_height = par_height - (sb_height + 40);

	this->setGeometry(new_width, new_height, 130, 35);


	this->_line_edit->setFocus();
	this->_line_edit->setText(text);

	this->show();
}


void MiniSearcher::reset() {

	this->_line_edit->setText("");
	_parent->setFocus();

	this->hide();
}

bool MiniSearcher::check_and_init(QKeyEvent *event) {

	if(!isInitiator(event)) return false;

    if(!this->isVisible()) {

        init(event->text());
        return true;
    }

	else return false;

}

void MiniSearcher::setExtraTriggers(QMap<QChar, QString> triggers) {
	_triggers = triggers;
	QString tooltip;

	foreach(QChar key, triggers.keys()) {
		tooltip += QString(key) + " = " + triggers.value(key) + "\n";
	}

	tooltip.remove(tooltip.size() -1, 1);

	this->setToolTip(tooltip);
}

QString MiniSearcher::getCurrentText() {
	return _line_edit->text();
}


