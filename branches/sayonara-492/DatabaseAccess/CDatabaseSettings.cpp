/* CDatabaseSettings.cpp */

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


#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/globals.h"
#include <vector>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QObject>
#include <QSqlError>

bool CDatabaseConnector::load_setting_bool(QString key, bool def){
	bool ret;
	QVariant v;
    load_setting(key, v, def);

	ret =  v.toBool();
    if(v.isNull() || !v.isValid()) return def;
	else return ret;
}

QString CDatabaseConnector::load_setting_string(QString key, QString def){
	QString ret;
	QVariant v;
    load_setting(key, v, def);
	ret = v.toString();
	if(v.isNull()) return def;
	else return ret;
}

int CDatabaseConnector::load_setting_int(QString key, int def){
	bool ok;
	int ret;
	QVariant v;
    load_setting(key, v, def);
	ret = v.toInt(&ok);
	if(!ok) return def;
	else return ret;

}

QStringList CDatabaseConnector::load_setting_strlist(QString key, QChar sep){

    DB_TRY_OPEN(m_database);

	return load_setting_string(key).split(sep);
}


bool CDatabaseConnector::load_settings(){

    DB_TRY_OPEN(m_database);
    DB_RETURN_NOT_OPEN_BOOL(m_database);

	CSettingsStorage* settings = CSettingsStorage::getInstance();

	/* Last FM */
	bool lfm_active = load_setting_bool(SET_LFM_ACTIVE, false);
	settings->setLastFMActive(lfm_active);

	QString last_fm_username, last_fm_password;
	QStringList list = load_setting_strlist(SET_LFM_LOGIN);
	if(list.size() >= 2){
		last_fm_username = list[0];
		last_fm_password = list[1];
	}

	settings->setLastFMNameAndPW(last_fm_username, last_fm_password);


	bool lfm_corrections =	load_setting_bool(SET_LFM_CORRECTIONS);
	settings->setLastFMCorrections(lfm_corrections);


	QString lfm_session_key = load_setting_string(SET_LFM_SESSION_KEY);
	if(lfm_session_key.size() != 32) lfm_session_key = "";
	settings->setLastFMSessionKey(lfm_session_key);



	/* Equalizer */
	QVariant v_eq_last;
	int eq_last = 0;
	load_setting(SET_EQ_LAST, v_eq_last);
	if(v_eq_last != 0){
		eq_last = v_eq_last.toInt();
	}

	settings->setLastEqualizer(eq_last);

	vector<EQ_Setting> vec_eq_settings;
	for(int i=0; i<7; i++){
		QVariant v_eq_preset;
		QString eq_preset = 0;
		switch(i){
			case 0: load_setting(SET_EQ_FLAT, v_eq_preset); break;
			case 1: load_setting(SET_EQ_ROCK, v_eq_preset); break;
			case 2: load_setting(SET_EQ_TREBLE, v_eq_preset); break;
			case 3: load_setting(SET_EQ_BASS, v_eq_preset); break;
			case 4: load_setting(SET_EQ_MID, v_eq_preset); break;
			case 5: load_setting(SET_EQ_LIGHT_ROCK, v_eq_preset); break;
			case 6: load_setting(SET_EQ_CUSTOM, v_eq_preset); break;
			default: load_setting(SET_EQ_FLAT, v_eq_preset); break;
		}

		if(v_eq_preset != 0){
			EQ_Setting eq_setting;
			eq_setting.parseFromString(v_eq_preset.toString());
			vec_eq_settings.push_back(eq_setting);
		}
	}

	settings->setEqualizerSettings(vec_eq_settings);

	/* Volume */
    int volume = load_setting_int(SET_ENGINE_VOL, 50);
	settings->setVolume(volume);


	/* Library path */
	QString lib_path = load_setting_string(SET_LIB_PATH);
	settings->setLibraryPath(lib_path);

    QStringList lib_shown_cols_title, lib_shown_cols_artist, lib_shown_cols_album;
    lib_shown_cols_title = load_setting_string(SET_LIB_SHOWN_COLS_TITLE, "1,1,1,1,1,1,1,1,1,1").split(",");
    lib_shown_cols_artist = load_setting_string(SET_LIB_SHOWN_COLS_ARTIST, "1,1,1,1,1,1,1,1,1,1").split(",");
    lib_shown_cols_album = load_setting_string(SET_LIB_SHOWN_COLS_ALBUM, "1,1,1,1,1,1,1,1,1,1").split(",");
    settings->setLibShownColsTitle(lib_shown_cols_title);
    settings->setLibShownColsAlbum(lib_shown_cols_album);
    settings->setLibShownColsArtist(lib_shown_cols_artist);

    bool show_only_tracks = load_setting_bool(SET_LIB_SHOWN_ONLY_TRACKS, false);
    settings->setLibShowOnlyTracks(show_only_tracks);


	/* Player size */
	QSize player_size(800, 600);
	QStringList l_player_size = load_setting_strlist(SET_PLAYER_SIZE);
	if(l_player_size.size() >= 2){
		player_size.setWidth(l_player_size[0].toInt());
		player_size.setHeight(l_player_size[1].toInt());
	}
	settings->setPlayerSize(player_size);

	
	// playlist
    QStringList playlist = load_setting_strlist(SET_PL);
	settings->setPlaylist(playlist);

    bool load_playlist = load_setting_bool(SET_PL_LOAD, false);
	settings->setLoadPlaylist(load_playlist);

	bool load_last_track = load_setting_bool(SET_PL_LOAD_LAST_TRACK, false);
    settings->setLoadLastTrack(load_last_track);

    bool remember_time = load_setting_bool(SET_PL_REMEMBER_TIME, false);
    settings->setRememberTime(remember_time);

    bool start_playing = load_setting_bool(SET_PL_START_PLAYING, false);
    settings->setStartPlaying(start_playing);

	LastTrack track = LastTrack::fromString(load_setting_string(SET_PL_LAST_TRACK, ""));
        settings->setLastTrack(track);

	QString playlist_mode_str = load_setting_string(SET_PL_MODE);
	Playlist_Mode playlist_mode_typed;
	playlist_mode_typed.fromString(playlist_mode_str);
	settings->setPlaylistMode(playlist_mode_typed);

	// style
	int style = load_setting_int(SET_PLAYER_STYLE);
	settings->setPlayerStyle(style);

	/* show notifications */
    bool show_notifications = load_setting_bool(SET_NOTIFICATION_SHOW, true);
	settings->setShowNotifications(show_notifications);

    int notification_timeout = load_setting_int(SET_NOTIFICATION_TIMEOUT, 5000);
    settings->setNotificationTimout(notification_timeout);

    QString notification_name = load_setting_string(SET_NOTIFICATION_NAME, "Standard");
    settings->setNotification(notification_name);

	/* show library */
	bool show_library = load_setting_bool(SET_LIB_SHOW, true);
	settings->setShowLibrary(show_library);


	/* shown plugin */
	int shown_plugin = load_setting_int(SET_PLAYER_SHOWN_PLUGIN, PLUGIN_NONE);
	if(shown_plugin < 0 || shown_plugin > PLUGIN_NUM) 
		shown_plugin = PLUGIN_NONE;
	settings->setShownPlugin(shown_plugin);


	/* Minimize to tray */
	bool min2tray = load_setting_bool(SET_PLAYER_MIN_2_TRAY, true);
	settings->setMinimizeToTray(min2tray);

	/* small playlist items */
	bool show_small_pl = load_setting_bool(SET_PL_SMALL_ITEMS, true);
	settings->setShowSmallPlaylist(show_small_pl);

	/* Sound Engine */
	QString sound_engine = load_setting_string(SET_ENGINE);
	settings->setSoundEngine(sound_engine);

	/* Stream ripper */
	bool streamripper = load_setting_bool(SET_SR_ACTIVE, false);
	settings->setStreamRipper(streamripper);

	bool streamripper_warning = load_setting_bool(SET_SR_WARNING, true);
	settings->setStreamRipperWarning(streamripper_warning);

	QString streamripper_path = load_setting_string(SET_SR_PATH, QDir::homePath());
	if(streamripper_path.trimmed().size() == 0 || !QFile::exists(streamripper_path)) streamripper_path = QDir::homePath();
	settings->setStreamRipperPath(streamripper_path);

	bool streamripper_complete_tracks = load_setting_bool(SET_SR_COMPLETE_TRACKS, true);
	settings->setStreamRipperCompleteTracks(streamripper_complete_tracks);

    bool streamripper_session_path = load_setting_bool(SET_SR_SESSION_PATH, true);
    settings->setStreamRipperSessionPath(streamripper_session_path);

	bool socket_active = load_setting_bool(SET_SOCKET_ACTIVE);
	settings->setSocketActivated(socket_active);

	int socket_from = load_setting_int(SET_SOCKET_FROM);
	settings->setSocketFrom(socket_from);

	int socket_to = load_setting_int(SET_SOCKET_TO);
	settings->setSocketTo(socket_to);

	bool show_playlist_numbers = load_setting_bool(SET_PL_SHOW_NUMBERS);
	settings->setPlaylistNumbers(show_playlist_numbers);

	bool allow_only_one_instance = load_setting_bool(SET_PLAYER_ONE_INSTANCE, true);
	settings->setAllowOnlyOneInstance(allow_only_one_instance);

	return true;
}

bool CDatabaseConnector::store_settings(){

    DB_TRY_OPEN(m_database);
    DB_RETURN_NOT_OPEN_BOOL(m_database);

	QString last_fm_username;
	QString last_fm_password;
	CSettingsStorage* storage = CSettingsStorage::getInstance();

	m_database.transaction();

	bool lfm_active = storage->getLastFMActive();
	store_setting(SET_LFM_ACTIVE, lfm_active);

	storage->getLastFMNameAndPW(last_fm_username, last_fm_password);
	store_setting(SET_LFM_LOGIN, last_fm_username + "," + last_fm_password);

	bool lfm_corrections = storage->getLastFMCorrections();
	store_setting(SET_LFM_CORRECTIONS, lfm_corrections);

	QString lfm_session_key = storage->getLastFMSessionKey();
	store_setting(SET_LFM_SESSION_KEY, lfm_session_key);

	int last_eq_used = storage->getLastEqualizer();
	store_setting(SET_EQ_LAST, last_eq_used);

	QString custom_equalizer = storage->getCustomEqualizer().toString();
	store_setting(SET_EQ_CUSTOM, custom_equalizer);

	int volume = storage->getVolume();
	store_setting(SET_ENGINE_VOL, volume);

	QString library_path = storage->getLibraryPath();
	store_setting(SET_LIB_PATH, library_path);

    QStringList lib_shown_cols_title, lib_shown_cols_artist, lib_shown_cols_album;
    lib_shown_cols_title = storage->getLibShownColsTitle();
    lib_shown_cols_album = storage->getLibShownColsAlbum();
    lib_shown_cols_artist = storage->getLibShownColsArtist();
    store_setting(SET_LIB_SHOWN_COLS_TITLE, lib_shown_cols_title.join(","));
    store_setting(SET_LIB_SHOWN_COLS_ALBUM, lib_shown_cols_album.join(","));
    store_setting(SET_LIB_SHOWN_COLS_ARTIST, lib_shown_cols_artist.join(","));

    bool lib_show_only_tracks = storage->getLibShowOnlyTracks();
    store_setting(SET_LIB_SHOWN_ONLY_TRACKS, lib_show_only_tracks);

	QSize player_size = storage->getPlayerSize();
	QString str_size = QString::number(player_size.width()) + "," + QString::number(player_size.height());
	store_setting(SET_PLAYER_SIZE, str_size);

    QString cur_playlist = storage->getPlaylist().join(",");
	store_setting(SET_PL, cur_playlist);

	int load_playlist = storage->getLoadPlaylist();
	store_setting(SET_PL_LOAD, load_playlist);

	bool load_last_track = storage->getLoadLastTrack();
	store_setting(SET_PL_LOAD_LAST_TRACK, load_last_track);

	QString last_track = storage->getLastTrack()->toString();
	store_setting(SET_PL_LAST_TRACK, last_track);

    bool remember_time = storage->getRememberTime();
    store_setting(SET_PL_REMEMBER_TIME, remember_time);

    bool start_playing = storage->getStartPlaying();
    store_setting(SET_PL_START_PLAYING, start_playing);

	QString playlist_mode = storage->getPlaylistMode().toString();
	store_setting(SET_PL_MODE, playlist_mode);

	int style = storage->getPlayerStyle();
	if(style == 0 || style == 1)
		store_setting(SET_PLAYER_STYLE, style);
	else
		store_setting(SET_PLAYER_STYLE, 0);

	bool show_notifications = storage->getShowNotification();
	store_setting(SET_NOTIFICATION_SHOW, show_notifications);

    int notification_timeout = storage->getNotificationTimeout();
    store_setting(SET_NOTIFICATION_TIMEOUT, notification_timeout);

    QString notification_name = storage->getNotification();
    store_setting(SET_NOTIFICATION_NAME, notification_name);

	bool show_library = storage->getShowLibrary();
	store_setting("show_library", show_library);

	int shown_plugin = storage->getShownPlugin();
	store_setting(SET_PLAYER_SHOWN_PLUGIN, shown_plugin);

	bool min2tray = storage->getMinimizeToTray();
	store_setting(SET_PLAYER_MIN_2_TRAY, min2tray);

	bool small_playlist_items = storage->getShowSmallPlaylist();
	store_setting(SET_PL_SMALL_ITEMS, small_playlist_items);

	QString sound_engine = storage->getSoundEngine();
	store_setting(SET_ENGINE, sound_engine);

	bool streamripper = storage->getStreamRipper();
	store_setting(SET_SR_ACTIVE, streamripper);
	
	bool streamripper_warning = storage->getStreamRipperWarning();
	store_setting(SET_SR_WARNING, streamripper_warning);

	QString streamripper_path = storage->getStreamRipperPath();
	store_setting(SET_SR_PATH, streamripper_path);

	bool streamripper_complete_tracks = storage->getStreamRipperCompleteTracks();
	store_setting(SET_SR_COMPLETE_TRACKS, streamripper_complete_tracks);

    bool streamripper_session_path = storage->getStreamRipperSessionPath();
    store_setting(SET_SR_SESSION_PATH, streamripper_session_path);

	bool socket_active = storage->getSocketActivated();
	store_setting(SET_SOCKET_ACTIVE, socket_active);

	int socket_from = storage->getSocketFrom();
	store_setting(SET_SOCKET_FROM, socket_from);

	int socket_to = storage->getSocketTo();
	store_setting(SET_SOCKET_TO, socket_to);

	bool show_playlist_numbers = storage->getPlaylistNumbers();
	store_setting(SET_PL_SHOW_NUMBERS, show_playlist_numbers);

	bool allow_only_one_instance = storage->getAllowOnlyOneInstance();
	store_setting(SET_PLAYER_ONE_INSTANCE, allow_only_one_instance);

	m_database.commit();
	return true;
}



void CDatabaseConnector::load_setting(QString key, QVariant& tgt_value, QVariant def){


	tgt_value = 0;
	try {
		QSqlQuery q (this -> m_database);
		q.prepare("select value from settings where key = ?;");
		q.addBindValue(QVariant(key));

		if (!q.exec()) {
			throw QString ("SQL - Error: loading " + key);
		}

		if(q.next()){
			tgt_value = q.value(0);
		}

        else{
            tgt_value = def;
        }
	}

	catch (QString& ex) {
		qDebug() << ex;
        tgt_value = def;
	}
}


void CDatabaseConnector::store_setting(QString key, QVariant value){
	 try {

		QSqlQuery q (this -> m_database);
		q.prepare("select value from settings where key = :key;");
		q.bindValue(":key", key);

		if (!q.exec()) {
			throw QString ("SQL - Error: load " + key + " (during inserting)");
		}

		if (!q.next()) {
			q.prepare("INSERT INTO settings VALUES(:key, :val);");
			q.bindValue(":key", key);
			q.bindValue(":value", value);
			if (!q.exec()) {
				throw QString ("SQL - Error: First insert of " + key);
			}

			else{
				qDebug() << "Inserted " << key << " first time";
			}
		}

		q.prepare("UPDATE settings set value=:value WHERE key=:key;");
		q.bindValue(":key", key);
		q.bindValue(":value", value);

		if (!q.exec()) {
			throw QString ("SQL - Error: update setting " + key);
		}

		else{
           // qDebug() << "Updated " << key << " -> " << value;
		}
	}
	catch (QString& ex) {
		qDebug() << ex;
	}
}

