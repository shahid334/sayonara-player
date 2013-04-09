/* CLibraryAdmin.cpp */

/* Copyright (C) 2013  Lucio Carreras
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




#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "library/ReloadThread.h"
#include "library/CLibraryBase.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include "application.h"
#include "GUI/library/ImportFolderDialog/GUIImportFolder.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QFileSystemWatcher>
#include <QListWidget>

/// TODO: FORBIN RELOAD LIBRARY <=> IMPORT at same time

void CLibraryBase::baseDirSelected (const QString & baseDir) {

    QStringList fileList;
    int num_files = 0;

    m_reader.getFilesInsiderDirRecursive(QDir(baseDir),fileList, num_files);

    emit sig_playlist_created(fileList);

}

void CLibraryBase::importFiles(const MetaDataList& v_md){

    MetaDataList v_md_new = v_md;
    qDebug() << "Import files " << v_md.size();
    bool success = CDatabaseConnector::getInstance()->storeMetadata(v_md_new);
    emit sig_import_result(success);
}

void CLibraryBase::importDirectory(QString directory){


    m_library_path = CSettingsStorage::getInstance()->getLibraryPath();

    m_src_dir = directory;
    m_import_folder_thread->set_src_dir(directory);
    m_import_folder_thread->start();

    QDir lib_dir(m_library_path);
    QStringList content = lib_dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

    content.push_front("");

    if(m_import_dialog){
        disconnect(m_import_dialog, SIGNAL(accepted(const QString&, bool)), 
                   this,            SLOT(accept_import(const QString&, bool)));
        disconnect(m_import_dialog, SIGNAL(sig_cancelled()),
                   this,            SLOT(cancel_import()));
        disconnect(m_import_dialog, SIGNAL(sig_closed()), 
                   this,            SLOT(import_dialog_closed()));
        disconnect(m_import_dialog, SIGNAL(sig_opened()), 
                   this,            SLOT(import_dialog_opened()));


        delete m_import_dialog;
    }

    m_import_dialog = new GUI_ImportFolder(m_app->getMainWindow(), content, true);

    connect(m_import_dialog, SIGNAL(sig_accepted(const QString&, bool)), 
            this,            SLOT(accept_import(const QString&, bool)));
    connect(m_import_dialog, SIGNAL(sig_cancelled()), 
            this,            SLOT(cancel_import()));
    connect(m_import_dialog, SIGNAL(sig_closed()),
            this,            SLOT(import_dialog_closed()));
    connect(m_import_dialog, SIGNAL(sig_opened()), 
            this,            SLOT(import_dialog_opened()));

    m_import_dialog->set_thread_active(true);
    m_import_dialog->set_status(tr("Loading files..."));
    m_import_dialog->show();
}

void CLibraryBase::import_dialog_opened(){
	emit sig_reload_library_allowed(false);
}
void CLibraryBase::import_dialog_closed(){
	emit sig_reload_library_allowed(true);
}

void CLibraryBase::import_progress(int i){
    m_import_dialog->set_progress(i);
}

// fired if ok was clicked in dialog
void CLibraryBase::accept_import(const QString& chosen_item, bool copy){

	// the preload thread may terminate now
    m_import_folder_thread->set_may_terminate(true);
    _import_to = chosen_item;
    _import_copy = copy;
}

// fired if cancel button was clicked in dialog
void CLibraryBase::cancel_import(){

    qDebug() << "Cancel import";

	// preload thread
    if(m_import_folder_thread->isRunning()){

        m_import_folder_thread->set_cancelled();
        m_import_dialog->set_status("Cancelled");
    	m_import_dialog->set_thread_active(false);
      
        m_import_dialog->close();
    }

	// copy thread
    else if(m_copy_folder_thread->isRunning()){

		// useless during rollback
        if(m_copy_folder_thread->get_mode() == COPY_FOLDER_THREAD_ROLLBACK){
			return;		
		}

        m_copy_folder_thread->terminate();
        m_copy_folder_thread->set_cancelled();
        m_import_dialog->set_status(tr("Cancelled"));
    }

	// close dialog
    else{
        m_import_dialog->close();
    }
}

// preload thread
void CLibraryBase::import_folder_thread_done(){

    int n_tracks = m_import_folder_thread->get_n_tracks();

    if(n_tracks > 0){
        QString status = tr("%1 tracks ready").arg(n_tracks);
        m_import_dialog->set_status(status);
    }

    else{
        m_import_dialog->set_status(tr("No tracks"));
    }
}

void CLibraryBase::import_folder_thread_finished(){

    m_import_dialog->set_progress(0);
    m_import_dialog->set_thread_active(false);

    QStringList files;
    QMap<QString, MetaData> map;
    
    m_import_folder_thread->get_filelist(files);
    m_import_folder_thread->get_md_map(map);

    if(map.keys().size() == 0){
        m_import_dialog->set_status(tr("No Tracks"));
        return;
    }

    if(!_import_copy){
        MetaDataList v_md;
        foreach(QString filename, files){
			bool has_key = map.keys().contains(filename);
			if(!has_key) continue;

            MetaData md;
            md = map.value(filename);
            v_md.push_back(md);
        }

        bool success = _db->storeMetadata(v_md);

        emit sig_import_result(success);
        emit sig_reload_library_finished();

        refresh();

        return;
    }
    

    m_import_dialog->set_thread_active(true);
    m_copy_folder_thread->set_vars(_import_to, m_library_path, m_src_dir, files, map );
    m_copy_folder_thread->set_mode(COPY_FOLDER_THREAD_COPY);
    m_copy_folder_thread->start();

}







void CLibraryBase::copy_folder_thread_finished(){

    MetaDataList v_md;
    m_copy_folder_thread->get_metadata(v_md);
    m_import_dialog->set_thread_active(false);

    // no tracks were copied or rollback was finished
    if(v_md.size() == 0) {
        m_import_dialog->set_status(tr("No Tracks"));

        return;
    }

    // copy was cancelled
    if(m_copy_folder_thread->get_cancelled()){
        m_copy_folder_thread->set_mode(COPY_FOLDER_THREAD_ROLLBACK);
        m_copy_folder_thread->start();
        m_import_dialog->set_status(tr("Rollback..."));
	m_import_dialog->set_thread_active(true);
        return;
    }

    bool success = _db->storeMetadata(v_md);
    int n_snd_files = m_copy_folder_thread->get_n_files();
    int n_files_copied = m_copy_folder_thread->get_copied_files();

    if(v_md.size() == 0) success = false;
    if(success){
        QString str = "";
        if(n_snd_files == n_files_copied)
            str =   tr("All files could be imported");

        else
            str =  tr("%1 of %2 files could be imported").arg(n_files_copied).arg(n_snd_files);

        QMessageBox::information( m_app->getMainWindow(), tr("Import files"), str);
        refresh();
    }

    else{
        QMessageBox::warning(m_app->getMainWindow(),
                             tr("Import files"),
                             tr("Sorry, but tracks could not be imported") + "<br />") +
                tr("Please use the import function of the file menu<br /> or move tracks to library and use 'Reload library'");
    }

    m_import_dialog->close();
    emit sig_import_result(success);
}


void CLibraryBase::clearLibrary(){
    MetaDataList lst;
    _db->getTracksFromDatabase(lst);
    _db->deleteTracks(lst);
    refresh();
}


void CLibraryBase::reloadLibrary(bool clear){

    m_library_path = CSettingsStorage::getInstance()->getLibraryPath();

    if(m_library_path.length() == 0) {
        QMessageBox msgBox(m_app->getMainWindow());
        msgBox.setText(tr("Please select your library first"));
        msgBox.exec();

        QString dir = QFileDialog::getExistingDirectory(m_app->getMainWindow(), tr("Open Directory"),	getenv("$HOME"), QFileDialog::ShowDirsOnly);

        if(dir.length() < 3) {
            QMessageBox msgBox(m_app->getMainWindow());
            msgBox.setText(tr("I said: \"Please select your library first\". Bye bye!"));
            msgBox.exec();
            return;
        }

        else {
            m_library_path = dir;
            CSettingsStorage::getInstance()->setLibraryPath(dir);
        }
    }


    if(clear){
        clearLibrary();
    }

    if(m_thread->isRunning()){
        m_thread->terminate();
    }

    m_thread->set_lib_path(m_library_path);
    m_thread->start();
}


// TODO:
void CLibraryBase::reload_thread_finished(){

    _db->getAllAlbums(_vec_albums);
    _db->getAllArtists(_vec_artists);
    _db->getTracksFromDatabase(_vec_md);

    emit sig_all_artists_loaded(_vec_artists);
    emit sig_all_albums_loaded(_vec_albums);
    emit sig_all_tracks_loaded(_vec_md);
    emit sig_reload_library_finished();
}

void CLibraryBase::library_reloading_state_new_block(){
    m_thread->pause();
    _db->getAllAlbums(_vec_albums);
    _db->getAllArtists(_vec_artists);
    _db->getTracksFromDatabase(_vec_md);

    emit sig_all_artists_loaded(_vec_artists);
    emit sig_all_albums_loaded(_vec_albums);
    emit sig_all_tracks_loaded(_vec_md);

    m_thread->goon();

}

void CLibraryBase::library_reloading_state_slot(QString str){

    emit sig_reloading_library(str);
}


void CLibraryBase::insertMetaDataIntoDB(MetaDataList& v_md) {

    _db->storeMetadata(v_md);

    MetaDataList data;
    _db->getTracksFromDatabase(data);
    emit sig_all_tracks_loaded(data);
}



void CLibraryBase::loadDataFromDb () {

    _filter.cleared = true;
    _filter.filtertext = "";

    _db->getAllArtists(_vec_artists);
    _db->getAllAlbums(_vec_albums);
    _db->getTracksFromDatabase(_vec_md);

    emit_stuff();
}



void CLibraryBase::psl_delete_tracks(int answer){
    delete_tracks(_vec_md, answer);
}


void CLibraryBase::psl_delete_certain_tracks(const QList<int>& lst, int answer){

    MetaDataList vec_md;
    foreach(int idx, lst){
        vec_md.push_back(_vec_md[idx]);
    }

    delete_tracks(vec_md, answer);
}



void CLibraryBase::delete_tracks(MetaDataList& vec_md, int answer){

    QStringList file_list;
    QString file_entry = tr("files");
    int n_files = vec_md.size();
    int n_fails = 0;

    foreach(MetaData md, vec_md){
        file_list.push_back(md.filepath);
    }

    _db->deleteTracks(vec_md);
    vec_md.clear();

    if(answer == 1){
        file_entry = tr("entries");
        foreach(QString filename, file_list){
            QFile file(filename);
            if( !file.remove() )
                n_fails ++;
        }
    }

    QString answer_str;

    if(n_fails == 0){
        answer_str = tr("All %1 could be removed").arg(file_entry);
    }

    else {
        answer_str = tr("%1 of %2 %3 could not be removed").arg(n_fails).arg(n_files).arg(file_entry);
    }

    emit sig_delete_answer(answer_str);
    refresh();
}
