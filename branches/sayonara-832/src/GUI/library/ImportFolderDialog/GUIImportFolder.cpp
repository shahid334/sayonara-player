/* GUIImportFolder.cpp */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include <QString>
#include <QPixmap>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>


GUI_ImportFolder::GUI_ImportFolder(QWidget* parent, bool copy_enabled) : QDialog(parent){

    Q_UNUSED(parent);


    this->ui = new Ui::ImportFolder();
    ui->setupUi(this);

    _thread_active = false;


    ui->combo_folders->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);
    ui->cb_copy2lib->setEnabled(copy_enabled);

    ui->cb_copy2lib->setChecked(copy_enabled);
    ui->combo_folders->setVisible(copy_enabled);
    ui->lab_target_path->setVisible(copy_enabled);
    ui->lab_target_info->setVisible(copy_enabled);

    QPixmap pixmap(Helper::getIconPath() + "/import.png");
    ui->lab_img->setPixmap(pixmap.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QString libpath = CSettingsStorage::getInstance()->getLibraryPath();
    ui->lab_target_path->setText( libpath );

    connect(ui->btn_ok, SIGNAL(clicked()), this, SLOT(bb_accepted()));
    connect(ui->combo_folders, SIGNAL(editTextChanged(const QString &)), this, SLOT(combo_box_changed(const QString&)));
    connect(ui->btn_choose_dir, SIGNAL(clicked()), this, SLOT(choose_dir()));
    connect(ui->btn_cancel, SIGNAL(clicked()), this, SLOT(bb_rejected()));


    ui->pb_progress->setValue(0);
    ui->pb_progress->setVisible(false);

    this->setModal(true);

    bool dark = (CSettingsStorage::getInstance()->getPlayerStyle() == 1);
    changeSkin(dark);
}


GUI_ImportFolder::~GUI_ImportFolder() {

}

void GUI_ImportFolder::changeSkin(bool dark){

}


void GUI_ImportFolder::set_folderlist(const QStringList& lst){
    ui->combo_folders->clear();
    ui->combo_folders->addItems(lst);
}

void GUI_ImportFolder::set_status(QString str){
    this->ui->pb_progress->hide();
    this->ui->lab_status->show();
    this->ui->lab_status->setText(str);
}

void GUI_ImportFolder::set_progress(int val){

    if(val){
        ui->pb_progress->show();
        ui->lab_status->hide();
    }

    else
        ui->pb_progress->hide();

    ui->pb_progress->setValue(val);
    if(val == 100) val = 0;
}

void GUI_ImportFolder::bb_accepted(){

    emit sig_accepted(ui->combo_folders->currentText().trimmed(), ui->cb_copy2lib->isChecked());
}

void GUI_ImportFolder::bb_rejected(){
    emit sig_cancelled();
}


void GUI_ImportFolder::choose_dir(){


    QString lib_path = CSettingsStorage::getInstance()->getLibraryPath();
    QString dir = QFileDialog::getExistingDirectory(this, 
		tr("Choose target directory"),
                lib_path, QFileDialog::ShowDirsOnly);

    if(!dir.contains(lib_path)){
        QMessageBox::warning(this, tr("Warning"), tr("%1<br />is no library directory").arg(dir));
        return;
    }

    QString parent_folder = Helper::get_parent_folder(dir);
    dir.remove(parent_folder);

    while(dir.startsWith(QDir::separator())) dir=dir.remove(0, 1);
    while(dir.endsWith(QDir::separator())) dir = dir.remove(dir.size() - 1, 1);

    this->ui->combo_folders->setEditText(dir);
}


void GUI_ImportFolder::combo_box_changed(const QString& text){

    QString libpath = CSettingsStorage::getInstance()->getLibraryPath();
    ui->lab_target_path->setText( libpath + QDir::separator() + text );
}

void GUI_ImportFolder::set_thread_active(bool b){
    _thread_active = b;

    if(b)
        this->ui->btn_cancel->setText(tr("Cancel"));
    else
        this->ui->btn_cancel->setText(tr("Close"));

}

void GUI_ImportFolder::closeEvent(QCloseEvent* e){

    if(!_thread_active){
        e->accept();
        emit sig_closed();
        return;
    }

    e->ignore();
    emit sig_cancelled();
}

void GUI_ImportFolder::showEvent(QShowEvent* e){
    emit sig_opened();
    e->accept();
}
