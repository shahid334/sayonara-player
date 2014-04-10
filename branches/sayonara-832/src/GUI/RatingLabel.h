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
};



#endif // RATINGLABEL_H
