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

void CLibraryBase::baseDirSelected (const QString & baseDir) {

    QStringList fileList;
    int num_files = 0;

    m_reader.getFilesInsiderDirRecursive(QDir(baseDir),fileList, num_files);

    emit sig_playlist_created(fileList);

}

void CLibraryBase::importDirectory(QString directory){


    m_library_path = CSettingsStorage::getInstance()->getLibraryPath();


    m_src_dir = directory;

    QDir lib_dir(m_library_path);
    QDir src_dir(directory);

    QDir tmp_src_dir = src_dir;
    tmp_src_dir.cdUp();

    QString rel_src_path = tmp_src_dir.relativeFilePath(directory) + QDir::separator();
    QStringList content = lib_dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

    content.push_front("");

    if(m_import_dialog){
        disconnect(m_import_dialog, SIGNAL(accepted(const QString&, bool)), this, SLOT(importDirectoryAccepted(const QString&, bool)));
        delete m_import_dialog;
        m_import_dialog = 0;
    }

    m_import_dialog = new GUI_ImportFolder(m_app->getMainWindow(), content, true);
    connect(m_import_dialog, SIGNAL(accepted(const QString&, bool)), this, SLOT(importDirectoryAccepted(const QString&, bool)));
    m_import_dialog->show();
}

void CLibraryBase::importFiles(const MetaDataList& v_md){

    MetaDataList v_md_new = v_md;
    qDebug() << "Import files " << v_md.size();
    bool success = CDatabaseConnector::getInstance()->storeMetadata(v_md_new);
    emit sig_import_result(success);
}




void CLibraryBase::importDirectoryAccepted(const QString& chosen_item, bool copy){

    QDir lib_dir(m_library_path);
    QDir src_dir(m_src_dir);

    QStringList files;
    int n_files;
    CDirectoryReader reader;
    reader.getFilesInsiderDirRecursive(src_dir, files, n_files);

    if(!copy){
        MetaDataList v_md;
        foreach(QString filename, files){
            MetaData md;
            md = _db->getTrackByPath(filename);

            if(md.id < 0){
                if(!ID3::getMetaDataOfFile(md)) continue;
            }

            v_md.push_back(md);
        }

        bool success = _db->storeMetadata(v_md);

        emit sig_import_result(success);
        emit sig_reload_library_finished();

        refresh();

        return;
    }


    // extract src_folder_name = the name of the folder to import without stuff in front of it
    QString src_folder_name = m_src_dir;
    src_folder_name.remove(Helper::get_parent_folder(src_folder_name));
    while(src_folder_name.startsWith(QDir::separator())) src_folder_name.remove(0,1);
    while(src_folder_name.endsWith(QDir::separator())) src_folder_name.remove(src_folder_name.size() - 1, 1);

    // and create folder in library
    lib_dir.mkpath(chosen_item + QDir::separator() + src_folder_name);

    
    MetaDataList v_md;
    int n_files_copied = 0;
    int n_snd_files = 0;

    QString target_path = m_library_path + QDir::separator() + 
                          chosen_item + QDir::separator() + 
                          src_folder_name;

    QDir target_dir(target_path);


    foreach(QString filename, files){

        // extract folders and create directories
	QString target_path;
	QString folder = Helper::get_parent_folder(filename);
	folder.remove(m_src_dir);
	while(folder.startsWith(QDir::separator())) folder.remove(0,1);
	while(folder.endsWith(QDir::separator())) folder.remove(folder.size() - 1, 1);

	target_dir.mkpath(folder);
	QString new_target_path = target_path + QDir::separator() + folder;

	// copy file
	QFile f(filename);
	QString filename_wo_folder = Helper::get_filename_of_path(filename);
	QString new_filename = new_target_path + QDir::separator() + filename_wo_folder;
	bool copied = f.copy(new_filename);

	// insert to db
	if(!Helper::is_soundfile(filename)) continue;
	else if(Helper::is_soundfile(filename)){
		n_snd_files ++;

		if(copied) n_files_copied ++;
		else continue;
	}

        MetaData md;
        md.filepath = new_filename;
        if( ID3::getMetaDataOfFile(md) )
            v_md.push_back( md );
    }


    bool success = _db->storeMetadata(v_md);

    if(v_md.size() == 0) success = false;

    if(success){
	    if(n_snd_files == n_files_copied){
	            QMessageBox::information(
				m_app->getMainWindow(), 
				tr("Import files"), 
				tr("All files could be imported")
                    );
            }

	    else {
	            QMessageBox::information(
				m_app->getMainWindow(), 
				tr("Import files"), 
				tr("%1 of %2 files could be imported").arg(n_files_copied).arg(n_snd_files)
                    );
            }

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
