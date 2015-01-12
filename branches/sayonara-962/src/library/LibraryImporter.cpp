/* LibraryImporter.cpp */

/* Copyright (C) 2011-2014  Lucio Carreras
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

#include "library/LibraryImporter.h"
#include "HelperStructs/MetaData.h"

#include <QMap>
#include <QDir>
#include <QMessageBox>

LibraryImporter::LibraryImporter(QWidget* main_window, QObject *parent) :
	QObject(parent),
	SayonaraClass()
{

	_main_window = main_window;
	_db = CDatabaseConnector::getInstance();

	_caching_thread = new ImportCachingThread(this);
	_copy_thread = new ImportCopyThread(this);

	_tag_edit = new TagEdit(this, true);
	_import_dialog = new GUI_ImportFolder(_main_window, _tag_edit, true);

	_lib_path = _settings->get(Set::Lib_Path);

	connect(_import_dialog, SIGNAL(sig_accepted(const QString&, bool)),
		   this,            SLOT(accept_import(const QString&, bool)));
	connect(_import_dialog, SIGNAL(sig_cancelled()),
		   this,            SLOT(cancel_import()));
	connect(_import_dialog, SIGNAL(sig_closed()),
		   this,            SLOT(import_dialog_closed()));
	connect(_import_dialog, SIGNAL(sig_opened()),
		   this,            SLOT(import_dialog_opened()));

	connect(_caching_thread, SIGNAL(finished()), this, SLOT(caching_thread_finished()));
	connect(_caching_thread, SIGNAL(sig_done()), this, SLOT(caching_thread_done()));
	connect(_caching_thread, SIGNAL(sig_progress(int)), this, SLOT(import_progress(int)));

	connect(_copy_thread, SIGNAL(finished()), this, SLOT(copy_thread_finished()));
	connect(_copy_thread, SIGNAL(sig_progress(int)), this, SLOT(import_progress(int)));
	connect(_tag_edit, SIGNAL(sig_metadata_changed(const MetaDataList&, const MetaDataList&)),
		   this, SLOT(metadata_changed(const MetaDataList&, const MetaDataList&)));
}


void LibraryImporter::psl_import_dir(const QString& dir) {

    QStringList lst;
	lst << dir;
	psl_import_files(lst, dir);
}

void LibraryImporter::psl_import_files(const QStringList& list, QString src_dir) {

	_src_dir = src_dir;

	QDir lib_dir(_lib_path);
	QStringList content = lib_dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);
	content.push_front("");

	_import_dialog->set_folderlist(content);
	_import_dialog->set_thread_active(true);
	_import_dialog->set_status(IMPORT_DIALOG_CACHING);
	_import_dialog->show();

    _caching_thread->set_filelist(list);
    _caching_thread->start();
}


// preload thread has cached everything, but ok button has not been clicked yet
void LibraryImporter::caching_thread_done() {

	MetaDataList v_md;

    int n_tracks = _caching_thread->get_n_tracks();

	v_md = _caching_thread->get_metadata();
	_tag_edit->set_metadata(v_md);

	_import_dialog->set_status(n_tracks);
}


// Caching is done, ok has been clicked
void LibraryImporter::caching_thread_finished() {

    _import_dialog->set_progress(0);
    _import_dialog->set_thread_active(false);

	int n_tracks = _caching_thread->get_n_tracks();

	if(n_tracks == 0) {
		_import_dialog->set_status(IMPORT_DIALOG_NO_TRACKS);
        return;
    }

	MetaDataList v_md = _caching_thread->get_metadata();
	QStringList files = _caching_thread->get_extracted_files();
	QMap<QString, MetaData> md_map = _caching_thread->get_md_map();
	QMap<QString, QString> pd_map = _caching_thread->get_pd_map();

    if(!_copy_to_lib) {

		foreach(MetaData md, md_map.values()){
			v_md << md;
		}

		bool success = _db->storeMetadata(v_md);

		if(success){
			emit sig_imported();
		}

        return;
    }

    _import_dialog->set_thread_active(true);

	_copy_thread->set_vars(_import_to, files, md_map, pd_map);
    _copy_thread->set_mode(IMPORT_COPY_THREAD_COPY);
    _copy_thread->start();
}



void LibraryImporter::copy_thread_finished() {

	MetaDataList v_md = _copy_thread->get_metadata();

	_import_dialog->set_thread_active(false);

    // no tracks were copied or rollback was finished
	if(v_md.size() == 0) {
		_import_dialog->set_status(IMPORT_DIALOG_NO_TRACKS);
        return;
    }

    // copy was cancelled
    qDebug() << "Copy folder thread finished " << _copy_thread->get_cancelled();
    if(_copy_thread->get_cancelled()) {
        _copy_thread->set_mode(IMPORT_COPY_THREAD_ROLLBACK);
        _copy_thread->start();
		_import_dialog->set_status(IMPORT_DIALOG_ROLLBACK);
        _import_dialog->set_thread_active(true);
        return;
    }

    // store to db
	bool success = _db->storeMetadata(v_md);
    int n_snd_files = _copy_thread->get_n_files();
    int n_files_copied = _copy_thread->get_copied_files();

    // error and success messages
    if(success) {
        QString str = "";
		if(n_snd_files == n_files_copied){
			str =   tr("All files could be imported");
		}

		else{
			str =  tr("%1 of %2 files could be imported").arg(n_files_copied).arg(n_snd_files);
		}

		_import_dialog->show_info(str);
		emit sig_imported();
    }

    else{
		_import_dialog->show_warning(
				tr("Sorry, but tracks could not be imported") +
				"<br />" +
				tr("Please use the import function of the file menu<br /> or move tracks to library and use 'Reload library'")
		);

    }

    _import_dialog->close();
}


void LibraryImporter::metadata_changed(const MetaDataList& old_md, const MetaDataList& new_md){

	_caching_thread->update_metadata(old_md, new_md);
}


void  LibraryImporter::import_dialog_opened() {
    emit sig_lib_changes_allowed(false);
}


void  LibraryImporter::import_dialog_closed() {
    emit sig_lib_changes_allowed(true);
}


void  LibraryImporter::import_progress(int i) {
    _import_dialog->set_progress(i);
}

// fired if ok was clicked in dialog
void  LibraryImporter::accept_import(const QString& chosen_item, bool copy) {

    // the preload thread may terminate now
    _caching_thread->set_may_terminate(true);
    _import_to = chosen_item;
    _copy_to_lib = copy;
}


// fired if cancel button was clicked in dialog
void LibraryImporter::cancel_import() {


	_import_dialog->set_status(IMPORT_DIALOG_CANCELLED);

    // preload thread
    if(_caching_thread->isRunning()) {

        _caching_thread->set_cancelled();
        _import_dialog->set_thread_active(false);
        _import_dialog->close();
    }

    // copy folder thread
    else if(_copy_thread->isRunning()) {

        // useless during rollback
        if(_copy_thread->get_mode() == IMPORT_COPY_THREAD_ROLLBACK) {
            return;
        }

        _copy_thread->set_cancelled();

    }

    // close dialog
    else{
        _import_dialog->close();
    }
}
