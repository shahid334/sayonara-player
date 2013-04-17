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



#include "library/threads/ImportCopyThread.h"
#include "HelperStructs/Helper.h"

#include <QString>

#include <QDir>
#include <QFile>

ImportCopyThread::ImportCopyThread(QObject *parent) :
    QThread(parent)
{
}


void ImportCopyThread::run(){

    _cancelled = false;
    if(_mode == IMPORT_COPY_THREAD_COPY) copy();
    else if(_mode == IMPORT_COPY_THREAD_ROLLBACK) rollback();

}


void ImportCopyThread::set_vars(QString chosen_dir, QString lib_dir, QStringList &files, QMap<QString, MetaData> &map){

    _chosen_dir = chosen_dir;
    _lib_dir = lib_dir;
    _files = files;
    _map = map;
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
