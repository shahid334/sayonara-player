#include "library/ImportFolderThread.h"
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
    if(_src_dir.size() == 0) return;
    _filelist.clear();
    _map.clear();

    QDir dir(_src_dir);
    int n_files;
    CDirectoryReader reader;
    reader.getFilesInsiderDirRecursive(dir, _filelist, n_files);

    int i=0;
    foreach(QString filepath, _filelist){
	if(_cancelled) break;
        int percent = (i++ * 100000) / _filelist.size();

        emit sig_progress(percent / 1000);
        if(!Helper::is_soundfile(filepath)) continue;

        MetaData md;
        md.filepath = filepath;

        if(!ID3::getMetaDataOfFile(md)) continue;
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

void ImportFolderThread::set_src_dir(QString dir){
    _src_dir = dir;
}

void ImportFolderThread::get_filelist(QStringList& lst){
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
