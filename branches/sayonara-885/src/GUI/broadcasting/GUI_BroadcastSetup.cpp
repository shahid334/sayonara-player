#include "GUI_BroadcastSetup.h"
#include "HelperStructs/Helper.h"


GUI_BroadcastSetup::GUI_BroadcastSetup(QWidget *parent) :
	QDialog(parent),
	Ui::GUI_BroadcastSetup()
{
	setupUi(this);
	lab_icon->setPixmap(Helper::getPixmap("broadcast.png", QSize(100, 100), true));

	_settings = Settings::getInstance();
}

void GUI_BroadcastSetup::accept(){

	bool new_active = cb_active->isChecked();
	bool new_prompt = cb_prompt->isChecked();
	int new_port = sb_port->value();

	bool old_active = _settings->get(Set::BroadCast_Active);
	bool old_prompt = _settings->get(Set::Broadcast_Prompt);
	int old_port = _settings->get(Set::Broadcast_Port);

	if(old_active != new_active){
		_settings->set(Set::BroadCast_Active, new_active);
		emit sig_active_changed(new_active);
	}

	if(old_prompt != new_prompt){
		_settings->set(Set::Broadcast_Prompt, new_prompt);
		emit sig_prompt_changed(new_prompt);
	}

	if(old_port != new_port){
		_settings->set(Set::Broadcast_Port, new_port);
		emit sig_port_changed(new_port);
	}

	QDialog::accept();
}

void GUI_BroadcastSetup::reject(){
	QDialog::reject();
}

void GUI_BroadcastSetup::show(){

	cb_active->setChecked( _settings->get(Set::BroadCast_Active) );
	cb_prompt->setChecked( _settings->get(Set::Broadcast_Prompt) );
	sb_port->setValue( _settings->get(Set::Broadcast_Port) );

	QDialog::show();
}
