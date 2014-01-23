#include "MiniSearcher.h"
#include <QString>
#include <QScrollBar>

MiniSearcher::MiniSearcher(SearchableTableView *parent) :
    QLineEdit(parent)
{
    _parent = parent;
    _initialized = false;
    this->hide();

    connect(_parent, SIGNAL(sig_mouse_pressed()), this, SLOT(reset()));
    connect(_parent, SIGNAL(sig_mouse_moved()), this, SLOT(reset()));
    connect(_parent, SIGNAL(sig_mouse_released()), this, SLOT(reset()));
    connect(_parent, SIGNAL(sig_key_pressed(QKeyEvent*)), this, SLOT(key_pressed(QKeyEvent*)));


}

bool MiniSearcher::is_initiator(QKeyEvent* event){

    QString text = event->text();
    return (text.size() > 0 && text[0].isLetterOrNumber());
}

void MiniSearcher::key_pressed(QKeyEvent* event){

    int key = event->key();
    QString text = event->text();

    if(!_initialized) {
        if(is_initiator(event)){
            init(text);
        }
    }

    else{

        switch(key){
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:

                if(this->isVisible())
                    reset();
                break;

            default:
                break;
        }
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
    _initialized = true;

}


void MiniSearcher::reset(){

    this->setText("");
    this->hide();
    _parent->setFocus();
    _initialized = false;

}
