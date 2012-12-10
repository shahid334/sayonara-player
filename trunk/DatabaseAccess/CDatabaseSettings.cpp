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
	bool lfm_active = load_setting_bool("LastFM_active", false);
	settings->setLastFMActive(lfm_active);

	QString last_fm_username, last_fm_password;
	QStringList list = load_setting_strlist("LastFM_login");
	if(list.size() >= 2){
		last_fm_username = list[0];
		last_fm_password = list[1];
	}

	settings->setLastFMNameAndPW(last_fm_username, last_fm_password);


	bool lfm_corrections =	load_setting_bool("lfm_corrections");
	settings->setLastFMCorrections(lfm_corrections);


	QString lfm_session_key = load_setting_string("lfm_session_key");
	if(lfm_session_key.size() != 32) lfm_session_key = "";
	settings->setLastFMSessionKey(lfm_session_key);



	/* Equalizer */
	QVariant v_eq_last;
	int eq_last = 0;
	load_setting("eq_last", v_eq_last);
	if(v_eq_last != 0){
		eq_last = v_eq_last.toInt();
		qDebug() << "Got from database: " << eq_last;
	}

	settings->setLastEqualizer(eq_last);

	vector<EQ_Setting> vec_eq_settings;
	for(int i=0; i<7; i++){
		QVariant v_eq_preset;
		QString eq_preset = 0;
		switch(i){
			case 0: load_setting("EQ_pr_flat", v_eq_preset); break;
			case 1: load_setting("EQ_pr_rock", v_eq_preset); break;
			case 2: load_setting("EQ_pr_treble", v_eq_preset); break;
			case 3: load_setting("EQ_pr_bass", v_eq_preset); break;
			case 4: load_setting("EQ_pr_mid", v_eq_preset); break;
			case 5: load_setting("EQ_pr_light_rock", v_eq_preset); break;
			case 6: load_setting("EQ_pr_custom", v_eq_preset); break;
			default: load_setting("EQ_pr_flat", v_eq_preset); break;
		}

		if(v_eq_preset != 0){
			EQ_Setting eq_setting;
			eq_setting.parseFromString(v_eq_preset.toString());
			vec_eq_settings.push_back(eq_setting);
		}
	}

	settings->setEqualizerSettings(vec_eq_settings);

	/* Volume */
    int volume = load_setting_int("volume", 50);
	settings->setVolume(volume);


	/* Library path */
	QString lib_path = load_setting_string("library_path");
	settings->setLibraryPath(lib_path);


	/* Player size */
	QSize player_size(800, 600);
	QStringList l_player_size = load_setting_strlist("player_size");
	if(l_player_size.size() >= 2){
		player_size.setWidth(l_player_size[0].toInt());
		player_size.setHeight(l_player_size[1].toInt());
	}
	settings->setPlayerSize(player_size);

	
	// playlist
	QString playlist = load_setting_string("playlist");
	settings->setPlaylist(playlist);

    bool load_playlist = load_setting_bool("load_playlist", false);
	settings->setLoadPlaylist(load_playlist);

	QString playlist_mode_str = load_setting_string("playlist_mode");
	Playlist_Mode playlist_mode_typed;
	playlist_mode_typed.fromString(playlist_mode_str);
	settings->setPlaylistMode(playlist_mode_typed);

	// style
	int style = load_setting_int("player_style");
	settings->setPlayerStyle(style);

	/* show notifications */
    bool show_notifications = load_setting_bool("show_notifications", true);
	settings->setShowNotifications(show_notifications);

    int notification_timeout = load_setting_int("notification_timeout", 5000);
    settings->setNotificationTimout(notification_timeout);

    QString notification_name = load_setting_string("notification_name", "Standard");
    settings->setNotification(notification_name);

	/* show library */
	bool show_library = load_setting_bool("show_library", true);
	settings->setShowLibrary(show_library);


	/* shown plugin */
	int shown_plugin = load_setting_int("shown_plugin", PLUGIN_NONE);
	if(shown_plugin < 0 || shown_plugin > PLUGIN_NUM) 
		shown_plugin = PLUGIN_NONE;
	settings->setShownPlugin(shown_plugin);


	/* Minimize to tray */
	bool min2tray = load_setting_bool("min_to_tray", true);
	settings->setMinimizeToTray(min2tray);

	/* small playlist items */
	bool show_small_pl = load_setting_bool("small_playlist_items", true);
	settings->setShowSmallPlaylist(show_small_pl);

	/* Sound Engine */
	QString sound_engine = load_setting_string("sound_engine");
	settings->setSoundEngine(sound_engine);

	/* Stream ripper */
	bool streamripper = load_setting_bool("streamripper", false);
	settings->setStreamRipper(streamripper);

	bool streamripper_warning = load_setting_bool("streamripper_warning", true);
	settings->setStreamRipperWarning(streamripper_warning);

	QString streamripper_path = load_setting_string("streamripper_path", QDir::homePath());
	if(streamripper_path.trimmed().size() == 0 || !QFile::exists(streamripper_path)) streamripper_path = QDir::homePath();
	settings->setStreamRipperPath(streamripper_path);

	bool streamripper_complete_tracks = load_setting_bool("streamripper_complete_tracks", true);
	settings->setStreamRipperCompleteTracks(streamripper_complete_tracks);

    bool streamripper_session_path = load_setting_bool("streamripper_session_path", true);
    settings->setStreamRipperSessionPath(streamripper_session_path);

	bool socket_active = load_setting_bool("socket_active");
	settings->setSocketActivated(socket_active);

	int socket_from = load_setting_int("socket_from");
	settings->setSocketFrom(socket_from);

	int socket_to = load_setting_int("socket_to");
	settings->setSocketTo(socket_to);

	bool show_playlist_numbers = load_setting_bool("show_playlist_numbers");
	settings->setPlaylistNumbers(show_playlist_numbers);

	bool allow_only_one_instance = load_setting_bool("only_one_instance", true);
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
	store_setting("LastFM_active", lfm_active);

	storage->getLastFMNameAndPW(last_fm_username, last_fm_password);
	store_setting("LastFM_login", last_fm_username + "," + last_fm_password);

	bool lfm_corrections = storage->getLastFMCorrections();
	store_setting("lfm_corrections", lfm_corrections);

	QString lfm_session_key = storage->getLastFMSessionKey();
	store_setting("lfm_session_key", lfm_session_key);

	int last_eq_used = storage->getLastEqualizer();
	store_setting("eq_last", last_eq_used);

	QString custom_equalizer = storage->getCustomEqualizer().toString();
	store_setting("EQ_pr_custom", custom_equalizer);

	int volume = storage->getVolume();
	store_setting("volume", volume);

	QString library_path = storage->getLibraryPath();
	store_setting("library_path", library_path);

	QSize player_size = storage->getPlayerSize();
	QString str_size = QString::number(player_size.width()) + "," + QString::number(player_size.height());
	store_setting("player_size", str_size);

	QString cur_playlist = storage->getPlaylist();
	store_setting("playlist", cur_playlist);

	int load_playlist = storage->getLoadPlaylist();
	store_setting("load_playlist", load_playlist);

	QString playlist_mode = storage->getPlaylistMode().toString();
	store_setting("playlist_mode", playlist_mode);

	int style = storage->getPlayerStyle();
	if(style == 0 || style == 1)
		store_setting("player_style", style);
	else
		store_setting("player_style", 0);

	bool show_notifications = storage->getShowNotification();
	store_setting("show_notifications", show_notifications);

    int notification_timeout = storage->getNotificationTimeout();
    store_setting("notification_timeout", notification_timeout);

    QString notification_name = storage->getNotification();
    store_setting("notification_name", notification_name);

	bool show_library = storage->getShowLibrary();
	store_setting("show_library", show_library);

	int shown_plugin = storage->getShownPlugin();
	store_setting("shown_plugin", shown_plugin);

	bool min2tray = storage->getMinimizeToTray();
	store_setting("min_to_tray", min2tray);

	bool small_playlist_items = storage->getShowSmallPlaylist();
	store_setting("small_playlist_items", small_playlist_items);

	QString sound_engine = storage->getSoundEngine();
	store_setting("sound_engine", sound_engine);

	bool streamripper = storage->getStreamRipper();
	store_setting("streamripper", streamripper);
	
	bool streamripper_warning = storage->getStreamRipperWarning();
	store_setting("streamripper_warning", streamripper_warning);

	QString streamripper_path = storage->getStreamRipperPath();
	store_setting("streamripper_path", streamripper_path);

	bool streamripper_complete_tracks = storage->getStreamRipperCompleteTracks();
	store_setting("streamripper_complete_tracks", streamripper_complete_tracks);

    bool streamripper_session_path = storage->getStreamRipperSessionPath();
    store_setting("streamripper_session_path", streamripper_session_path);

	bool socket_active = storage->getSocketActivated();
	store_setting("socket_active", socket_active);

	int socket_from = storage->getSocketFrom();
	store_setting("socket_from", socket_from);

	int socket_to = storage->getSocketTo();
	store_setting("socket_to", socket_to);

	bool show_playlist_numbers = storage->getPlaylistNumbers();
	store_setting("show_playlist_numbers", show_playlist_numbers);

	bool allow_only_one_instance = storage->getAllowOnlyOneInstance();
	store_setting("only_one_instance", allow_only_one_instance);

	m_database.commit();
	return true;
}



void CDatabaseConnector::load_setting(QString key, QVariant& tgt_value, QVariant def){

    qDebug() << "Wanna load " << key;

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

