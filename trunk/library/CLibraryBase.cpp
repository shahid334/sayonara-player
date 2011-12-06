/* CLibraryBase.cpp */

/* Copyright (C) 2011  Lucio Carreras
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


#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "library/ReloadThread.h"
#include "library/CLibraryBase.h"
#include "HelperStructs/id3.h"
#include <QDebug>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QFileSystemWatcher>
#include <QInputDialog>
#include <QListWidget>

CLibraryBase::CLibraryBase(QObject *parent) :
    QObject(parent)
{
	m_library_path = CSettingsStorage::getInstance()->getLibraryPath();
	m_thread = new ReloadThread();
	m_watcher = new QFileSystemWatcher();
	m_watcher->addPath(m_library_path);

	connect(m_thread, SIGNAL(finished()), this, SLOT(reload_thread_finished()));
	connect(m_watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(file_system_changed(const QString&)));
	connect(m_thread, SIGNAL(reloading_library(int)), this, SLOT(library_reloading_state_slot(int)));

}




void CLibraryBase::baseDirSelected (const QString & baseDir) {

    QStringList fileList;
    int num_files = 0;
    this -> m_reader.getFilesInsiderDirRecursive(QDir(baseDir),fileList, num_files);

    emit playlistCreated(fileList);

}



void CLibraryBase::importDirectory(QString directory){

	m_library_path = CSettingsStorage::getInstance()->getLibraryPath();

	QDir lib_dir(m_library_path);
	QDir src_dir(directory);

	QDir tmp_src_dir = src_dir;
	tmp_src_dir.cdUp();

	QString rel_src_path = tmp_src_dir.relativeFilePath(directory) + QDir::separator();
	QStringList content = lib_dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

	content.push_front("");

	bool ret;
	QString dialogText = QInputDialog::getItem(
			NULL, 															// parent
			"Select subfolder", 											// header
			"Select a subfolder from your library\n\n" + m_library_path, 	// text
			content, 														// dst (QStringList)
			0, 																// current index
			true, 															// editable
			&ret);															// everything ok?

	if(!ret) {
		QMessageBox msgBox(QMessageBox::Warning, "Error", "Sorry, there are problems with that folder." );
		msgBox.exec();
		return;
	}

	// remove first slash
	if(	dialogText.size() > 0 &&
		(dialogText.at(0) == '/' || dialogText.at(0) == '\\')){

		dialogText.remove(0, 1);
	}

	// remove last slash
	if( dialogText.size() > 0 &&
		(dialogText.at(dialogText.size()-1) == '/' || dialogText.at(dialogText.size()-1) == '\\')){

		dialogText.remove(dialogText.size(), 1);
	}


	QString target_path = m_library_path +
			QDir::separator() +
			dialogText +
			QDir::separator() +
			rel_src_path.replace(" ", "_");

	qDebug() << "Copy to " << target_path;

	QStringList files2copy;
	files2copy.push_back(src_dir.absolutePath());
	int num_files = 1;

	for(int i=0; i<num_files; i++){

		// fetch all entries of a file, maybe it's a directory
		QDir sub_dir(files2copy[i]);
		QStringList sub_files = sub_dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotDot | QDir::NoDot, QDir::DirsFirst);

		// it is a directory
		if(sub_files.size() != 0){

			// create this directory
			QDir tmp_src_dir(src_dir);
			QDir new_sub_dir( target_path + tmp_src_dir.relativeFilePath(files2copy[i]) );
			sub_dir.mkpath(new_sub_dir.path());

			// remove this "file" from list
			files2copy.removeAt(i);
			num_files--;


			// insert all files/directories from subdir
			// into array
			for(int j=0; j<sub_files.size(); j++){
				files2copy.insert(i+j, sub_dir.path() + QDir::separator() + sub_files[j] );
			}

			num_files += sub_files.size();
			i--;
		}
	}


	// copy & save to database
	vector<MetaData> v_metadata;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	for(int i=0; i<files2copy.size(); i++){

		// target path + relative src path
		QDir tmp_src_dir(src_dir);
		QString new_filename = target_path + tmp_src_dir.relativeFilePath(files2copy[i]);

		QFile f(files2copy[i]);
		f.copy(new_filename);
		if(Helper::is_soundfile(new_filename)){
			MetaData md = ID3::getMetaDataOfFile(new_filename);
			v_metadata.push_back( md );
		}
	}

	db->storeMetadata(v_metadata);
	emit reloading_library_finished();
}


void CLibraryBase::reloadLibrary(){

	m_library_path = CSettingsStorage::getInstance()->getLibraryPath();

	if(m_library_path.length() == 0) {
		QMessageBox msgBox;
		msgBox.setText("Please select your library first");
		msgBox.exec();

		QString dir = QFileDialog::getExistingDirectory(0, tr("Open Directory"),	getenv("$HOME"), QFileDialog::ShowDirsOnly);

		if(dir.length() < 3) {
			QMessageBox msgBox;
			msgBox.setText("I said: \"Please select your library first\". Bye bye!");
			msgBox.exec();
			return;
		}

		else {
			m_library_path = dir;
			CSettingsStorage::getInstance()->setLibraryPath(dir);
		}
	}


	if(m_thread->isRunning()){
		m_thread->terminate();
	}

	m_thread->set_lib_path(m_library_path);
	m_thread->start();


}

void CLibraryBase::reload_thread_finished(){

	vector<MetaData> v_metadata;
	m_thread->get_metadata(v_metadata);
	insertMetaDataIntoDB(v_metadata);
	getAllAlbums();
	getAllArtists();

	emit reloading_library_finished();
}

void CLibraryBase::library_reloading_state_slot(int percent){
	emit reloading_library(percent);
}



void CLibraryBase::insertMetaDataIntoDB(vector<MetaData>& in) {

	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	db->storeMetadata(in);
    std::vector<MetaData> data;
    db->getTracksFromDatabase(data);
    emit signalMetaDataLoaded(data);
}


void CLibraryBase::getAllArtistsAlbumsTracks(){
	loadDataFromDb();

}



void CLibraryBase::loadDataFromDb () {
    std::vector <MetaData> vec;
    CDatabaseConnector::getInstance()->getTracksFromDatabase(vec);
    if(vec.size() > 0)
    	emit signalMetaDataLoaded(vec);

    getAllAlbums();
    getAllArtists();
}


void CLibraryBase::getAllArtists(){


	vector<Artist> vec;
	CDatabaseConnector::getInstance()->getAllArtists(vec);
		if(vec.size() > 0) {

			emit allArtistsLoaded(vec);
		}

}

void CLibraryBase::getArtistsByAlbum(int album){

	vector<Artist> vec;
	CDatabaseConnector::getInstance()->getAllArtistsByAlbum(album, vec);
	if(vec.size() > 0){
		emit allArtistsLoaded(vec);
		getTracksByAlbum(album);
	}

}




void CLibraryBase::getAllAlbums(){

	vector<Album> vec;
	CDatabaseConnector::getInstance()->getAllAlbums(vec);
	if(vec.size() > 0) emit allAlbumsLoaded(vec);
}

void CLibraryBase::getAlbumsByArtist(int artist){
	vector<Album> vec;
	CDatabaseConnector::getInstance()->getAllAlbumsByArtist(artist, vec);
	if(vec.size() > 0)
		emit allAlbumsLoaded(vec);
		getTracksByArtist(artist);

}


void CLibraryBase::getTracksByAlbum(int album){

	vector<MetaData> vec;
	CDatabaseConnector::getInstance()->getAllTracksByAlbum(album, vec);
	if(vec.size() > 0)
		emit signalMetaDataLoaded(vec);

}

void CLibraryBase::getTracksByArtist(int artist){
	vector<MetaData> vec;
	CDatabaseConnector::getInstance()->getAllTracksByArtist(artist, vec);
	if(vec.size() > 0)
		emit signalMetaDataLoaded(vec);

}

void CLibraryBase::setLibraryPath(QString path){

	m_library_path = path;
}

void CLibraryBase::file_system_changed(const QString& path){
	Q_UNUSED(path);

	emit library_should_be_reloaded();


}
