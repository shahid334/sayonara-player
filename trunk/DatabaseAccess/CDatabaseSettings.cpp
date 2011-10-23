#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include <vector>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QObject>
#include <QSqlError>



bool CDatabaseConnector::load_settings(){

	/* Last FM */
	QVariant last_fm_setting;
	QString last_fm_username;
	QString last_fm_password;
	load_setting("LastFM_login", last_fm_setting);
	if(last_fm_setting != 0){
		QStringList list = last_fm_setting.toString().split(',');
		if(list.size() == 2){
			last_fm_username = list[0];
			last_fm_password = list[1];
		}
	}

	CSettingsStorage::getInstance()->setLastFMNameAndPW(last_fm_username, last_fm_password);


	/* Equalizer */
	QVariant v_eq_last;
	int eq_last = 0;
	load_setting("eq_last", v_eq_last);
	if(v_eq_last != 0){
		eq_last = v_eq_last.toInt();
		qDebug() << "Got from database: " << eq_last;
	}

	CSettingsStorage::getInstance()->setLastEqualizer(eq_last);

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

	CSettingsStorage::getInstance()->setEqualizerSettings(vec_eq_settings);

	/* Volume */
	QVariant v_volume;
	int volume = 0;
	load_setting("volume", v_volume);
	if(v_volume != 0)
		volume = v_volume.toInt();

	CSettingsStorage::getInstance()->setVolume(volume);



	QVariant v_lib_path;
	QString lib_path = "";
	load_setting("library_path", v_lib_path);
	if(v_lib_path != 0)
		lib_path = v_lib_path.toString();

	CSettingsStorage::getInstance()->setLibraryPath(lib_path);


	QVariant v_player_size;
	QStringList l_player_size;
	QSize player_size(800, 600);
	load_setting("player_size", v_player_size);
	if(v_player_size != 0){
		l_player_size = v_player_size.toString().split(',');
		player_size.setWidth(l_player_size[0].toInt());
		player_size.setHeight(l_player_size[1].toInt());
	}

	CSettingsStorage::getInstance()->setPlayerSize(player_size);

	// playlist
	QVariant playlist;
	QString playlist_str;
	load_setting("playlist", playlist);
	if(playlist != 0){
		playlist_str = playlist.toString();
	}

	CSettingsStorage::getInstance()->setPlaylist(playlist_str);


	QVariant load_playlist;
	bool load_playlist_bool;
	load_setting("load_playlist", load_playlist);
	load_playlist_bool = load_playlist.toBool();

	CSettingsStorage::getInstance()->setLoadPlaylist(load_playlist_bool);


	QVariant playlist_mode;
	Playlist_Mode playlist_mode_typed;
	load_setting("playlist_mode", playlist_mode);
	if(!playlist_mode.isNull()){
		playlist_mode_typed.fromString(playlist_mode.toString());
	}

	CSettingsStorage::getInstance()->setPlaylistMode(playlist_mode_typed);

	return true;
}

bool CDatabaseConnector::store_settings(){

	QString last_fm_username;
	QString last_fm_password;
	CSettingsStorage::getInstance()->getLastFMNameAndPW(last_fm_username, last_fm_password);
	store_setting("LastFM_login", last_fm_username + "," + last_fm_password);

	int last_eq_used = CSettingsStorage::getInstance()->getLastEqualizer();
	store_setting("eq_last", last_eq_used);

	QString custom_equalizer = CSettingsStorage::getInstance()->getCustomEqualizer().toString();
	store_setting("EQ_pr_custom", custom_equalizer);

	int volume = CSettingsStorage::getInstance()->getVolume();
	store_setting("volume", volume);

	QString library_path = CSettingsStorage::getInstance()->getLibraryPath();
	store_setting("library_path", library_path);

	QSize player_size = CSettingsStorage::getInstance()->getPlayerSize();
	QString str_size = QString::number(player_size.width()) + "," + QString::number(player_size.height());
	store_setting("player_size", str_size);

	QString cur_playlist = CSettingsStorage::getInstance()->getPlaylist();
	store_setting("playlist", cur_playlist);

	int load_playlist = CSettingsStorage::getInstance()->getLoadPlaylist();
	store_setting("load_playlist", load_playlist);

	QString playlist_mode = CSettingsStorage::getInstance()->getPlaylistMode().toString();
	store_setting("playlist_mode", playlist_mode);

	return true;
}





void CDatabaseConnector::load_setting(QString key, QVariant& tgt_value){
	if (!this -> m_database.isOpen())
	        this -> m_database.open();

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
			qDebug() << "Fetched " << key;
		}
	}

	catch (QString& ex) {
		qDebug() << ex;
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
			qDebug() << "Updated " << key;
		}
	}
	catch (QString& ex) {
		qDebug() << ex;
	}
}

