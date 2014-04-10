#include "GUI/RatingLabel.h"
#include "HelperStructs/globals.h"
#include <QPainter>
#include <QRect>
#include <QColor>
#include <QDebug>

const int SRect = 3;
const int Offset = 3;


Rating::Rating(){
    _rating = 0;

}
Rating::Rating(int rating){
    _rating = rating;

}

int Rating::get_rating(){

    return _rating;
}

void Rating::set_rating(int rating){
    _rating = rating;
}


void Rating::paint(QPainter *painter, const QRect &rect,
               const QPalette &palette) const{


    QRect rating_rect;
    QColor col;
    int wrect = ((rect.width() - Offset) - SRect * SRect) / 5 - SRect;

    for(int rating = 0; rating < _rating; rating++){

        rating_rect.setRect(Offset + rect.x() + (wrect + SRect) * rating,
                            rect.y() + SRect * 2,
                            wrect,
                            rect.height() - SRect * 4);

        col = SAYONARA_ORANGE_COL;

        painter->setPen(QColor(50, 50, 50));

        painter->drawRect(rect);
        painter->fillRect(rating_rect, col);
    }

    for(int rating= _rating; rating < 5; rating ++){

        rating_rect.setRect(Offset + rect.x() + (wrect + SRect) * rating,
                            rect.y() + SRect * 2,
                            wrect,
                            rect.height() - SRect * 4);

        col = QColor(50, 50, 50);

        painter->fillRect(rating_rect, col);
    }

}



RatingLabel::RatingLabel(QWidget *parent) :
    QLabel(parent)
{
    _rating = Rating(0);
    _id = rand();
    qDebug() << "Create editor " << _id;

    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocusProxy(parent);

    QSizePolicy p(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->setSizePolicy(p);
    this->setMouseTracking(true);
}

RatingLabel::~RatingLabel(){
    qDebug() << "Destroy editor " << _id;
    _id = 0;
}

int RatingLabel::get_id(){

    return _id;
}


int RatingLabel::calc_rating(QPoint pos){

    int pos_x = pos.x();
    int wrect = (width() - Offset) / 5;
    int rating = 0;
    if(pos_x + Offset > wrect / 2 - 2)
        rating = (pos_x + Offset) / wrect + 1;

    if(rating > 5) rating = 5;
    return rating;

}


void RatingLabel::paintEvent(QPaintEvent *e){

    QPainter painter(this);
    _rating.paint(&painter, rect(), palette());
    this->setFocus();
}


void RatingLabel::mouseMoveEvent(QMouseEvent *e){

    int rating = calc_rating(e->pos());
    this->update_rating(rating);
}


void RatingLabel::mousePressEvent(QMouseEvent *e){

    int rating = calc_rating(e->pos());
    this->update_rating(rating);
}


void RatingLabel::mouseReleaseEvent(QMouseEvent *e){

    emit sig_finished(true);
}

void RatingLabel::focusOutEvent(QFocusEvent* e){

    emit sig_finished(false);

}

void RatingLabel::update_rating(int rating){
    _rating.set_rating(rating);
    update();
}

void RatingLabel::increase(){
    int rating = _rating.get_rating();
    if(rating < 5) rating++;
    update_rating(rating);
}

void RatingLabel::decrease(){
    int rating = _rating.get_rating();
    if(rating > 0) rating--;
    update_rating(rating);
}

void RatingLabel::set_rating(Rating rating){

    _rating = rating;
    update();
}

Rating RatingLabel::get_rating(){

    return _rating;
}

void RatingLabel::kill_yourself(){

    if(_id == 0) return;
    emit sig_finished(false);
}




