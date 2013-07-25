#include "GUI_Spectrum.h"
#include <QPainter>
#include <QDebug>


GUI_Spectrum::GUI_Spectrum(QString name, QString action_text, QWidget *parent) :
    PlayerPlugin(name, action_text, parent)
{
    ui = new Ui::GUI_Spectrum();
    ui->setupUi(this);

}

void
GUI_Spectrum::set_spectrum(QList<float>& lst){

    _spec = lst;
    this->update();

}

void
GUI_Spectrum::paintEvent(QPaintEvent *e){
     QPainter painter(this);

    int x=10;
    int ninety = (_spec.size() * 900) / 1000;
    if(ninety == 0) return;
    int w = ((this->width() -20) / ninety) -2;


    for(int i=0; i<ninety; i++){

        float f = _spec[i];

        int h = (f + 80) * (this->height() - 20) / 80.0;
        QRect rect(x, this->height() - h, w, h);
        painter.fillRect(rect, QColor(255, 0, 0));

        x += w + 2;

    }



}


QAction* GUI_Spectrum::getAction(){
    PlayerPlugin::calc_action(this->getVisName());
    return _pp_action;
}


void GUI_Spectrum::showEvent(QShowEvent * e){
    Q_UNUSED(e);
    emit sig_show(true);
}

void GUI_Spectrum::closeEvent(QCloseEvent *e){
    PlayerPlugin::closeEvent(e);
    emit sig_show(false);
}
