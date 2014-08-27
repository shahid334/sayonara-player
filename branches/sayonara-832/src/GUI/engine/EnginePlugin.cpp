
#include "GUI/engine/EnginePlugin.h"

EnginePlugin::EnginePlugin(QString name, QWidget* parent) :
    PlayerPlugin(name, parent)
{
    _ecsc = new EngineColorStyleChooser(minimumWidth(), minimumHeight());

    _timer = new QTimer();
    _timer->setInterval(30);
    _timer_stopped = true;

    connect(_timer, SIGNAL(timeout()), this, SLOT(timed_out()));
}

EnginePlugin::~EnginePlugin(){
    disconnect(_btn_config, SIGNAL(clicked()), this, SLOT(config_clicked()));
    disconnect(_btn_prev, SIGNAL(clicked()), this, SLOT(prev_clicked()));
    disconnect(_btn_next, SIGNAL(clicked()), this, SLOT(next_clicked()));
    disconnect(_btn_close, SIGNAL(clicked()), this, SLOT(close()));

    delete _btn_config;
    delete _btn_prev;
    delete _btn_next;
    delete _btn_close;
}


void EnginePlugin::init_buttons(){
    _btn_config->setGeometry(10, 10, 20, 20);
    _btn_prev->setGeometry(35, 10, 20, 20);
    _btn_next->setGeometry(60, 10, 20, 20);
    _btn_close->setGeometry(85, 10, 20, 20);

    connect(_btn_config, SIGNAL(clicked()), this, SLOT(config_clicked()));
    connect(_btn_prev, SIGNAL(clicked()), this, SLOT(prev_clicked()));
    connect(_btn_next, SIGNAL(clicked()), this, SLOT(next_clicked()));
    connect(_btn_close, SIGNAL(clicked()), this, SLOT(close()));

    _btn_config->hide();
    _btn_prev->hide();
    _btn_next->hide();
    _btn_close->hide();
}



void EnginePlugin::psl_stop(){
    _timer->start();
    _timer_stopped = false;
}

void EnginePlugin::config_clicked(){
    emit sig_right_clicked(_cur_style_idx);
}

void EnginePlugin::next_clicked(){
    int n_styles = _ecsc->get_num_color_schemes();
    _cur_style_idx = (_cur_style_idx + 1) % n_styles;

    psl_style_update();
}


void EnginePlugin::prev_clicked(){
    int n_styles = _ecsc->get_num_color_schemes();
    _cur_style_idx = (_cur_style_idx - 1);
    if(_cur_style_idx < 0){
        _cur_style_idx = n_styles - 1;
    }

    psl_style_update();
}



void EnginePlugin::showEvent(QShowEvent *e){
    PlayerPlugin::showEvent(e);
    update();
    emit sig_show(true);
}


void EnginePlugin::closeEvent(QCloseEvent *e) {
    PlayerPlugin::closeEvent(e);
    update();
    emit sig_show(false);
}

void EnginePlugin::resizeEvent(QResizeEvent* e){
    psl_style_update();
}


void EnginePlugin::mousePressEvent(QMouseEvent *e) {

    switch(e->button()){
        case Qt::LeftButton:
            next_clicked();
            break;
        case Qt::MidButton:
            close();
            break;
        case Qt::RightButton:
            emit sig_right_clicked(_cur_style_idx);
        default:
            break;
    }
}


void EnginePlugin::enterEvent(QEvent* e){
    PlayerPlugin::enterEvent(e);
    _btn_config->show();
    _btn_prev->show();
    _btn_next->show();
    _btn_close->show();

}

void EnginePlugin::leaveEvent(QEvent* e){
    PlayerPlugin::leaveEvent(e);
    _btn_config->hide();
    _btn_prev->hide();
    _btn_next->hide();
    _btn_close->hide();
}

