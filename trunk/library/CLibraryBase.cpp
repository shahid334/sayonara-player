#include "library/CLibraryBase.h"
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
