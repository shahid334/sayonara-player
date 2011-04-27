/*
 * LibrarySetupWidget.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: luke
 */

#include <QFileDialog>
#include <QWidget>
#include <QObject>
#include "GUI/Library/LibrarySetupWidget.h"

#include <ui_GUI_LibrarySetup_Widget.h>

LibrarySetupWidget::LibrarySetupWidget(QWidget* parent) : QWidget(parent) {
	this->ui = new Ui::LibrarySetup();

	this->ui->setupUi(this);
	connect(this->ui->btn_save, SIGNAL(clicked(bool)), this, SLOT(save_button_clicked(bool)));
	connect(this->ui->btn_choose, SIGNAL(clicked(bool)), this, SLOT(choose_button_clicked(bool)));

}

LibrarySetupWidget::~LibrarySetupWidget() {
	// TODO Auto-generated destructor stub
}

void LibrarySetupWidget::save_button_clicked(bool b){
	Q_UNUSED(b);
	if(this->ui->le_libpath->text().length() > 3){
		emit libpath_changed(this->ui->le_libpath->text());
		this->close();
	}
}

void LibrarySetupWidget::choose_button_clicked(bool b){
	Q_UNUSED(b)
	QString path = QFileDialog::getExistingDirectory(this, "Choose music library", QDir::homePath());
	this->ui->le_libpath->setText(path);

}/*

void LibrarySetupWidget::show(){
	this->show();
}*/
