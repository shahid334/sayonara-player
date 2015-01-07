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



ImportCachingThread::ImportCachingThread(QObject *parent) :
    QThread(parent)
{


}

void ImportCachingThread::run() {

	_cancelled = false;
    _may_terminate = false;
    _filelist.clear();

	_v_md.clear();


	// file may be a directory or a real file,
	// nevertheless, we want all real files files
    foreach(QString file, _files) {

        if(_cancelled) break;

        // file is a directory
		// if "import dir" was selected
        if(Helper::is_dir(file)) {

			QDir src_dir(file);

			CDirectoryReader reader;
			QStringList inner_files;

			reader.get_files_in_dir_rec(src_dir, inner_files);

            // save from which folders these files are
			foreach(QString inner_file, inner_files) {
				_filelist.push_back(inner_file);
            }
        }

        // file is standard file
		// if import files was selected
        else if(Helper::is_file(file)) {
            _filelist.push_back(file);
        }
    }

    int i=0;

    foreach(QString filepath, _filelist) {

        if(_cancelled) break;

		int percent = (i++ * 100000) / _filelist.size();

        emit sig_progress(percent / 1000);
        if(!Helper::is_soundfile(filepath)) continue;

		MetaData md;
        md.filepath = filepath;

        if(!ID3::getMetaDataOfFile(md)) continue;

		_v_md << md;
		_md_map[md.filepath] = md;
    }

    if(_cancelled) {
		_md_map.clear();
		_v_md.clear();
    }

    emit sig_done();

    while(!_may_terminate && !_cancelled) {
		Helper::sleep_ms(10);
    }
}

void ImportCachingThread::update_metadata(const MetaDataList &old_md, const MetaDataList &new_md){

	MetaDataList v_md_old = old_md;
	MetaDataList v_md_new = new_md;

	for(int i=0; i<_v_md.size(); i++){

		int found_at = -1;

		for(int j=0; j<v_md_old.size(); j++){

			if(_v_md[i].filepath == v_md_old[j].filepath){
				_v_md[i] = v_md_new[j];
				found_at = j;
			}
		}

		if(found_at != -1){
			v_md_old.remove(found_at);
			v_md_new.remove(found_at);
		}
	}

	for(int i=0; i<old_md.size(); i++){
		QString filepath = old_md[i].filepath;
		_md_map[ filepath ] = new_md[i];
	}
}

void ImportCachingThread::set_filelist(const QStringList& lst) {
    _files = lst;
}

QStringList ImportCachingThread::get_extracted_files() {
	return _filelist;
}

void ImportCachingThread::set_may_terminate(bool b) {
    _may_terminate = b;
}

void ImportCachingThread::set_cancelled() {
    _cancelled = true;
}

QMap<QString, MetaData> ImportCachingThread::get_md_map() {
	if(_cancelled) _md_map.clear();
	return _md_map;
}

MetaDataList ImportCachingThread::get_metadata(){
	if(_cancelled) _v_md.clear();
	return _v_md;
}

int ImportCachingThread::get_n_tracks() {
	return _v_md.size();
}
