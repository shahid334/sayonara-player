#include "GUI_Speed.h"

GUI_Speed::GUI_Speed(QString name, QWidget *parent) :
	PlayerPlugin(name, parent)
{
	ui = new Ui::GUI_Speed();
	ui->setupUi(this);

	connect(ui->sli_speed, SIGNAL(sliderMoved(int)), this, SLOT(slider_changed(int)));
	connect(ui->cb_active, SIGNAL(toggled(bool)), this, SLOT(active_changed(bool)));
}


void GUI_Speed::slider_changed(int val){
	float val_f = val / 100.0f;
	ui->lab_speed->setText(QString::number(val_f, 'f', 2));
	emit sig_speed_changed(val_f);
}


void GUI_Speed::active_changed(bool b){

	if(!b){
		emit sig_speed_changed(-1.0f);
	}

	else {
		emit sig_speed_changed( ui->sli_speed->value() / 100.0f );
	}
}
