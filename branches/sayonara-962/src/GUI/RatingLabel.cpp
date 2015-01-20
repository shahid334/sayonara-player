/* RatingLabel.cpp */

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



#include "GUI/RatingLabel.h"
#include "HelperStructs/globals.h"
#include <QPainter>
#include <QRect>
#include <QColor>
#include <QDebug>


const int Offset = 3;


Rating::Rating() {
    _rating = 0;

}
Rating::Rating(int rating) {
    _rating = rating;

}

int Rating::get_rating() {

    return _rating;
}

void Rating::set_rating(int rating) {
    _rating = rating;
}


void Rating::paint(QPainter *painter, const QRect &rect,
               const QPalette &palette) const{


    QRect rating_rect;
    QColor col;
	int wrect = (rect.width() / 5);

    for(int rating = 0; rating < _rating; rating++) {

		rating_rect.setRect(5 + rect.x() + (wrect) * (rating),
							rect.y() + Offset * 2,
							wrect - Offset,
							rect.height() - Offset * 4);

        col = SAYONARA_ORANGE_COL;

		painter->setPen(QColor(50, 50, 50));

        painter->drawRect(rect);
		painter->fillRect(rating_rect, col);
    }

    for(int rating= _rating; rating < 5; rating ++) {

		rating_rect.setRect(5 + rect.x() + (wrect ) * (rating),
							rect.y() + Offset * 2,
							wrect - Offset,
							rect.height() - Offset * 4);

        col = QColor(50, 50, 50);

		painter->fillRect(rating_rect, col);
    }

}



RatingLabel::RatingLabel(QWidget *parent, bool enabled) :
    QLabel(parent)
{
	_enabled = enabled;
	_parent = parent;
    _rating = Rating(0);
    _id = rand();

    QSizePolicy p(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	this->setFocus();
    this->setSizePolicy(p);
	this->setMouseTracking(true);

}

RatingLabel::~RatingLabel() {
    _id = 0;
}

int RatingLabel::get_id() {

    return _id;
}


int RatingLabel::calc_rating(QPoint pos) {

	int rating = (pos.x() + Offset) / ((width() - 5) / 5);

    if(rating > 5) rating = 5;

    return rating;
}


void RatingLabel::paintEvent(QPaintEvent *e) {

    QPainter painter(this);
    _rating.paint(&painter, rect(), palette());
    this->setFocus();
}


void RatingLabel::mouseMoveEvent(QMouseEvent *e) {

	if(!_enabled) return;
	int rating = calc_rating(e->pos());
	this->update_rating(rating);
}


void RatingLabel::mousePressEvent(QMouseEvent *e) {


	if(!_enabled) return;

	this->setMouseTracking(true);
    int rating = calc_rating(e->pos());
	update_rating(rating);
}


void RatingLabel::mouseReleaseEvent(QMouseEvent *e) {

	if(!_enabled) return;
    emit sig_finished(true);
}


void RatingLabel::focusOutEvent(QFocusEvent* e) {

	_parent->setFocus();
	if(!_enabled) return;
	emit sig_finished(false);

}

void RatingLabel::update_rating(int rating) {
    _rating.set_rating(rating);
    update();
}

void RatingLabel::increase() {
    int rating = _rating.get_rating();
    if(rating < 5) rating++;
    update_rating(rating);
}

void RatingLabel::decrease() {
    int rating = _rating.get_rating();
    if(rating > 0) rating--;
    update_rating(rating);
}

void RatingLabel::set_rating(Rating rating) {

    _rating = rating;
    update();
}

Rating RatingLabel::get_rating() {

    return _rating;
}

void RatingLabel::kill_yourself() {
    emit sig_finished(false);
}

