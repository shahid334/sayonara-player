#include "MiniSearcher.h"
#include <QString>
#include <QScrollBar>
#include <QDebug>

MiniSearcher::MiniSearcher(SearchableTableView *parent) :
    QLineEdit(parent)
{
    _parent = parent;
    this->hide();

    connect(parent, SIGNAL(sig_mouse_pressed()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_mouse_moved()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_mouse_released()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_focus_out()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_key_pressed(QKeyEvent*)), this, SLOT(key_pressed(QKeyEvent*)));
}

MiniSearcher::MiniSearcher(SearchableListView *parent) : QLineEdit(parent)
{
    _parent = parent;
    this->hide();

    connect(parent, SIGNAL(sig_mouse_pressed()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_mouse_moved()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_mouse_released()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_focus_out()), this, SLOT(reset()));
    connect(parent, SIGNAL(sig_key_pressed(QKeyEvent*)), this, SLOT(key_pressed(QKeyEvent*)));

}

bool MiniSearcher::isInitiator(QKeyEvent* event){

    QString text = event->text();

    qDebug() << text << ", " << _triggers;

   bool bla =  ( text.size() > 0 &&
            ( text[0].isLetterOrNumber() || _triggers.contains(text[0]) )
           );

   qDebug() << "Bla = " << bla << ", " << _triggers.contains(text[0]);


   return bla;
}

bool MiniSearcher::isInitialized(){
    return this->isVisible();
}

void MiniSearcher::focusOutEvent(QFocusEvent* event){

    //reset();

}

void MiniSearcher::keyPressEvent(QKeyEvent* event){

    int key = event->key();

        switch(key){
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:

                if(this->isVisible()){
                    reset();
                }

                break;

            default:
                QLineEdit::keyPressEvent(event);
                break;
        }

}


void MiniSearcher::init(QString text){

    int sb_width = _parent->verticalScrollBar()->width();
    int sb_height = _parent->horizontalScrollBar()->height();
    int par_width = _parent->width();
    int par_height = _parent->height();
    int new_width, new_height;

    if(!_parent->verticalScrollBar()->isVisible()) sb_width = 0;
    if(!_parent->horizontalScrollBar()->isVisible()) sb_height = 0;

    new_width = par_width - (sb_width + 105);
    new_height = par_height - (sb_height + 30);

    this->setGeometry(new_width, new_height, 100, 25);
    this->setFocus();
    this->setText(text);
    this->show();
}


void MiniSearcher::reset(){

    this->setText("");
    _parent->setFocus();

    this->hide();
}

bool MiniSearcher::check_and_init(QKeyEvent *event){
    if(!isInitiator(event)) return false;

    if(!this->isVisible()){

        init(event->text());
        return true;
    }

    else return false;

}

 void MiniSearcher::register_extra_keys(QList<QChar> keys){


     foreach(QChar key, keys){
         if(!_triggers.contains(key)) _triggers << key;
     }

 }
