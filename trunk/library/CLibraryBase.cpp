#include "library/CLibraryBase.h"
#include "HelperStructs/id3.h"
#include <QDebug>

CLibraryBase::CLibraryBase(QObject *parent) :
    QObject(parent)
{

}


void CLibraryBase::baseDirSelected (const QString & baseDir) {
    qDebug() << "Base Dir: " << baseDir;
    QStringList fileList;
    this -> m_reader.getFilesInsiderDirRecursive(QDir(baseDir),fileList);

    /* absolute paths */
    qDebug() << fileList;
    emit playlistCreated(fileList);

}

void CLibraryBase::reloadLibrary(){
	CDatabaseConnector::getInstance()->deleteTracksAlbumsArtists();
//	QString libraryPath = CDatabaseConnector::getInstance()->getLibraryPath();
	QStringList fileList;
	vector<MetaData> v_metadata;
//  this->m_reader.getFilesInsiderDirRecursive(QDir(libraryPath), fileList);
	if(m_library_path.length() == 0) return;
	this->m_reader.getFilesInsiderDirRecursive(QDir(m_library_path), fileList);

	uint todo = fileList.size();

	for(int i=0; i<fileList.size(); i++){
		MetaData md = ID3::getMetaDataOfFile(fileList.at(i));
		v_metadata.push_back(md);

		emit mp3s_loaded_signal((int)(i * 100.0 / todo));
	}

	emit mp3s_loaded_signal(100);

	insertMetaDataIntoDB(v_metadata);
	getAllAlbums();
	getAllArtists();
}

void CLibraryBase::insertMetaDataIntoDB(vector<MetaData>& in) {
	CDatabaseConnector::getInstance()->storeMetadata(in);
    std::vector<MetaData> data;
    CDatabaseConnector::getInstance()->getTracksFromDatabase(data);
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
