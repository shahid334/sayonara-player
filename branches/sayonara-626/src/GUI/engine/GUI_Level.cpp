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
                    -6.0f,  -5.0f,    // yellow
                    -4.0f,  -3.0f,    // red
                    -2.0f,  -1.0f};   // red

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

  /*  _timer = new QTimer(this);
    _timer->setInterval(40);
    connect(_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    _timer->start();*/

    ui->lab_l_0 ->setToolTip(QString::number((double) borders[0])  + " db");
    ui->lab_l_05->setToolTip(QString::number((double) borders[1])  + " db");
    ui->lab_l_1 ->setToolTip(QString::number((double) borders[2])  + " db");
    ui->lab_l_15->setToolTip(QString::number((double) borders[3])  + " db");
    ui->lab_l_2 ->setToolTip(QString::number((double) borders[4])  + " db");
    ui->lab_l_25->setToolTip(QString::number((double) borders[5])  + " db");
    ui->lab_l_3 ->setToolTip(QString::number((double) borders[6])  + " db");
    ui->lab_l_35->setToolTip(QString::number((double) borders[7])  + " db");
    ui->lab_l_4 ->setToolTip(QString::number((double) borders[8])  + " db");
    ui->lab_l_45->setToolTip(QString::number((double) borders[9])  + " db");
    ui->lab_l_5 ->setToolTip(QString::number((double) borders[10]) + " db");
    ui->lab_l_55->setToolTip(QString::number((double) borders[11]) + " db");
    ui->lab_l_6 ->setToolTip(QString::number((double) borders[12]) + " db");
    ui->lab_l_65->setToolTip(QString::number((double) borders[13]) + " db");
    ui->lab_l_7 ->setToolTip(QString::number((double) borders[14]) + " db");
    ui->lab_l_75->setToolTip(QString::number((double) borders[15]) + " db");
    ui->lab_l_8 ->setToolTip(QString::number((double) borders[16]) + " db");
    ui->lab_l_85->setToolTip(QString::number((double) borders[17]) + " db");
    ui->lab_l_9 ->setToolTip(QString::number((double) borders[18]) + " db");
    ui->lab_l_95->setToolTip(QString::number((double) borders[19]) + " db");

    ui->lab_r_0 ->setToolTip(QString::number((double) borders[0])  + " db");
    ui->lab_r_05->setToolTip(QString::number((double) borders[1])  + " db");
    ui->lab_r_1 ->setToolTip(QString::number((double) borders[2])  + " db");
    ui->lab_r_15->setToolTip(QString::number((double) borders[3])  + " db");
    ui->lab_r_2 ->setToolTip(QString::number((double) borders[4])  + " db");
    ui->lab_r_25->setToolTip(QString::number((double) borders[5])  + " db");
    ui->lab_r_3 ->setToolTip(QString::number((double) borders[6])  + " db");
    ui->lab_r_35->setToolTip(QString::number((double) borders[7])  + " db");
    ui->lab_r_4 ->setToolTip(QString::number((double) borders[8])  + " db");
    ui->lab_r_45->setToolTip(QString::number((double) borders[9])  + " db");
    ui->lab_r_5 ->setToolTip(QString::number((double) borders[10]) + " db");
    ui->lab_r_55->setToolTip(QString::number((double) borders[11]) + " db");
    ui->lab_r_6 ->setToolTip(QString::number((double) borders[12]) + " db");
    ui->lab_r_65->setToolTip(QString::number((double) borders[13]) + " db");
    ui->lab_r_7 ->setToolTip(QString::number((double) borders[14]) + " db");
    ui->lab_r_75->setToolTip(QString::number((double) borders[15]) + " db");
    ui->lab_r_8 ->setToolTip(QString::number((double) borders[16]) + " db");
    ui->lab_r_85->setToolTip(QString::number((double) borders[17]) + " db");
    ui->lab_r_9 ->setToolTip(QString::number((double) borders[18]) + " db");
    ui->lab_r_95->setToolTip(QString::number((double) borders[19]) + " db");
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

    float max_l = _get_max(_levels_l, _buffer_size);
    float max_r = _get_max(_levels_r, _buffer_size);

    bool show_max = false;

    this->ui->lab_l_0 ->setEnabled(level_l > borders[0]   || (max_l > borders[0]  && max_l < borders[1]  && show_max));
    this->ui->lab_l_05->setEnabled(level_l > borders[1]   || (max_l > borders[1]  && max_l < borders[2]  && show_max));
    this->ui->lab_l_1 ->setEnabled(level_l > borders[2]   || (max_l > borders[2]  && max_l < borders[3]  && show_max));
    this->ui->lab_l_15->setEnabled(level_l > borders[3]   || (max_l > borders[3]  && max_l < borders[4]  && show_max));
    this->ui->lab_l_2 ->setEnabled(level_l > borders[4]   || (max_l > borders[4]  && max_l < borders[5]  && show_max));
    this->ui->lab_l_25->setEnabled(level_l > borders[5]   || (max_l > borders[5]  && max_l < borders[6]  && show_max));
    this->ui->lab_l_3 ->setEnabled(level_l > borders[6]   || (max_l > borders[6]  && max_l < borders[7]  && show_max));
    this->ui->lab_l_35->setEnabled(level_l > borders[7]   || (max_l > borders[7]  && max_l < borders[8]  && show_max));
    this->ui->lab_l_4 ->setEnabled(level_l > borders[8]   || (max_l > borders[8]  && max_l < borders[9]  && show_max));
    this->ui->lab_l_45->setEnabled(level_l > borders[9]   || (max_l > borders[9]  && max_l < borders[10] && show_max));
    this->ui->lab_l_5 ->setEnabled(level_l > borders[10]  || (max_l > borders[10] && max_l < borders[11] && show_max));
    this->ui->lab_l_55->setEnabled(level_l > borders[11]  || (max_l > borders[11] && max_l < borders[12] && show_max));
    this->ui->lab_l_6 ->setEnabled(level_l > borders[12]  || (max_l > borders[12] && max_l < borders[13] && show_max));
    this->ui->lab_l_65->setEnabled(level_l > borders[13]  || (max_l > borders[13] && max_l < borders[14] && show_max));
    this->ui->lab_l_7 ->setEnabled(level_l > borders[14]  || (max_l > borders[14] && max_l < borders[15] && show_max));
    this->ui->lab_l_75->setEnabled(level_l > borders[15]  || (max_l > borders[15] && max_l < borders[16] && show_max));
    this->ui->lab_l_8 ->setEnabled(level_l > borders[16]  || (max_l > borders[16] && max_l < borders[17] && show_max));
    this->ui->lab_l_85->setEnabled(level_l > borders[17]  || (max_l > borders[17] && max_l < borders[18] && show_max));
    this->ui->lab_l_9 ->setEnabled(level_l > borders[18]  || (max_l > borders[18] && max_l < borders[19] && show_max));
    this->ui->lab_l_95->setEnabled(level_l > borders[19]  || (max_l > borders[19] && show_max));

    this->ui->lab_r_0 ->setEnabled(level_r > borders[0]   || (max_r > borders[0]  && max_r < borders[1]  && show_max));
    this->ui->lab_r_05->setEnabled(level_r > borders[1]   || (max_r > borders[1]  && max_r < borders[2]  && show_max));
    this->ui->lab_r_1 ->setEnabled(level_r > borders[2]   || (max_r > borders[2]  && max_r < borders[3]  && show_max));
    this->ui->lab_r_15->setEnabled(level_r > borders[3]   || (max_r > borders[3]  && max_r < borders[4]  && show_max));
    this->ui->lab_r_2 ->setEnabled(level_r > borders[4]   || (max_r > borders[4]  && max_r < borders[5]  && show_max));
    this->ui->lab_r_25->setEnabled(level_r > borders[5]   || (max_r > borders[5]  && max_r < borders[6]  && show_max));
    this->ui->lab_r_3 ->setEnabled(level_r > borders[6]   || (max_r > borders[6]  && max_r < borders[7]  && show_max));
    this->ui->lab_r_35->setEnabled(level_r > borders[7]   || (max_r > borders[7]  && max_r < borders[8]  && show_max));
    this->ui->lab_r_4 ->setEnabled(level_r > borders[8]   || (max_r > borders[8]  && max_r < borders[9]  && show_max));
    this->ui->lab_r_45->setEnabled(level_r > borders[9]   || (max_r > borders[9]  && max_r < borders[10] && show_max));
    this->ui->lab_r_5 ->setEnabled(level_r > borders[10]  || (max_r > borders[10] && max_r < borders[11] && show_max));
    this->ui->lab_r_55->setEnabled(level_r > borders[11]  || (max_r > borders[11] && max_r < borders[12] && show_max));
    this->ui->lab_r_6 ->setEnabled(level_r > borders[12]  || (max_r > borders[12] && max_r < borders[13] && show_max));
    this->ui->lab_r_65->setEnabled(level_r > borders[13]  || (max_r > borders[13] && max_r < borders[14] && show_max));
    this->ui->lab_r_7 ->setEnabled(level_r > borders[14]  || (max_r > borders[14] && max_r < borders[15] && show_max));
    this->ui->lab_r_75->setEnabled(level_r > borders[15]  || (max_r > borders[15] && max_r < borders[16] && show_max));
    this->ui->lab_r_8 ->setEnabled(level_r > borders[16]  || (max_r > borders[16] && max_r < borders[17] && show_max));
    this->ui->lab_r_85->setEnabled(level_r > borders[17]  || (max_r > borders[17] && max_r < borders[18] && show_max));
    this->ui->lab_r_9 ->setEnabled(level_r > borders[18]  || (max_r > borders[18] && max_r < borders[19] && show_max));
    this->ui->lab_r_95->setEnabled(level_r > borders[19]  || (max_r > borders[19] && show_max));

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
