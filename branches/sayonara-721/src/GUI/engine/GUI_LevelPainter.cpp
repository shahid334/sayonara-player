#include "GUI_LevelPainter.h"
#include <QPainter>
#include <QBrush>
#include <QDebug>


float borders[] = {-45.0f, -39.0f,  // green
                   -35.0f, -32.0f,  // green
                   -28.0f, -24.5f,  // green
                   -21.5f, -18.5f,   // green
                   -16.5f, -14.5f,    // green
                   -12.5f, -10.5f,    // yellow
                   -9.0f,  -7.5f,    // yellow
                    -6.0f,  -4.75f,    // yellow
                    -3.5f,  -2.5f,    // red
                    -1.5f,  -0.5f};   // red

GUI_LevelPainter::GUI_LevelPainter(QString name, QString action_text, QWidget *parent) :
    PlayerPlugin(name, action_text, parent)
{
    ui = new Ui::GUI_LevelPainter();
    ui->setupUi(this);


    _red = QColor(216, 0, 0);
    _red_dark = QColor(50, 0, 0);

    _blue = QColor(0, 0, 255);

    _green = QColor(0, 216, 0);
    _green_dark = QColor(0, 50, 0);

    _yellow = QColor(216, 216, 0);
    _yellow_dark = QColor(50, 50, 0);

    _white = QColor(255, 255, 255);
    _black = QColor(20, 20, 20);
}


void GUI_LevelPainter::set_level(float level_l, float level_r){

    _level_l_old = _level_l;
    _level_r_old = _level_r;

    _level_l = level_l;
    _level_r = level_r;

    if(_level_l > -8.0) _col_l = _red;
    else if(_level_l > -20.0f) _col_l = _yellow;
    else _col_l = _green;

    if(_level_r > -8.0) _col_r = _red;
    else if(_level_r > -20.0f) _col_r = _yellow;
    else _col_r = _green;

    this->update();
}

int GUI_LevelPainter::get_last_bright_light(float level){

    int i;
    for(i=0; i<20; i++){
        if(level < borders[i]){
            i--;
            break;
        }
    }

    return i;
}

void GUI_LevelPainter::paintEvent(QPaintEvent* e){

    QPainter painter(this);
    int width = this->width();

    int rect_width = (width-20) / 20 - 3;

    QColor col;
    int y = 20;
    int level = _level_l;

    for(int c=0; c<2; c++){

        int border = 0;
        for(int i=0; i<20; i++){

            QRect rect(10 + border, y, rect_width , 6);

            if(level > borders[i]){
                if(i < 10) col = _green;
                else if(i < 15) col = _yellow;
                else if(i < 20) col = _red;
            }

            else{
                if(i < 10) col = _green_dark;
                else if(i < 15) col = _yellow_dark;
                else if(i < 20) col = _red_dark;
            }




            painter.fillRect(rect, col);

            border += rect_width + 3;
        }

        level = _level_r;
        y = 29;
    }
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
