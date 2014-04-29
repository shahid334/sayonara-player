/* RatingLabel.h */

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



#ifndef RATINGLABEL_H
#define RATINGLABEL_H

#include <QLabel>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QPalette>
#include <QPainter>
#include <QRect>
#include <QPoint>

class Rating {

public:
    Rating();
    Rating(int rating);


    void paint(QPainter *painter, const QRect &rect,
                   const QPalette &palette) const;

     void set_rating(int rating);
     int get_rating();

private:
     int _rating;


};

Q_DECLARE_METATYPE(Rating)

class RatingLabel : public QLabel
{
    Q_OBJECT

signals:
    void sig_finished(bool);


public:
    RatingLabel(QWidget *parent = 0);
    virtual ~RatingLabel();
    

    void paintEvent(QPaintEvent *e);
    void focusOutEvent(QFocusEvent* e);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent* ev);
    void mouseMoveEvent(QMouseEvent *ev);

    void set_rating(Rating rating);
    Rating get_rating();

    void kill_yourself();

    void increase();
    void decrease();

    int get_id();

private:

   int _id;
   int _row;
   Rating _rating;
   void update_rating(int rating);
   int calc_rating(QPoint pos);
   QWidget* _parent;
};



#endif // RATINGLABEL_H
