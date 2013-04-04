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


SettingsThread::SettingsThread(){
	_settings = CSettingsStorage::getInstance();
	_goon = true;
	
}

SettingsThread::~SettingsThread(){}

void SettingsThread::run(){

	while(_goon){
		usleep(5000000);
		if(_settings->get_sth_changed()){
			_settings->set_sth_changed(false);
			_settings->save_all();
		}
	}

	qDebug() << "thread finished";
}

void SettingsThread::stop(){
	_goon = false;

}


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


CSettingsStorage::~CSettingsStorage () {
    //is called
}

void CSettingsStorage::init() {
    m_dbFile = "player.db";
    m_dbSource = "empty.db";
    m_sayonaraPath = ".Sayonara";
}

QString CSettingsStorage::getDBFileName () {
		
    return Helper::getSayonaraPath() + QDir::separator() + m_dbFile;
}



void CSettingsStorage::set_sth_changed(bool b){
	_sth_changed = b;
}

bool CSettingsStorage::get_sth_changed(){
	return _sth_changed;
}

void CSettingsStorage::save_all(){
	emit sig_save_all();
}


QString CSettingsStorage::getVersion(){
	return _version;
}

void CSettingsStorage::setVersion(QString version){
	_version = version;
    _sth_changed = true;
}

bool CSettingsStorage::getLastFMActive(){ return m_lfm_active; }

void CSettingsStorage::setLastFMActive(bool b){
    m_lfm_active = b;
    _sth_changed = true;
}

QPair<QString, QString> CSettingsStorage::getLastFMNameAndPW () { return this -> m_lastFm; }
void CSettingsStorage::getLastFMNameAndPW (QString & name, QString & pw) {
    name= this -> m_lastFm.first;
    pw= this -> m_lastFm.second;
    _sth_changed = true;
}

void CSettingsStorage::setLastFMNameAndPW (const QString & name,const QString & pw) {
    this -> m_lastFm.first = name;
    this -> m_lastFm.second = pw;
    _sth_changed = true;
}

void CSettingsStorage::setLastFMCorrections(bool b){ 
	m_lfm_corrections = b; 
    _sth_changed = true;
}

bool CSettingsStorage::getLastFMCorrections(){return m_lfm_corrections;}

bool CSettingsStorage::getLastFMShowErrors(){ return m_lfm_show_errors; }


void CSettingsStorage::setLastFMShowErrors(bool b){ 
	m_lfm_show_errors = b; 
    _sth_changed = true;
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
    _sth_changed = true;
}


int CSettingsStorage::getVolume(){ return m_volume; }
void CSettingsStorage::setVolume(int vol){
    if(vol < 0 || vol > 100) return;
    m_volume = vol;
    _sth_changed = true;
}

void CSettingsStorage::setLastEqualizer(int eq_idx){ 
	m_last_eq = eq_idx;     
    _sth_changed = true;
}

int CSettingsStorage::getLastEqualizer(){ return m_last_eq;}

QString CSettingsStorage::getLibraryPath(){return m_library_path;}
void CSettingsStorage::setLibraryPath(QString path){
	m_library_path = path;
    _sth_changed = true;
}

QSize CSettingsStorage::getPlayerSize(){ return m_player_size; }
void CSettingsStorage::setPlayerSize(QSize size){ 
    m_player_size = size;
    _sth_changed = true;
}

bool CSettingsStorage::getPlayerFullscreen(){ return m_player_fullscreen;}
void CSettingsStorage::setPlayerFullscreen(bool b){
    m_player_fullscreen = b;
    _sth_changed = true;
}

QStringList CSettingsStorage::getPlaylist(){
	return m_playlist;
}
void CSettingsStorage::setPlaylist(QStringList playlist){
	m_playlist = playlist;
        _sth_changed = true;
}


bool CSettingsStorage::getLoadLastTrack(){ return m_loadLastTrack; }

void CSettingsStorage::setLoadLastTrack(bool b){
	m_loadLastTrack = b;
        _sth_changed = true;
}

bool CSettingsStorage::getLoadPlaylist(){ return m_loadPlaylist; }

void CSettingsStorage::setLoadPlaylist(bool b){	m_loadPlaylist = b;     _sth_changed = true;}


LastTrack* CSettingsStorage::getLastTrack(){ return &m_lastTrack; }

void CSettingsStorage::setLastTrack(LastTrack& t){ m_lastTrack = t;     _sth_changed = true;}

void CSettingsStorage::updateLastTrack(){
    QString str = m_lastTrack.toString();
       _sth_changed = true;
}


bool CSettingsStorage::getRememberTime(){ return m_rememerTime; }

void CSettingsStorage::setRememberTime(bool b){ m_rememerTime = b;     _sth_changed = true;}

bool CSettingsStorage::getStartPlaying(){ return m_startPlaying; }

void CSettingsStorage::setStartPlaying(bool b){ m_startPlaying = b;     _sth_changed = true;}


void CSettingsStorage::setPlaylistMode(const Playlist_Mode& plmode){
	m_playlistmode = plmode;
    _sth_changed = true;
}

Playlist_Mode CSettingsStorage::getPlaylistMode(){
	return m_playlistmode;
}

void CSettingsStorage::setPlayerStyle(int style){
	m_style = style;
    _sth_changed = true;
}

int CSettingsStorage::getPlayerStyle(){
	return m_style;
}

void CSettingsStorage::setShowNotifications(bool active){
	m_show_notifications = active;
    _sth_changed = true;
}

void CSettingsStorage::setNotificationTimout(int timeout){
    m_notification_timeout = timeout;
    _sth_changed = true;

}

int CSettingsStorage::getNotificationTimeout(){
    return m_notification_timeout;
}

void CSettingsStorage::setNotification(QString n){
    m_notification_name = n;
    _sth_changed = true;

}

QString CSettingsStorage::getNotification(){
    return m_notification_name;
}

bool CSettingsStorage::getShowNotification(){
	return m_show_notifications;
}

void CSettingsStorage::setLastFMSessionKey(QString key){
	m_lfm_sessionKey = key;
    _sth_changed = true;
}
QString CSettingsStorage::getLastFMSessionKey(){
	return m_lfm_sessionKey;
}

void CSettingsStorage::setShowLibrary(bool b){
	m_show_library = b;
    _sth_changed = true;
}

bool CSettingsStorage::getShowLibrary(){
	return m_show_library;
}

void CSettingsStorage::setShownPlugin(QString plugin){
    m_shown_plugin = plugin;
    _sth_changed = true;
}

QString CSettingsStorage::getShownPlugin(){
	return m_shown_plugin;
}

void CSettingsStorage::setMinimizeToTray(bool b){
	m_minimize_to_tray = b;
    _sth_changed = true;
}
bool CSettingsStorage::getMinimizeToTray(){
	return m_minimize_to_tray;
}

bool CSettingsStorage::getShowSmallPlaylist(){
	return m_show_small_playlist;
}

void CSettingsStorage::setShowSmallPlaylist(bool b){
	m_show_small_playlist = b;
    _sth_changed = true;
}

QString CSettingsStorage::getSoundEngine(){
	return m_sound_engine;
}

void CSettingsStorage::setSoundEngine(QString engine){
	m_sound_engine = engine;
    _sth_changed = true;

}

bool CSettingsStorage::getStreamRipper(){
	return m_streamripper;
}

void CSettingsStorage::setStreamRipper(bool b){
	m_streamripper = b;
    _sth_changed = true;
}

bool CSettingsStorage::getStreamRipperWarning(){
	return	m_streamripper_warning;
}

void CSettingsStorage::setStreamRipperWarning(bool b){
	m_streamripper_warning = b;
    _sth_changed = true;
}

QString CSettingsStorage::getStreamRipperPath(){
	return m_streamripper_path;
}

void CSettingsStorage::setStreamRipperPath(QString path){
	m_streamripper_path = path;
    _sth_changed = true;
}

bool CSettingsStorage::getStreamRipperCompleteTracks(){
	return m_streamripper_complete_tracks;
}

void CSettingsStorage::setStreamRipperCompleteTracks(bool b){
	m_streamripper_complete_tracks = b;
    _sth_changed = true;
}

bool CSettingsStorage::getStreamRipperSessionPath(){
    return m_streamripper_session_path;
}

void CSettingsStorage::setStreamRipperSessionPath(bool b){
    m_streamripper_session_path = b;
    _sth_changed = true;
}

bool CSettingsStorage::getSocketActivated(){
	return m_socket_activated;
}
void CSettingsStorage::setSocketActivated(bool b){
	m_socket_activated = b;
    _sth_changed = true;
}

int CSettingsStorage::getSocketFrom(){
	return m_socket_from;
}
void CSettingsStorage::setSocketFrom(int val){
	m_socket_from = val;
    _sth_changed = true;
}

int CSettingsStorage::getSocketTo(){
	return m_socket_to;
}

void CSettingsStorage::setSocketTo(int val){
	m_socket_to = val;
    _sth_changed = true;
}

bool CSettingsStorage::getPlaylistNumbers(){
	return m_show_playlist_numbers;
}

void CSettingsStorage::setPlaylistNumbers(bool b){
	m_show_playlist_numbers = b;
    _sth_changed = true;
}


bool CSettingsStorage::getAllowOnlyOneInstance(){
	return m_allow_only_one_instance;
}

void CSettingsStorage::setAllowOnlyOneInstance(bool b){
	m_allow_only_one_instance = b;
    _sth_changed = true;
}


void CSettingsStorage::setLibShownColsTitle(QStringList lst){
    m_lib_shown_cols_title = lst;
    _sth_changed = true;
}

QStringList CSettingsStorage::getLibShownColsTitle(){
    return m_lib_shown_cols_title;
}

void CSettingsStorage::setLibShownColsAlbum(QStringList lst){
    m_lib_shown_cols_album = lst;
    _sth_changed = true;
}
QStringList CSettingsStorage::getLibShownColsAlbum(){
    return m_lib_shown_cols_album;
}

void CSettingsStorage::setLibShownColsArtist(QStringList lst){
    m_lib_shown_cols_artist = lst;
    _sth_changed = true;
}

QStringList CSettingsStorage::getLibShownColsArtist(){
    return m_lib_shown_cols_artist;
}

void CSettingsStorage::setLibSorting(QList<int> lst){
	m_lib_sortings = lst;
	_sth_changed = true;
}
QList<int> CSettingsStorage::getLibSorting(){
	return m_lib_sortings;
}



void CSettingsStorage::setLibShowOnlyTracks(bool only_tracks){
	m_show_only_tracks = only_tracks;
    _sth_changed = true;
}

bool CSettingsStorage::getLibShowOnlyTracks(){
	return m_show_only_tracks;

}

void CSettingsStorage::setLibLiveSearch(bool b){
    m_lib_live_search = b;
    _sth_changed = true;
}

bool CSettingsStorage::getLibLiveSheach(){
    return m_lib_live_search;
}





