
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
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QFileSystemWatcher>
#include <QInputDialog>
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
    CDatabaseConnector* db = CDatabaseConnector::getInstance();

    if(!copy){
        CDirectoryReader reader;
        QStringList files;
        int n_files;
        reader.getFilesInsiderDirRecursive(src_dir, files, n_files);
        MetaDataList v_md;
        foreach(QString filename, files){
            MetaData md;
            md = _db->getTrackByPath(filename);

            if(md.id < 0){
                if(!ID3::getMetaDataOfFile(md)) continue;
            }

            v_md.push_back(md);
        }

        bool success = db->storeMetadata(v_md);

        emit sig_import_result(success);
        emit sig_reload_library_finished();

        refresh();

        return;
    }

    QDir tmp_src_dir = src_dir;
    tmp_src_dir.cdUp();

    QString rel_src_path = tmp_src_dir.relativeFilePath(m_src_dir) + QDir::separator();

    QString target_path = m_library_path +
                QDir::separator() +
                chosen_item +
                QDir::separator() +
                rel_src_path.replace(" ", "_");

        QStringList files2copy;
        files2copy.push_back(src_dir.absolutePath());
        int num_files = 1;

        for(int i=0; i<num_files; i++){

            // fetch all entries of a file, maybe it's a directory
            QDir sub_dir(files2copy[i]);
            QStringList sub_files = sub_dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst);

            // it is a directory
            if(sub_files.size() != 0){

                // create this directory
                QDir tmp_src_dir(src_dir);
                QDir new_sub_dir( target_path + tmp_src_dir.relativeFilePath(files2copy[i]) );
                sub_dir.mkpath(new_sub_dir.path());

                // remove this "file" from list
                files2copy.removeAt(i);
                num_files--;


                // insert all files/directories from subdir
                // into array
                for(int j=0; j<sub_files.size(); j++){
                    files2copy.insert(i+j, sub_dir.path() + QDir::separator() + sub_files[j] );
                }

                num_files += sub_files.size();
                i--;
            }
        }


        // copy & save to database
        MetaDataList v_metadata;

        bool success = false;
        for(int i=0; i<files2copy.size(); i++){

            // target path + relative src path
            QDir tmp_src_dir(src_dir);
            QString new_filename = target_path + tmp_src_dir.relativeFilePath(files2copy[i]);

            QFile f(files2copy[i]);
            if( f.copy(new_filename) ){
                success = true;
                int percent = (i * 10000) / (100 * files2copy.size());
                if( i== files2copy.size() -1) {
                    m_import_dialog->close();
                }
                else m_import_dialog->progress_changed(percent);
                if(Helper::is_soundfile(new_filename)){
                    MetaData md;
                    md.filepath = QDir(new_filename).absolutePath();
                    if( ID3::getMetaDataOfFile( md))
                        v_metadata.push_back( md );
                }
            }
        }

        m_import_dialog->progress_changed(0);

        success &= db->storeMetadata(v_metadata);

        if(success){
            QMessageBox::information(m_app->getMainWindow(), "Import files", "All files could be imported");
            refresh();
        }

        else
            QMessageBox::warning(m_app->getMainWindow(), "Import files", QString("Sorry, but tracks could not be imported <br />") +
                                 "Please use the import function of the file menu<br /> or move tracks to library and use 'Reload library'");

        m_import_dialog->close();

        emit sig_import_result(success);
}



void CLibraryBase::reloadLibrary(){

    m_library_path = CSettingsStorage::getInstance()->getLibraryPath();

    if(m_library_path.length() == 0) {
        QMessageBox msgBox;
        msgBox.setText("Please select your library first");
        msgBox.exec();

        QString dir = QFileDialog::getExistingDirectory(0, tr("Open Directory"),	getenv("$HOME"), QFileDialog::ShowDirsOnly);

        if(dir.length() < 3) {
            QMessageBox msgBox;
            msgBox.setText("I said: \"Please select your library first\". Bye bye!");
            msgBox.exec();
            return;
        }

        else {
            m_library_path = dir;
            CSettingsStorage::getInstance()->setLibraryPath(dir);
        }
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
