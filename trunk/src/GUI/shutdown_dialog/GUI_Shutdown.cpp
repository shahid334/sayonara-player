#include "GUI/shutdown_dialog/GUI_Shutdown.h"
#include "HelperStructs/Shutdown/Shutdown.h"

GUI_Shutdown::GUI_Shutdown(QWidget* parent):
	SayonaraDialog(parent),
	Ui::GUI_Shutdown()
{
	setupUi(this);

	connect(btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));
	connect(btn_cancel, SIGNAL(clicked()), this, SLOT(cancel_clicked()));
	connect(rb_after_finished, SIGNAL(clicked(bool)), this, SLOT(rb_after_finished_clicked(bool)));
	connect(rb_after_minutes, SIGNAL(clicked(bool)), this, SLOT(rb_after_minutes_clicked(bool)));
}


void GUI_Shutdown::ok_clicked(){

	if(sb_minutes->isEnabled()){
		quint64 msec = sb_minutes->value() * 60 * 1000;
		Shutdown::getInstance()->shutdown(msec);
	}

	else {
		Shutdown::getInstance()->shutdown_after_end();
	}


	close();
	emit sig_closed();
}

void GUI_Shutdown::cancel_clicked(){
	close();
	emit sig_closed();
}

void GUI_Shutdown::rb_after_finished_clicked(bool b){
	rb_after_minutes->setChecked(false);
	sb_minutes->setEnabled(false);
}

void GUI_Shutdown::rb_after_minutes_clicked(bool b){
	rb_after_minutes->setChecked(false);
	sb_minutes->setEnabled(true);
}
