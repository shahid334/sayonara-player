#include "CSettingsStorage.h"
#include <QFile>
#include <QDir>
#include <QDebug>



CSettingsStorage * CSettingsStorage::getInstance() {
    static CSettingsStorage inst;
    return &inst;
}

bool CSettingsStorage::isRunFirstTime () {
    bool ret = false;
    QDir dir = QDir::homePath();
    if (dir.cd(this -> m_sayonaraPath)) {
        qDebug() << dir.path() + "/" + m_dbFile;
        QFile f (dir.path() + "/" + m_dbFile);
        ret = f.exists();
    }
    return ret;
}

bool CSettingsStorage::runFirstTime (bool deleteOld) {
    bool ret = false;
    QDir dir = QDir::homePath();
    ret = dir.cd(this -> m_sayonaraPath);
    if (!ret) {
        //dir does not exist, so we are creating it...
        ret = dir.mkdir(this -> m_sayonaraPath);
        if (ret) {
            ret = dir.cd(this -> m_sayonaraPath);
        }
    }
    //if ret is still not true we are not able to create the directory
    if (ret) {
        QFile dest (dir.absolutePath() + QDir::separator() + m_dbFile);
        if (deleteOld) {
            if (dest.exists()) {
                qDebug() << "Deleting: " << dir.absolutePath() + QDir::separator() + m_dbFile;
                dir.remove(m_dbFile);
            }
        }
        if (!dest.exists()) {
            qDebug() << "cp " << QDir::currentPath() + QDir::separator() + m_dbSource << " " << dir.absolutePath() + QDir::separator() + m_dbFile;
            if (QFile::copy(QDir::currentPath() + QDir::separator() +m_dbSource, dir.absolutePath() + QDir::separator()+ m_dbFile)) {
               qDebug() << "DB File has been copied to " <<   dir.absolutePath() + QDir::separator()+ m_dbFile;
            }
            else {
                qFatal(QString ("Were not able to copy file" + dir.absolutePath() + QDir::separator()+ m_dbFile).toStdString().c_str());
            }
        }
    }
    else {
        qFatal(QString ("We are not able to create the directory " + QDir::homePath() + m_sayonaraPath).toStdString().c_str());
    }
    if (!dir.exists()) {

    }
    return ret;
}


CSettingsStorage::~CSettingsStorage () {
    //is called
}


void CSettingsStorage::init() {
    m_dbFile = "player.db";
    m_dbSource = "empty.db";
    m_sayonaraPath = ".Sayonara";
}

QString CSettingsStorage::getDBFileName () {
    QDir dir = QDir::homePath();
    dir.cd(this -> m_sayonaraPath);
    return dir.absolutePath() + QDir::separator() + m_dbFile;
}

//TODO hash
QPair<QString, QString> CSettingsStorage::getLastFMNameAndPW () {
    return this -> m_lastFm;
}

void CSettingsStorage::getLastFMNameAndPW (QString & name, QString & pw) {
    name= this -> m_lastFm.first;
    pw= this -> m_lastFm.second;
}

void CSettingsStorage::setLastFMNameAndPW (const QString & name,const QString & pw) {
    this -> m_lastFm.first = name;
    this -> m_lastFm.second = pw;
}

