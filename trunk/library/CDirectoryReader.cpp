#include "library/CDirectoryReader.h"
#include <QDebug>
#include <QDir>

CDirectoryReader::CDirectoryReader () {
    this -> m_filters.push_back("*.mp3");
}
CDirectoryReader::~CDirectoryReader () {

}


void CDirectoryReader::setFilter (const QStringList & filter) {
    qDebug() << "Setting filter to: " << filter;
    this -> m_filters = filter;
}

void CDirectoryReader::getFilesInsiderDirRecursive (QDir baseDir, QStringList & files) {

    QStringList dirs;
	baseDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dirs = baseDir.entryList();

    foreach (QString dir, dirs) {

    	baseDir.cd(dir);
        this -> getFilesInsiderDirRecursive(baseDir, files);
        baseDir.cd("..");
    }
    QStringList tmp;
    baseDir.setFilter(QDir::Files);
    baseDir.setNameFilters(this -> m_filters);
    this -> getFilesInsideDirectory (baseDir, tmp);

    // absolute paths
    files += tmp;
}

void CDirectoryReader::getFilesInsideDirectory (QDir baseDir, QStringList & files) {
    baseDir.setFilter(QDir::Files);
    baseDir.setNameFilters(this -> m_filters);
    QStringList tmp;
    tmp = baseDir.entryList();
    foreach (QString f, tmp) {
        files.push_back(baseDir.absoluteFilePath(f));
    }
}
