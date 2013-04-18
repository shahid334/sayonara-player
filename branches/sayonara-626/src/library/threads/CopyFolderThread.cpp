/* CopyFolderThread.cpp */

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



#include "library/threads/CopyFolderThread.h"
#include "HelperStructs/Helper.h"

#include <QString>

#include <QDir>
#include <QFile>

CopyFolderThread::CopyFolderThread(QObject *parent) :
    ImportCopyThread(parent)
{
}

int _emit_percent(int i, int n){
        int ret_val;
	int percent = (i * 100000) / n;
	ret_val = percent / 1000;
        emit sig_progress(ret_val);
	return ret_val;
}


QString _calc_target_path(QString src_dir, QString lib_dir, QString chosen_dir){
	QString src_folder_name, dir;
        Helper::split_filename(src_dir, dir, src_folder_name);

	 QString chosen_item_str = chosen_dir + QDir::separator();
        if(chosen_dir.size() == 0) chosen_item_str = "";

         QString target_path = lib_dir + QDir::separator() +
                    chosen_item_str +
                    src_folder_name;

	return target_path;
}


void CopyFolderThread::copy(){
    _v_md.clear();
    _n_files = _files.size();
    _copied_files = 0;
    _lst_copied_files.clear();
    _created_dirs.clear();
    _percent = 0;

    QDir lib_dir(_lib_dir);
    

    // Create target dir in library
    foreach(QString dir, _pd_map.values()){
        if(dir.size() == 0) continue; // some files are imported directly

        QString target_path = _calc_target_path( dir, _lib_dir, _chosen_dir);
    
        if(!QFile::exists(target_path))
            _created_dirs << target_path;

        QDir root = QDir::root();
        bool success = root.mkpath(target_path);
    }
    // -- end

    QDir target_dir(target_path);

    int i=0;
    foreach(QString filename, _files){
        if(_cancelled) return;

	QString src_dir = _pd_map.value(filename);
	QString new_target_path;
        if(src_dir.size() > 0){
       
	        // extract folders between the files and src dir and create directories
       	 	// /home/user/folder/subfolder/subfolder2/bla.mp3 -> subfolder/subfolder2
       	 	QString folder = Helper::get_parent_folder(filename);
        	folder.remove(_src_dir);
        	while(folder.startsWith(QDir::separator())) folder.remove(0,1);
        	while(folder.endsWith(QDir::separator())) folder.remove(folder.size() - 1, 1);

		new_target_path = target_path + QDir::separator() + folder;
        	// file was in the root dir

        	if(!folder.size() == 0){
            		if(!QFile::exists(new_target_path)) _created_dirs << new_target_path;
            		target_dir.mkpath(folder);
        	}
	}

	else{
		new_target_path = _calc_target_path("", _lib_dir, _chosen_dir);
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

        // insert to db
	_percent = emit_percent(i++, _files.size());

        if(!Helper::is_soundfile(filename)) continue;
        else if(Helper::is_soundfile(filename)){
            if(!copied) continue;
        }

        MetaData md;
        bool got_md = _md_map.keys().contains(filename);

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


