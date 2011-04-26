#include "library/CLibraryBase.h"
#include <QDebug>

CLibraryBase::CLibraryBase(QObject *parent) :
    QObject(parent), m_database (this)
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

void CLibraryBase::insertMetaDataIntoDB(vector<MetaData>& in) {
    m_database.storeMetadata(in);
    std::vector<MetaData> data;
    m_database.getTracksFromDatabase(data);
    emit signalMetaDataLoaded(data);
}


void CLibraryBase::loadDataFromDb () {
    std::vector <MetaData> data;
    m_database.getTracksFromDatabase(data);
    emit signalMetaDataLoaded(data);

    getAllAlbums();
    getAllArtists();
}


void CLibraryBase::getAllArtists(){


	vector<Artist> vec;
		m_database.getAllArtists(vec);
		if(vec.size() > 0) {

			emit allArtistsLoaded(vec);
		}

}

void CLibraryBase::getArtistsByAlbumName(QString album){

}

void CLibraryBase::getAllAlbums(){

	vector<Album> vec;
	m_database.getAllAlbums(vec);
	if(vec.size() > 0) emit allAlbumsLoaded(vec);
}

void CLibraryBase::getAlbumsByArtistName(QString artist){


}


void CLibraryBase::getTracksByAlbumAndArtistName(QString album, QString artist){


}

