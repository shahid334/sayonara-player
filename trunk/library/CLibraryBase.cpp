#include "HelperStructs/CSettingsStorage.h"
#include "library/ReloadThread.h"
#include "library/CLibraryBase.h"
#include "HelperStructs/id3.h"
#include <QDebug>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QFileSystemWatcher>

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
