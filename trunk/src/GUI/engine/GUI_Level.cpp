#include "GUI_Level.h"
#include <QLabel>
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

QLabel* labels_l[20];
QLabel* labels_r[20];

GUI_Level::GUI_Level(QString name, QString action_name, QWidget* parent) : PlayerPlugin(name, action_name, parent)

{
    ui = new Ui::GUI_Level();
    ui->setupUi(this);

    _buffer_size = 1;
    _cur_idx = 0;

    _levels_l = new float[_buffer_size];
    _levels_r = new float[_buffer_size];
    for(int i=0; i<_buffer_size; i++){
        _levels_l[i] = -100.0f;
        _levels_r[i] = -100.0f;
    }

    _queue_it = false;

    QString style_green = "QLabel{border-radius: 1px;}QLabel::disabled{background: rgb(0, 50, 0);}QLabel::enabled{	background: rgb(0, 216, 0);}";
    QString style_yellow = "QLabel{border-radius: 1px;}QLabel::disabled{background: rgb(50, 50, 0);}QLabel::enabled{	background: rgb(216, 216, 0);}";
    QString style_red = "QLabel{border-radius: 1px;}QLabel::disabled{background: rgb(50, 0, 0);}QLabel::enabled{	background: rgb(192, 0, 0);}";


    labels_l[0] = ui->lab_l_0;
    labels_l[1] = ui->lab_l_05;
    labels_l[2] = ui->lab_l_1;
    labels_l[3] = ui->lab_l_15;
    labels_l[4] = ui->lab_l_2;
    labels_l[5] = ui->lab_l_25;
    labels_l[6] = ui->lab_l_3;
    labels_l[7] = ui->lab_l_35;
    labels_l[8] = ui->lab_l_4;
    labels_l[9] = ui->lab_l_45;
    labels_l[10] = ui->lab_l_5;
    labels_l[11] = ui->lab_l_55;
    labels_l[12] = ui->lab_l_6;
    labels_l[13] = ui->lab_l_65;
    labels_l[14] = ui->lab_l_7;
    labels_l[15] = ui->lab_l_75;
    labels_l[16] = ui->lab_l_8;
    labels_l[17] = ui->lab_l_85;
    labels_l[18] = ui->lab_l_9;
    labels_l[19] = ui->lab_l_95;

    labels_r[0] = ui->lab_r_0;
    labels_r[1] = ui->lab_r_05;
    labels_r[2] = ui->lab_r_1;
    labels_r[3] = ui->lab_r_15;
    labels_r[4] = ui->lab_r_2;
    labels_r[5] = ui->lab_r_25;
    labels_r[6] = ui->lab_r_3;
    labels_r[7] = ui->lab_r_35;
    labels_r[8] = ui->lab_r_4;
    labels_r[9] = ui->lab_r_45;
    labels_r[10] = ui->lab_r_5;
    labels_r[11] = ui->lab_r_55;
    labels_r[12] = ui->lab_r_6;
    labels_r[13] = ui->lab_r_65;
    labels_r[14] = ui->lab_r_7;
    labels_r[15] = ui->lab_r_75;
    labels_r[16] = ui->lab_r_8;
    labels_r[17] = ui->lab_r_85;
    labels_r[18] = ui->lab_r_9;
    labels_r[19] = ui->lab_r_95;

    for(int i=0; i<20; i++){
        if(i < 10){

            labels_l[i]->setStyleSheet(style_green);
            labels_r[i]->setStyleSheet(style_green);
        }
        else if(i< 16){
            labels_l[i]->setStyleSheet(style_yellow);
            labels_r[i]->setStyleSheet(style_yellow);

        }
        else {
            labels_l[i]->setStyleSheet(style_red);
            labels_r[i]->setStyleSheet(style_red);
        }

        labels_l[i]->setToolTip(QString::number((double) borders[i])  + " db");
        labels_r[i]->setToolTip(QString::number((double) borders[i])  + " db");
    }




}



GUI_Level::~GUI_Level(){
    delete _levels_l;
    delete _levels_r;
}


float _get_max(const float* l, int sz){
    float max = -1000000.0f;
    for(int i=0; i<sz; i++){
        if(l[i] > max) max = l[i];
    }

    return max;
}


void GUI_Level::set_level(float level_l, float level_r){

    if(_queue_it){
        QPair<float, float> pair;
        pair.first = level_l;
        pair.second = level_r;
        _queue.push_back(pair);
        return;
    }

    _levels_l[_cur_idx] = level_l;
    _levels_r[_cur_idx] = level_r;

 /*   float max_l = _get_max(_levels_l, _buffer_size);
    float max_r = _get_max(_levels_r, _buffer_size);

    bool show_max = false;*/

    for(int i=0; i<20; i++){
        labels_l[i]->setEnabled(level_l > borders[i]   /*|| (max_l > borders[0]  && max_l < borders[1]  && show_max)*/);
        labels_r[i]->setEnabled(level_r > borders[i]   /*|| (max_r > borders[0]  && max_r < borders[1]  && show_max)*/);

        labels_l[i]->update();
        labels_r[i]->update();
    }


   // _queue_it = true;
    _cur_idx ++;
    if(_cur_idx == _buffer_size) _cur_idx = 0;

}

void GUI_Level::timeout(){
    qDebug() << "Queue size = " << _queue.size();
    _queue_it = false;
    if(_queue.size() > 0){
        set_level(_queue[0].first, _queue[0].second);
        _queue.removeFirst();
    }
}

QAction* GUI_Level::getAction(){
    PlayerPlugin::calc_action(this->getVisName());
    return _pp_action;
}


void GUI_Level::showEvent(QShowEvent * e){
    Q_UNUSED(e);
    emit sig_show(true);
}

void GUI_Level::closeEvent(QCloseEvent *e){
    PlayerPlugin::closeEvent(e);
    emit sig_show(false);
}
