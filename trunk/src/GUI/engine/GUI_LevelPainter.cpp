#include "GUI_LevelPainter.h"
#include <QPainter>
#include <QBrush>
#include <QDebug>

GUI_LevelPainter::GUI_LevelPainter(QString name, QString action_text, QWidget *parent) :
    PlayerPlugin(name, action_text, parent)
{
    ui = new Ui::GUI_LevelPainter();
    ui->setupUi(this);

    _white = QColor(255, 255, 255);
    _red = QColor(255, 0, 0);
    _blue = QColor(0, 0, 255);
    _green = QColor(0, 255, 0);
    _yellow = QColor(255, 255, 0);
    _black = QColor(20, 20, 20);
}


void GUI_LevelPainter::set_level(float level_l, float level_r){
    _level_l = level_l + 4.0f;
    _level_r = level_r + 4.0f;

    if(_level_l > -8.0) _col_l = _red;
    else if(_level_l > -20.0f) _col_l = _yellow;
    else _col_l = _green;

    if(_level_r > -8.0) _col_r = _red;
    else if(_level_r > -20.0f) _col_r = _yellow;
    else _col_r = _green;



    this->update();
}

void GUI_LevelPainter::paintEvent(QPaintEvent* e){

    QPainter painter(this);
    int width = this->width();

    int all_width_l = (int ) (((_level_l + 50) * width) / 50);
    int all_width_r = (int ) (((_level_r + 50) * width) / 50);

    QRect rect_l(10, 20, all_width_l , 6);
    QRect rect_r(10, 29, all_width_r , 6);

    QRect rect_l_b(10 + rect_l.width(), 20, width - rect_l.width(), 6);
    QRect rect_r_b(10 + rect_r.width(), 29, width - rect_r.width(), 6);



    painter.fillRect(rect_l, _col_l);
    painter.fillRect(rect_r, _col_r);

    painter.fillRect(rect_l_b, _black);
    painter.fillRect(rect_r_b, _black);

}

QAction* GUI_LevelPainter::getAction(){
    PlayerPlugin::calc_action(this->getVisName());
    return _pp_action;
}


void GUI_LevelPainter::showEvent(QShowEvent * e){
    Q_UNUSED(e);
    emit sig_show(true);
}

void GUI_LevelPainter::closeEvent(QCloseEvent *e){
    PlayerPlugin::closeEvent(e);
    emit sig_show(false);
}
