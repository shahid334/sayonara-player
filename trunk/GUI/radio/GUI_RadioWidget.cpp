/*
 * GUIRadioWidget.cpp
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#include "GUI/radio/GUI_RadioWidget.h"
#include <QWidget>
#include <QDockWidget>

GUI_RadioWidget::GUI_RadioWidget(QWidget* parent) : QDockWidget(parent)  {

	this->_ui = new Ui::RadioWidget( );
	this->_ui->setupUi(this);
	this->_ui->rb_artist->setChecked(true);

	connect(_ui->rb_artist, SIGNAL(released()), this, SLOT(radio_button_changed()));
	connect(_ui->rb_tag, SIGNAL(released()), this, SLOT(radio_button_changed()));

	connect(_ui->btn_listen, SIGNAL(released()), this, SLOT(start_listen()));


}

GUI_RadioWidget::~GUI_RadioWidget() {
	// TODO Auto-generated destructor stub
}


void GUI_RadioWidget::start_listen(){

	if(_ui->le_text->text().size() >= 3)
		emit listen_clicked(_ui->le_text->text(), _ui->rb_artist->isChecked());
}

void GUI_RadioWidget::radio_button_changed(){

	if(_ui->rb_artist->isChecked()){
		_ui->rb_tag->setChecked(false);
	}

	else _ui->rb_artist->setChecked(false);
}
