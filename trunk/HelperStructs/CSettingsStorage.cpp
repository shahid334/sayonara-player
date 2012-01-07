/* CSettingsStorage.cpp */

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


#include "HelperStructs/Helper.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/CSettingsStorage.h"
#include "DatabaseAccess/CDatabaseConnector.h"


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

            qDebug() << "cp " << Helper::getIconPath() + m_dbSource << " " << dir.absolutePath() + QDir::separator() + m_dbFile;
            if (QFile::copy(Helper::getIconPath() + m_dbSource, dir.absolutePath() + QDir::separator()+ m_dbFile)) {
               qDebug() << "DB File has been copied to " <<   dir.absolutePath() + QDir::separator()+ m_dbFile;
            }
            else {
               // qFatal(QString ("Were not able to copy file" + dir.absolutePath() + QDir::separator()+ m_dbFile).toStdString().c_str());
                qFatal("Were not able to copy file %s", QString(dir.absolutePath() + QDir::separator()+ m_dbFile).toStdString().c_str());


            }
        }
    }
    else {
        qFatal("We are not able to create the directory %s", QString(QDir::homePath() + m_sayonaraPath).toStdString().c_str() );
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
		
#ifdef Q_OS_WIN   
	QString dir = QDir::homePath() + QString("\\.Sayonara\\player.db");
    return dir;
#else

    //dir.cd(Helper::getIconPath());
    return QDir::homePath() + QDir::separator() + m_sayonaraPath + QDir::separator() + m_dbFile;
#endif


}



//TODO hash
QPair<QString, QString> CSettingsStorage::getLastFMNameAndPW () { return this -> m_lastFm; }
void CSettingsStorage::getLastFMNameAndPW (QString & name, QString & pw) {
    name= this -> m_lastFm.first;
    pw= this -> m_lastFm.second;
}
void CSettingsStorage::setLastFMNameAndPW (const QString & name,const QString & pw) {
    this -> m_lastFm.first = name;
    this -> m_lastFm.second = pw;
}


EQ_Setting CSettingsStorage::getCustomEqualizer(){ 
	if(m_vec_eqSettings.size() == 0) {
		EQ_Setting setting;
		return setting;
	}
	return this->m_vec_eqSettings[m_vec_eqSettings.size()-1]; }
void CSettingsStorage::getEqualizerSettings(vector<EQ_Setting>& vec){vec = this->m_vec_eqSettings;}
void CSettingsStorage::setEqualizerSettings(const vector<EQ_Setting>& vec){

	m_vec_eqSettings.clear();
	for(uint i=0; i<vec.size(); i++){
		m_vec_eqSettings.push_back(vec[i]);
	}
}


int CSettingsStorage::getVolume(){ return m_volume; }
void CSettingsStorage::setVolume(int vol){ if(vol >= 0 && vol <= 100) m_volume = vol; }

void CSettingsStorage::setLastEqualizer(int eq_idx){ m_last_eq = eq_idx; }
int CSettingsStorage::getLastEqualizer(){ return m_last_eq;}

QString CSettingsStorage::getLibraryPath(){return m_library_path;}
void CSettingsStorage::setLibraryPath(QString path){m_library_path = path;}

QSize CSettingsStorage::getPlayerSize(){ return m_player_size; }
void CSettingsStorage::setPlayerSize(QSize size){ m_player_size = size; }

QString CSettingsStorage::getPlaylist(){
	return m_playlist;
}
void CSettingsStorage::setPlaylist(QString playlist){
	m_playlist = playlist;
}

bool CSettingsStorage::getLoadPlaylist(){
	return m_loadPlaylist;
}

void CSettingsStorage::setLoadPlaylist(bool b){
	m_loadPlaylist = b;
}

void CSettingsStorage::setPlaylistMode(const Playlist_Mode& plmode){
	m_playlistmode = plmode;
}

Playlist_Mode CSettingsStorage::getPlaylistMode(){
	return m_playlistmode;
}

void CSettingsStorage::setPlayerStyle(int style){
	m_style = style;
}

int CSettingsStorage::getPlayerStyle(){
	return m_style;
}

void CSettingsStorage::setShowNotifications(bool active){
	m_show_notifications = active;
}

bool CSettingsStorage::getShowNotification(){
	return m_show_notifications;
}
