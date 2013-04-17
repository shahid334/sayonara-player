/* ImportFolderThread.cpp */

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



#include "library/threads/ImportFolderThread.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Helper.h"
#include <QDir>
#include <QMap>
#include <QDebug>


ImportFolderThread::ImportFolderThread(QObject *parent) :
    QThread(parent)
{


}

void ImportFolderThread::run(){
    _cancelled = false;
    _may_terminate = false;
    _filelist.clear();
    _map.clear();

    foreach(QString file, _files){
	qDebug() << "Controlling " << file;
	if(Helper::is_dir(file)){
	
            QDir src_dir(file);
	    qDebug() << "is dir";
	    int n_files = 0;
	    CDirectoryReader reader;
	    QStringList tmp_filelist;
	    reader.getFilesInsiderDirRecursive(src_dir, tmp_filelist, n_files);
            qDebug() << "Could extract " << n_files << ", " << tmp_filelist.size() << " files out of it";
	    
            _filelist.append(tmp_filelist);
	}

        else if(Helper::is_file(file)){
            qDebug() << "is file";
	    _filelist.push_back(file);
	}
    }

    int i=0;
    foreach(QString filepath, _filelist){
	if(_cancelled) break;
        int percent = (i++ * 100000) / _filelist.size();

        emit sig_progress(percent / 1000);
        if(!Helper::is_soundfile(filepath)) continue;

        MetaData md;
        md.filepath = filepath;
        qDebug() << "Accessing " << filepath;

        if(!ID3::getMetaDataOfFile(md)) {
		qDebug() << "Could not get Metadata";
		continue;
         }
        _map[filepath] = md;
    }

    if(_cancelled){
	_map.clear();
    }

    emit sig_done();
    while(!_may_terminate && !_cancelled){
        usleep(10000);
    }
}

void ImportFolderThread::set_filelist(QStringList& lst){
    _files = lst;
}

void ImportFolderThread::get_extracted_files(QStringList& lst){
    lst =  _filelist;
}

void ImportFolderThread::set_may_terminate(bool b){
    _may_terminate = b;
}

void ImportFolderThread::set_cancelled(){
    _cancelled = true;
}

void ImportFolderThread::get_md_map(QMap<QString, MetaData> &map){
    if(_cancelled) _map.clear();
    map = _map;
}

int ImportFolderThread::get_n_tracks(){
    return _map.keys().size();
}
