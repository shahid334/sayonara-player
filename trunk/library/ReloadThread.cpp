/*
 * ReloadThread.cpp
 *
 *  Created on: Jun 19, 2011
 *      Author: luke
 */

#include "library/ReloadThread.h"
#include "library/CDirectoryReader.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/id3.h"
#include <QStringList>
#include <QDebug>
#include <vector>

using namespace std;

ReloadThread::ReloadThread() {

}

ReloadThread::~ReloadThread() {

}


void ReloadThread::run(){





	QStringList fileList;

	CDirectoryReader reader;
	int num_files = 0;
	reader.getFilesInsiderDirRecursive(QDir(_library_path), fileList, num_files);

	vector<MetaData> v_md;


	for(int i=0; i<fileList.size(); i++){

		MetaData md = ID3::getMetaDataOfFile(fileList.at(i));
		v_md.push_back(md);
	}

	CDatabaseConnector::getInstance()->deleteTracksAlbumsArtists();
	_v_metadata = v_md;


}

void ReloadThread::set_lib_path(QString library_path){
	_library_path = library_path;
}

void ReloadThread::get_metadata(vector<MetaData>& md){
	md = _v_metadata;
}
