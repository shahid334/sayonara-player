/* CopyFolderThread.cpp */

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



#include "library/threads/ImportCopyThread.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"

#include <QString>

#include <QDir>
#include <QFile>

ImportCopyThread::ImportCopyThread(QObject *parent) :
    QThread(parent)
{
}



void ImportCopyThread::emit_percent(int i, int n){
	int percent = (i * 100000) / n;
	_percent = percent / 1000;
        emit sig_progress(_percent);
	
}


QString _calc_target_path(QString src_dir, QString lib_dir, QString chosen_dir){

	QString src_folder_name, dir;
	if(src_dir.size() > 0){

		// /home/user/dir/ -> /home/user, dir
	        Helper::split_filename(src_dir, dir, src_folder_name);
	}

        else src_folder_name = "";


	// chosen dir = cd
	QString chosen_item_str = chosen_dir + QDir::separator();
        if(chosen_dir.size() == 0) chosen_item_str = "";

	// /home/user/Music/cd/
        QString target_path = lib_dir + QDir::separator() +
                    chosen_item_str +
                    src_folder_name;

	return target_path;
}


void ImportCopyThread::copy(){
    _v_md.clear();
    _n_files = _files.size();
    _copied_files = 0;
    _lst_copied_files.clear();
    _created_dirs.clear();
    _percent = 0;
    int i=0;

    foreach(QString filename, _files){
        if(_cancelled) return;

	// src_dir = closest dir to root where to export
	// src_dir = /home/user/dir
	QString src_dir = _pd_map.value(filename);
	QString new_target_path;

	
	// here we handle files in a folder
        if(src_dir.size() > 0){
                
                // target path = /home/user/Music/chosen_dir/dir
                QString target_path = _calc_target_path(src_dir, _lib_dir, _chosen_dir);

	        // extract folders between the files and src dir and create directories
       	 	// /home/user/dir/subfolder/subfolder2/bla.mp3 -> subfolder/subfolder2
       	 	QString folder = Helper::get_parent_folder(filename);
        	folder.remove(src_dir);

		// /home/user/Music/chosen_dir/dir/subfolder/subfolder2
		new_target_path = target_path + QDir::separator() + folder;
	}

	else{
		new_target_path = _calc_target_path("", _lib_dir, _chosen_dir);
	}

	// create that folder
       	if(!QFile::exists(new_target_path)){
		 _created_dirs << new_target_path;
            	QDir::root().mkpath(new_target_path);
        }

        // copy file
        QFile f(filename);
        QString filename_wo_folder = Helper::get_filename_of_path(filename);
        QString new_filename = new_target_path + QDir::separator() + filename_wo_folder;
        bool existed = QFile::exists(new_filename);
        bool copied = f.copy(new_filename);
        if(copied) {

            _copied_files++;

            if(!existed)
                _lst_copied_files << new_filename;

        }

        // insert into vector for db
	emit_percent(i++, _files.size());

        if(!Helper::is_soundfile(filename)) continue;
        else if(Helper::is_soundfile(filename)){
            if(!copied) continue;
        }

        MetaData md;
        bool got_md = _md_map.keys().contains(filename);

        if( got_md ){
            md = _md_map.value(filename);
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

void ImportCopyThread::rollback(){


    int n_operations = _lst_copied_files.size() + _created_dirs.size();
    int n_ops_todo = n_operations;
    int percent;

    _v_md.clear();
    foreach(QString f, _lst_copied_files){
        QFile file(f);
        file.remove();
        percent = ((n_ops_todo--) * (_percent * 1000)) / (n_operations);

        emit sig_progress(percent/ 1000);
    }

    QDir dir(_lib_dir);
    foreach(QString d, _created_dirs){
        d.remove(_lib_dir);
        while(d.startsWith(QDir::separator())) d.remove(0,1);


        dir.rmpath(d);
    	percent = ((n_ops_todo--) * (_percent * 1000)) / (n_operations);

        emit sig_progress(percent/ 1000);
    }

    _percent = 0;
    _n_files = 0;
    _copied_files = 0;

    _lst_copied_files.clear();
    _created_dirs.clear();

}






void ImportCopyThread::run(){

    _cancelled = false;
    if(_mode == IMPORT_COPY_THREAD_COPY) copy();
    else if(_mode == IMPORT_COPY_THREAD_ROLLBACK) rollback();

}


void ImportCopyThread::set_vars(QString chosen_dir, QStringList &files, QMap<QString, MetaData>& md_map, QMap<QString, QString>& pd_map){

    _chosen_dir = chosen_dir;
    _lib_dir = CSettingsStorage::getInstance()->getLibraryPath();
    _files = files;
    _md_map = md_map;
    _pd_map = pd_map;
}

void ImportCopyThread::set_cancelled(){
    _cancelled = true;
}

bool ImportCopyThread::get_cancelled(){
    return _cancelled;
}

int ImportCopyThread::get_n_files(){
    return _n_files;
}

int ImportCopyThread::get_copied_files(){
    return _copied_files;
}

void ImportCopyThread::get_metadata(MetaDataList& v_md){
    v_md = _v_md;
}

void ImportCopyThread::set_mode(int mode){
    _mode = mode;
}

int ImportCopyThread::get_mode(){
	return _mode;
}

