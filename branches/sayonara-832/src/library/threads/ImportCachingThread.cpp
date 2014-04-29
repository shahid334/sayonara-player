/* ImportCachingThread.cpp */

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



#include "library/threads/ImportCachingThread.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Helper.h"
#include <QDir>
#include <QMap>
#include <QDebug>


ImportCachingThread::ImportCachingThread(QObject *parent) :
    QThread(parent)
{


}

void ImportCachingThread::run(){
    _cancelled = false;
    _may_terminate = false;
    _filelist.clear();
    _md_map.clear();
    _pd_map.clear();

    foreach(QString file, _files){

        if(_cancelled) break;

        // file is a directory
        if(Helper::is_dir(file)){

            QDir src_dir(file);
            int n_files = 0;
            CDirectoryReader reader;
            QStringList tmp_filelist;
            reader.getFilesInsiderDirRecursive(src_dir, tmp_filelist, n_files);

            // save from which folders these files are
            foreach(QString tmpFile, tmp_filelist){
                _pd_map[tmpFile] = file;
                _filelist.push_back(tmpFile);
            }
        }

        // file is standard file
        else if(Helper::is_file(file)){
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

        if(!ID3::getMetaDataOfFile(md)) continue;
        _md_map[filepath] = md;
    }

    if(_cancelled){
        _md_map.clear();
        _pd_map.clear();
    }

    emit sig_done();
    while(!_may_terminate && !_cancelled){
        usleep(10000);
    }
}

void ImportCachingThread::set_filelist(const QStringList& lst){
    _files = lst;
}

void ImportCachingThread::get_extracted_files(QStringList& lst){
    lst =  _filelist;
}

void ImportCachingThread::set_may_terminate(bool b){
    _may_terminate = b;
}

void ImportCachingThread::set_cancelled(){
    _cancelled = true;
}

void ImportCachingThread::get_md_map(QMap<QString, MetaData> &map){
    if(_cancelled) _md_map.clear();
    map = _md_map;
}

void ImportCachingThread::get_pd_map(QMap<QString, QString> &map){
    if(_cancelled) _pd_map.clear();
    map = _pd_map;
}

int ImportCachingThread::get_n_tracks(){
    return _md_map.keys().size();
}
