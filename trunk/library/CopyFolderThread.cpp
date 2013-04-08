#include "library/CopyFolderThread.h"
#include "HelperStructs/Helper.h"

#include <QString>

#include <QDir>
#include <QFile>

CopyFolderThread::CopyFolderThread(QObject *parent) :
    QThread(parent)
{
}


void CopyFolderThread::copy(){
    _v_md.clear();
    _n_files = _files.size();
    _copied_files = 0;
    _lst_copied_files.clear();
    _created_dirs.clear();

    QDir lib_dir(_lib_dir);
    // extract src_folder_name = the name of the folder to import without stuff in front of it
    QString src_folder_name = _src_dir;
    src_folder_name.remove(Helper::get_parent_folder(src_folder_name));
    while(src_folder_name.startsWith(QDir::separator())) src_folder_name.remove(0,1);
    while(src_folder_name.endsWith(QDir::separator())) src_folder_name.remove(src_folder_name.size() - 1, 1);

    QString chosen_item_str = _chosen_dir + QDir::separator();
    if(_chosen_dir.size() == 0) chosen_item_str = "";

    QString target_path = _lib_dir + QDir::separator() +
            chosen_item_str +
            src_folder_name;

    // and create folder in library
    if(!QFile::exists(target_path))
        _created_dirs << target_path;

    lib_dir.mkpath(chosen_item_str + src_folder_name);




    QDir target_dir(target_path);

    int i=0;
    foreach(QString filename, _files){
        if(_cancelled) return;

        // extract folders and create directories

        QString folder = Helper::get_parent_folder(filename);
        folder.remove(_src_dir);
        while(folder.startsWith(QDir::separator())) folder.remove(0,1);
        while(folder.endsWith(QDir::separator())) folder.remove(folder.size() - 1, 1);

        // file was in the root dir
        if(!folder.size() == 0){
            if(!QFile::exists(target_path + QDir::separator() + folder)) _created_dirs << target_path + QDir::separator() + folder;
            target_dir.mkpath(folder);
        }

        QString new_target_path = target_path + QDir::separator() + folder;

        // copy file
        QFile f(filename);
        QString filename_wo_folder = Helper::get_filename_of_path(filename);
        QString new_filename = new_target_path + QDir::separator() + filename_wo_folder;
        bool copied = f.copy(new_filename);
        if(copied) {
            _copied_files ++;

            if(!QFile::exists(new_filename))
                _lst_copied_files << new_filename;
        }

        // insert to db
        int percent = (i++ * 100000) / _files.size();
        emit sig_progress(percent / 1000);

        if(!Helper::is_soundfile(filename)) continue;
        else if(Helper::is_soundfile(filename)){
            if(!copied) continue;
        }

        MetaData md;
        bool got_md = _map.keys().contains(filename);

        if( got_md ){
            md = _map.value(filename);
            md.filepath = new_filename;
            _v_md.push_back( md );
        }

        else {
            _copied_files --;
            QFile f(new_filename);
            f.remove();
        }
    }
}

void CopyFolderThread::rollback(){

    _v_md.clear();
    foreach(QString f, _lst_copied_files){
        QFile file(f);
        file.remove();
    }

    foreach(QString f, _created_dirs){
        QFile file(f);
        file.remove();
    }

    _n_files = 0;
    _copied_files = 0;

    _lst_copied_files.clear();
    _created_dirs.clear();

}


void CopyFolderThread::run(){

    _cancelled = false;
    if(_mode == COPY_FOLDER_THREAD_COPY) copy();
    else if(_mode == COPY_FOLDER_THREAD_ROLLBACK) rollback();

}


void CopyFolderThread::set_vars(QString chosen_dir, QString lib_dir, QString src_dir, QStringList &files, QMap<QString, MetaData> &map){

    _chosen_dir = chosen_dir;
    _lib_dir = lib_dir;
    _src_dir = src_dir;
    _files = files;

    _map = map;
}

void CopyFolderThread::set_cancelled(){
    _cancelled = true;
}

bool CopyFolderThread::get_cancelled(){
    return _cancelled;
}

int CopyFolderThread::get_n_files(){
    return _n_files;
}

int CopyFolderThread::get_copied_files(){
    return _copied_files;
}

void CopyFolderThread::get_metadata(MetaDataList& v_md){
    v_md = _v_md;
}

void CopyFolderThread::set_mode(int mode){
    _mode = mode;
}
