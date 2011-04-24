/*
 * GUI_Library_windowed.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#include "GUI_Library_windowed.h"
#include "ui_GUI_Library_windowed.h"

GUI_Library_windowed::GUI_Library_windowed(QWidget* parent) : QWidget(parent) {
	this->ui = new Ui::Library_windowed();
	this->ui->setupUi(this);

}

GUI_Library_windowed::~GUI_Library_windowed() {
	// TODO Auto-generated destructor stub
}
