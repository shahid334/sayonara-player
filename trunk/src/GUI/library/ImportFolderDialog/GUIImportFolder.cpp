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

#include "HelperStructs/Helper.h"

#include <QPixmap>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>


GUI_ImportFolder::GUI_ImportFolder(QWidget* parent, bool copy_enabled) :
	SayonaraDialog(parent),
	Ui::ImportFolder()
{
	setupUi(this);

	_ui_tag_edit = new GUI_TagEdit(this);

	tw_tabs->addTab(_ui_tag_edit, tr("Edit"));

    _thread_active = false;

	combo_folders->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);
	cb_copy2lib->setEnabled(copy_enabled);

	cb_copy2lib->setChecked(copy_enabled);
	combo_folders->setVisible(copy_enabled);
	lab_target_path->setVisible(copy_enabled);
	lab_target_info->setVisible(copy_enabled);

	QString libpath = _settings->get(Set::Lib_Path);
	lab_target_path->setText( libpath );

	connect(btn_ok, SIGNAL(clicked()), this, SLOT(bb_accepted()));
	connect(combo_folders, SIGNAL(editTextChanged(const QString &)), this, SLOT(combo_box_changed(const QString&)));
	connect(btn_choose_dir, SIGNAL(clicked()), this, SLOT(choose_dir()));
	connect(btn_cancel, SIGNAL(clicked()), this, SLOT(bb_rejected()));

	pb_progress->setValue(0);
	pb_progress->setVisible(false);

	setModal(true);
}


GUI_ImportFolder::~GUI_ImportFolder() {

}

void GUI_ImportFolder::language_changed(){
	retranslateUi(this);
}


void GUI_ImportFolder::set_folderlist(const QStringList& lst) {
	combo_folders->clear();
	combo_folders->addItems(lst);
}

void GUI_ImportFolder::set_status(int status) {

	pb_progress->hide();
	lab_status->show();

	tw_tabs->setCurrentIndex(0);
	tw_tabs->setTabEnabled(1, false );

	switch(status){
		case IMPORT_DIALOG_CACHING:
			lab_status->setText(tr("Loading tracks..."));
			break;

		case IMPORT_DIALOG_CANCELLED:
			lab_status->setText(tr("Cancelled"));
			break;

		case IMPORT_DIALOG_NO_TRACKS:
			lab_status->setText(tr("No tracks"));
			break;

		case IMPORT_DIALOG_ROLLBACK:
			lab_status->setText(tr("Rollback"));
			break;

		default:
			lab_status->setText(tr("%1 tracks available").arg(status));
			tw_tabs->setTabEnabled(1, true);
			break;
	}
}

void GUI_ImportFolder::set_progress(int val) {

    if(val) {
		pb_progress->show();
		lab_status->hide();
    }

	else{
		pb_progress->hide();
	}

	pb_progress->setValue(val);
    if(val == 100) val = 0;
}

void GUI_ImportFolder::bb_accepted() {
	emit sig_accepted(combo_folders->currentText().trimmed(), cb_copy2lib->isChecked());
}

void GUI_ImportFolder::bb_rejected() {
    emit sig_cancelled();
}


void GUI_ImportFolder::choose_dir() {

	QString dir;
	QString lib_path = _settings->get(Set::Lib_Path);

	dir = QFileDialog::getExistingDirectory(this,
		tr("Choose target directory"),
                lib_path, QFileDialog::ShowDirsOnly);

    if(!dir.contains(lib_path)) {
        QMessageBox::warning(this, tr("Warning"), tr("%1<br />is no library directory").arg(dir));
        return;
    }

    QString parent_folder = Helper::get_parent_folder(dir);
    dir.remove(parent_folder);

    while(dir.startsWith(QDir::separator())) dir=dir.remove(0, 1);
    while(dir.endsWith(QDir::separator())) dir = dir.remove(dir.size() - 1, 1);

	combo_folders->setEditText(dir);
}


void GUI_ImportFolder::combo_box_changed(const QString& text) {

	QString lib_path = _settings->get(Set::Lib_Path);
	lab_target_path->setText( lib_path + QDir::separator() + text );
}

void GUI_ImportFolder::set_thread_active(bool b) {
    _thread_active = b;

	if(b){
		btn_cancel->setText(tr("Cancel"));
	}

	else{
		btn_cancel->setText(tr("Close"));
	}
}

void GUI_ImportFolder::show_info(const QString &str){
	QMessageBox::information(this, tr("Info"), str);
}

void GUI_ImportFolder::show_warning(const QString &str){
	QMessageBox::warning(this, tr("Warning"), str);
}



void GUI_ImportFolder::closeEvent(QCloseEvent* e) {

    if(!_thread_active) {
        e->accept();
        emit sig_closed();
        return;
    }

    e->ignore();
    emit sig_cancelled();
}

void GUI_ImportFolder::showEvent(QShowEvent* e) {
    emit sig_opened();
    e->accept();
}


