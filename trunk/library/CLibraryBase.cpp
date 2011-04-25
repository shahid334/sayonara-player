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
}


void CLibraryBase::loadDataFromDb () {
    std::vector <MetaData> data;
    m_database.getTracksFromDatabase(data);
    emit signalMetaDataLoaded(data);
}
