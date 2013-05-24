#include "GUI_Level.h"
#include <QLabel>

float borders[] = {-30.0f, -27.5f,
                   -25.0f, -18.5f,
                   -15.0f, -12.5f,
                   -10.0f, -9.5f,
                   -9.0f, -8.5f,
                   -8.0f, -7.5f,
                   -7.0f, -6.5f,
                   -6.0f, -5.5f,
                   -5.0f, -4.5f,
                   -4.0f, -3.5f};

GUI_Level::GUI_Level(QString name, QString action_name, QWidget* parent) : PlayerPlugin(name, action_name, parent)

{
    ui = new Ui::GUI_Level();
    ui->setupUi(this);

    _buffer_size = 3;
    _cur_idx = 0;

    _levels_l = new float[_buffer_size];
    _levels_r = new float[_buffer_size];
    for(int i=0; i<_buffer_size; i++){
        _levels_l[i] = -100.0f;
        _levels_r[i] = -100.0f;
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

    _levels_l[_cur_idx] = level_l;
    _levels_r[_cur_idx] = level_r;


    level_l = _get_max(_levels_l, _buffer_size);
    level_r = _get_max(_levels_r, _buffer_size);


    this->ui->lab_l_0->setEnabled(level_l > borders[0]);
    this->ui->lab_l_05->setEnabled(level_l > borders[1]);
    this->ui->lab_l_1->setEnabled(level_l > borders[2]);
    this->ui->lab_l_15->setEnabled(level_l > borders[3]);
    this->ui->lab_l_2->setEnabled(level_l > borders[4]);
    this->ui->lab_l_25->setEnabled(level_l > borders[5]);
    this->ui->lab_l_3->setEnabled(level_l > borders[6]);
    this->ui->lab_l_35->setEnabled(level_l > borders[7]);
    this->ui->lab_l_4->setEnabled(level_l > borders[8]);
    this->ui->lab_l_45->setEnabled(level_l > borders[9]);
    this->ui->lab_l_5->setEnabled(level_l > borders[10]);
    this->ui->lab_l_55->setEnabled(level_l > borders[11]);
    this->ui->lab_l_6->setEnabled(level_l > borders[12]);
    this->ui->lab_l_65->setEnabled(level_l > borders[13]);
    this->ui->lab_l_7->setEnabled(level_l > borders[14]);
    this->ui->lab_l_75->setEnabled(level_l > borders[15]);
    this->ui->lab_l_8->setEnabled(level_l > borders[16]);
    this->ui->lab_l_85->setEnabled(level_l > borders[17]);
    this->ui->lab_l_9->setEnabled(level_l > borders[18]);
    this->ui->lab_l_95->setEnabled(level_l > borders[19]);


    this->ui->lab_r_0->setEnabled(level_r > borders[0]);
    this->ui->lab_r_05->setEnabled(level_r > borders[1]);
    this->ui->lab_r_1->setEnabled(level_r > borders[2]);
    this->ui->lab_r_15->setEnabled(level_r > borders[3]);
    this->ui->lab_r_2->setEnabled(level_r > borders[4]);
    this->ui->lab_r_25->setEnabled(level_r > borders[5]);
    this->ui->lab_r_3->setEnabled(level_r > borders[6]);
    this->ui->lab_r_35->setEnabled(level_r > borders[7]);
    this->ui->lab_r_4->setEnabled(level_r > borders[8]);
    this->ui->lab_r_45->setEnabled(level_r > borders[9]);
    this->ui->lab_r_5->setEnabled(level_r > borders[10]);
    this->ui->lab_r_55->setEnabled(level_r > borders[11]);
    this->ui->lab_r_6->setEnabled(level_r > borders[12]);
    this->ui->lab_r_65->setEnabled(level_r > borders[13]);
    this->ui->lab_r_7->setEnabled(level_r > borders[14]);
    this->ui->lab_r_75->setEnabled(level_r > borders[15]);
    this->ui->lab_r_8->setEnabled(level_r > borders[16]);
    this->ui->lab_r_85->setEnabled(level_r > borders[17]);
    this->ui->lab_r_9->setEnabled(level_r > borders[18]);
    this->ui->lab_r_95->setEnabled(level_r > borders[19]);

    _cur_idx ++;
    if(_cur_idx == _buffer_size) _cur_idx = 0;

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
    Q_UNUSED(e);
    emit sig_show(false);
}
