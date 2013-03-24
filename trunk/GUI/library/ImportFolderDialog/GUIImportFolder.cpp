/* GUIImportFolder.cpp */

/* Copyright (C) 2012  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GUI/library/ImportFolderDialog/GUIImportFolder.h"
#include "GUI/ui_GUI_ImportFolder.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include <QDialog>
#include <QString>
#include <QWidget>
#include <QPixmap>
#include <QScrollBar>


GUI_ImportFolder::GUI_ImportFolder(QWidget* parent, const QStringList& folder_list, bool copy_enabled) : QDialog(parent){

	Q_UNUSED(parent);


	this->ui = new Ui::ImportFolder();
	ui->setupUi(this);

    this->ui->combo_folders->addItems(folder_list);
    ui->combo_folders->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);
    this->ui->cb_copy2lib->setEnabled(copy_enabled);

    this->ui->cb_copy2lib->setChecked(copy_enabled);
    this->ui->combo_folders->setVisible(copy_enabled);
    this->ui->lab_target_path->setVisible(copy_enabled);
    this->ui->lab_target_info->setVisible(copy_enabled);

    QPixmap pixmap(Helper::getIconPath() + "/import.png");
    this->ui->lab_img->setPixmap(pixmap.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));


    QString libpath = CSettingsStorage::getInstance()->getLibraryPath();
    this->ui->lab_target_path->setText( libpath );

    connect(ui->btn_ok, SIGNAL(clicked()), this, SLOT(bb_accepted()));
    connect(ui->combo_folders, SIGNAL(editTextChanged(const QString &)), this, SLOT(combo_box_changed(const QString&)));

	ui->pb_progress->setValue(0);
	ui->pb_progress->setVisible(false);
    bool dark = (CSettingsStorage::getInstance()->getPlayerStyle() == 1);
    changeSkin(dark);
}


GUI_ImportFolder::~GUI_ImportFolder() {
	// TODO Auto-generated destructor stub
}

void GUI_ImportFolder::changeSkin(bool dark){

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
    emit accepted(this->ui->combo_folders->currentText().trimmed(), this->ui->cb_copy2lib->isChecked());
}


void GUI_ImportFolder::combo_box_changed(const QString& text){

	QString libpath = CSettingsStorage::getInstance()->getLibraryPath();
    this->ui->lab_target_path->setText( libpath + "/" + text );
}
