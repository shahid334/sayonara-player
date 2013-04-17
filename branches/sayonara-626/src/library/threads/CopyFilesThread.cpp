#include "HelperStructs/Helper.h"
#include "library/threads/CopyFilesThread.h"

#include <QString>

#include <QDir>
#include <QFile>

CopyFilesThread::CopyFilesThread(QObject *parent) :
    ImportCopyThread(parent)
{
}


void CopyFilesThread::copy(){
    _v_md.clear();
    _n_files = _files.size();
    _copied_files = 0;
    _lst_copied_files.clear();

    QDir lib_dir(_lib_dir);
    lib_dir.mkpath(_chosen_dir);

    int i = 0;

    foreach(QString filename, _files){
	if(!QFile::exists(filename)) continue;

	QFile f(filename);
	QString path, name;
	Helper::split_filename(filename, path, name);
	QString target_path = _lib_dir + QDir::separator() + _chosen_dir;
	QString new_filename = target_path + QDir::separator() + name;
	
	bool existed = QFile::exists(new_filename);
	bool copied = f.copy(filename, new_filename);

	if(copied){
	    if(!existed)
                _lst_copied_files << new_filename;

            _copied_files++;
	}

        int percent = (i++ * 100000) / _files.size();
        _percent = percent / 1000;
        emit sig_progress(_percent);

        if(!Helper::is_soundfile(filename)) continue;
        else if(Helper::is_soundfile(filename)){
            if(!copied) continue;
        }

	bool got_md = _map.keys().contains(filename);

        if( got_md ){
            MetaData md = _map.value(filename);
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
    

void CopyFilesThread::rollback(){

    int n_operations = _lst_copied_files.size();
    int n_ops_todo = n_operations;
    int percent;

    _v_md.clear();
    foreach(QString f, _lst_copied_files){
        QFile file(f);
        file.remove();
        percent = ((n_ops_todo--) * (_percent * 1000)) / (n_operations);

        emit sig_progress(percent/ 1000);
    }

    _percent = 0;
    _n_files = 0;
    _copied_files = 0;
    _lst_copied_files.clear();

}

