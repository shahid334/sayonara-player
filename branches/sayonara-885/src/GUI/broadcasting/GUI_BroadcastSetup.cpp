#include "GUI_BroadcastSetup.h"
#include "HelperStructs/Helper.h"


GUI_BroadcastSetup::GUI_BroadcastSetup(QWidget *parent) :
	QDialog(parent),
	Ui::GUI_BroadcastSetup()
{
	setupUi(this);
	lab_icon->setPixmap(Helper::getPixmap("broadcast.png", QSize(100, 100), true));

	_settings = CSettingsStorage::getInstance();
}

void GUI_BroadcastSetup::accept(){

	bool new_active = cb_active->isChecked();
	bool new_prompt = cb_prompt->isChecked();
	bool new_port = sb_port->value();

	bool old_active = _settings->getBroadcastActive();
	bool old_prompt = _settings->getBroadcastPrompt();
	bool old_port = _settings->getBroadcastPort();

	if(old_active != new_active){
		_settings->setBroadcastActive(new_active);
		emit sig_active_changed(new_active);
	}

	if(old_prompt != new_prompt){
		_settings->setBroadcastPrompt(new_prompt);
		emit sig_prompt_changed(new_prompt);
	}

	if(old_port != new_port){
		_settings->setBroadcastPort(sb_port->value());
		emit sig_port_changed(new_port);
	}

	QDialog::accept();
}

void GUI_BroadcastSetup::reject(){
	QDialog::reject();
}

void GUI_BroadcastSetup::show(){

	cb_active->setChecked(_settings->getBroadcastActive());
	cb_prompt->setChecked(_settings->getBroadcastPrompt());
	sb_port->setValue(_settings->getBroadcastPort());

	QDialog::show();
}
