/*
 * GUIImportFolder.cpp
 *
 *  Created on: Dec 7, 2011
 *      Author: luke
 */

#include "GUI/library/GUIImportFolder.h"
#include "HelperStructs/CSettingsStorage.h"
#include "ui_GUI_ImportFolder.h"
#include <QString>
#include <QWidget>


GUI_ImportFolder::GUI_ImportFolder(QWidget* parent, const QStringList& folder_list, bool copy_enabled) {

	Q_UNUSED(parent);


	this->ui = new Ui::ImportFolder();
	ui->setupUi(this);

	this->ui->comB_proposed_artists->addItems(folder_list);
	this->ui->cb_copy->setEnabled(copy_enabled);
	if(!copy_enabled){
		this->ui->cb_copy->setChecked(false);
	}

	connect(ui->bb_ok_cancel, SIGNAL(accepted()), this, SLOT(bb_accepted()));
	connect(ui->bb_ok_cancel, SIGNAL(rejected()), this, SLOT(bb_recjected()));
	connect(ui->comB_proposed_artists, SIGNAL(editTextChanged(const QString &)), this, SLOT(combo_box_changed(const QString&)));

	ui->pb_progress->setValue(0);
	ui->pb_progress->setVisible(false);

}

GUI_ImportFolder::~GUI_ImportFolder() {
	// TODO Auto-generated destructor stub
}

void GUI_ImportFolder::progress_changed(int val){

	if(val)
		ui->pb_progress->setVisible(true);
	else
		ui->pb_progress->setVisible(false);

	ui->pb_progress->setValue(val);
	if(val == 100) val = 0;
}

void GUI_ImportFolder::bb_accepted(){
	emit accepted(this->ui->comB_proposed_artists->currentText(), this->ui->cb_copy->isChecked());
}

void GUI_ImportFolder::bb_recjected(){
	this->close();
	//emit rejected();
}

void GUI_ImportFolder::combo_box_changed(const QString& text){

	QString libpath = CSettingsStorage::getInstance()->getLibraryPath();
	this->ui->lab_full_path->setText( libpath + "/" + text );
}
