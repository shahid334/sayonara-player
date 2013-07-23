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

#define IMPORT_FILES 0
#define IMPORT_DIR 1


#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "library/CLibraryBase.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Filter.h"
#include "application.h"

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



void CLibraryBase::importDirectory(const QString& dir){
	QStringList lst;
	lst << dir;
	importFiles(lst);	
}

void CLibraryBase::importFiles(const QStringList& list){
    
    m_library_path = CSettingsStorage::getInstance()->getLibraryPath();

    m_src_files = list;
    m_import_thread->set_filelist(list);


    QDir lib_dir(m_library_path);
    QStringList content = lib_dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

    content.push_front("");

    m_import_dialog->set_folderlist(content);
    m_import_dialog->set_thread_active(true);
    m_import_dialog->set_status(tr("Loading files..."));
    m_import_dialog->show();

    m_import_thread->start();

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
    m_import_thread->set_may_terminate(true);
    _import_to = chosen_item;
    _import_copy = copy;
}


// fired if cancel button was clicked in dialog
void CLibraryBase::cancel_import(){

    qDebug() << "Cancel import";

    // preload thread
    if(m_import_thread->isRunning()){

        m_import_thread->set_cancelled();
        m_import_dialog->set_status("Cancelled");
        m_import_dialog->set_thread_active(false);

        m_import_dialog->close();
    }

    // copy folder thread
    else if(m_import_copy_thread->isRunning()){

        qDebug() << "Rollback?";
        // useless during rollback
        if(m_import_copy_thread->get_mode() == IMPORT_COPY_THREAD_ROLLBACK){
            return;
        }

        qDebug() << "Rollback";
        m_import_copy_thread->set_cancelled();
        m_import_dialog->set_status(tr("Cancelled"));
    }

    // close dialog
    else{
        m_import_dialog->close();
    }
}


// preload thread has cached everything, but maybe ok button has not been clicked yet
void CLibraryBase::import_thread_done(){

    int n_tracks = m_import_thread->get_n_tracks();

    if(n_tracks > 0){
        QString status = tr("%1 tracks ready").arg(n_tracks);
        m_import_dialog->set_status(status);
    }

    else{
        m_import_dialog->set_status(tr("No tracks"));
    }
}


// Caching is done, ok has been clicked
void CLibraryBase::import_thread_finished(){

    m_import_dialog->set_progress(0);
    m_import_dialog->set_thread_active(false);

    QStringList files;
    QMap<QString, MetaData> md_map;
    QMap<QString, QString> pd_map;
    
    m_import_thread->get_extracted_files(files);
    m_import_thread->get_md_map(md_map);
    m_import_thread->get_pd_map(pd_map);

    if(md_map.keys().size() == 0){
        m_import_dialog->set_status(tr("No Tracks"));
        return;
    }

    if(!_import_copy){
        MetaDataList v_md;
        foreach(QString filename, files){
            bool has_key = md_map.keys().contains(filename);
            if(!has_key) continue;

            MetaData md;
            md = md_map.value(filename);
            v_md.push_back(md);
        }

        bool success = _db->storeMetadata(v_md);

        emit sig_import_result(success);
        emit sig_reload_library_finished();

        refresh();

        return;
    }
  

    m_import_dialog->set_thread_active(true);

    m_import_copy_thread->set_vars(_import_to, files, md_map, pd_map);
    m_import_copy_thread->set_mode(IMPORT_COPY_THREAD_COPY);
    m_import_copy_thread->start();

}



void CLibraryBase::import_copy_thread_finished(){

    MetaDataList v_md;
    m_import_copy_thread->get_metadata(v_md);
    m_import_dialog->set_thread_active(false);

    // no tracks were copied or rollback was finished
    if(v_md.size() == 0) {
        m_import_dialog->set_status(tr("No Tracks"));

        return;
    }

    // copy was cancelled
    qDebug() << "Copy folder thread finished " << m_import_copy_thread->get_cancelled();
    if(m_import_copy_thread->get_cancelled()){
        m_import_copy_thread->set_mode(IMPORT_COPY_THREAD_ROLLBACK);
        m_import_copy_thread->start();
        m_import_dialog->set_status(tr("Rollback..."));
        m_import_dialog->set_thread_active(true);
        return;
    }

    // store to db
    bool success = _db->storeMetadata(v_md);
    int n_snd_files = m_import_copy_thread->get_n_files();
    int n_files_copied = m_import_copy_thread->get_copied_files();

    // error and success messages
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
            emit sig_libpath_set(dir);
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

    emit_stuff();

    emit sig_reload_library_finished();
}

void CLibraryBase::library_reloading_state_new_block(){

    m_thread->pause();

    _db->getAllAlbums(_vec_albums);
    _db->getAllArtists(_vec_artists);
    _db->getTracksFromDatabase(_vec_md);

    emit_stuff();

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
