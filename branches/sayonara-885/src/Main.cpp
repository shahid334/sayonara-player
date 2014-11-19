/* Main.cpp */

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


/*
 * Main.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: luke
 */

#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>

#include "application.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/SmartComparison.h"
#include "Settings/Settings.h"

#include <QDir>
#include <QFile>
#include <QTranslator>
#include <QFontDatabase>


int check_for_another_instance_unix() {

	int pid = -1;

	QDir dir("/proc");
	dir.cd(".");
	QStringList lst = dir.entryList(QDir::Dirs);
	int n_instances = 0;

	foreach(QString dirname, lst) {
		bool ok;
		int tmp_pid = dirname.toInt(&ok);
		if(!ok) continue;

		dir.cd(dirname);

		QFile f(dir.absolutePath() + QDir::separator() + "cmdline");
		f.open(QIODevice::ReadOnly);
		if(!f.isOpen()) {
			dir.cd("..");
			continue;
		}

		QString str = f.readLine();
		f.close();

		if(str.contains("sayonara", Qt::CaseInsensitive)) {
			if(pid == -1 || tmp_pid < pid) pid = tmp_pid;
			n_instances ++;
			if(n_instances > 1) return pid;
		}
		dir.cd("..");
	}

	return 0;
}


void printHelp() {
	qDebug() << "sayonara <list>";
	qDebug() << "<list> can consist of either files or directories or both";

}



void segfault_handler(int sig){
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 20);

	// print out all the frames to stderr

	qDebug() << "";
	qDebug() << "Segfault received.";
	qDebug() << "Sorry, Sayonara has crashed. :(";


	int fd;
	FILE* f;
	QString target_file = Helper::getErrorFile();
	f = fopen(target_file.toStdString().c_str(), "w");
	if(!f) exit(1);
	fd = fileno(f);

	//backtrace_symbols_fd(array, size, STDERR_FILENO);
	backtrace_symbols_fd(array, size, fd);
	fclose(f);

	qDebug() << "Please send the error file " << target_file << " to luciocarreras@gmail.com";

	exit(1);
}

#define REGISTER_SETTING(type, key, db_key, def) set->register_setting( new Setting<type>(Set::key, db_key, def) )

bool register_settings(){

	Settings* set = Settings::getInstance();

	QStringList lfm_login;
	lfm_login << "None" << "None";
	REGISTER_SETTING(QStringList, LFM_Login, "LastFM_login", lfm_login);

	REGISTER_SETTING( bool, LFM_Active, "LastFM_active", false );
	REGISTER_SETTING( bool, LFM_Corrections, "lfm_corrections", false );
	REGISTER_SETTING( bool, LFM_ShowErrors, "lfm_show_errors", false );
	REGISTER_SETTING( QString, LFM_SessionKey, "lfm_session_key", QString() );

	REGISTER_SETTING( int, Eq_Last, "eq_last", 0);
	REGISTER_SETTING( EQ_Setting, Eq_Flat, "EQ_pr_flat", EQ_Setting() );
	REGISTER_SETTING( EQ_Setting, Eq_Rock, "EQ_pr_rock", EQ_Setting() );
	REGISTER_SETTING( EQ_Setting, Eq_Treble, "EQ_pr_treble", EQ_Setting() );
	REGISTER_SETTING( EQ_Setting, Eq_Bass, "EQ_pr_bass", EQ_Setting() );
	REGISTER_SETTING( EQ_Setting, Eq_Mid, "EQ_pr_mid", EQ_Setting() );
	REGISTER_SETTING( EQ_Setting, Eq_LightRock, "EQ_pr_light_rock", EQ_Setting() );
	REGISTER_SETTING( EQ_Setting, Eq_Custom, "EQ_pr_custom", EQ_Setting() );

	REGISTER_SETTING( bool, Lib_Show, "show_library", true );
	REGISTER_SETTING( QString, Lib_Path, "library_path", QString() );

	QList<int> shown_cols;
	shown_cols << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1;
	REGISTER_SETTING(  QList<int> , Lib_ColsTitle, "lib_shown_cols_title", shown_cols );
	REGISTER_SETTING(  QList<int> , Lib_ColsArtist, "lib_shown_cols_artist", shown_cols );
	REGISTER_SETTING(  QList<int> , Lib_ColsAlbum, "lib_shown_cols_album", shown_cols );

	REGISTER_SETTING( bool, Lib_OnlyTracks, "lib_shown_tracks", false );
	REGISTER_SETTING( bool, Lib_LiveSearch, "lib_live_search", true );

	QList<int> sortings;
	sortings << 1 << 5 << 21;
	REGISTER_SETTING(  QList<int> , Lib_Sorting, "lib_sortings", sortings );

	REGISTER_SETTING( QString, Player_Version, "player_version", "0.4.1");
	REGISTER_SETTING( QString, Player_Language, "player_language", "sayonara_lang_en" );
	REGISTER_SETTING( int, Player_Style, "player_style", 0 );
	REGISTER_SETTING( QSize, Player_Size, "player_size", QSize(800,600) );
	REGISTER_SETTING( QPoint, Player_Pos, "player_pos", QPoint(50,50) );
	REGISTER_SETTING( bool, Player_Fullscreen, "player_fullscreen", false );
	REGISTER_SETTING( bool, Player_Maximized, "player_maximized", false );
	REGISTER_SETTING( QString, Player_ShownPlugin, "shown_plugin", QString() );
	REGISTER_SETTING( bool, Player_OneInstance, "only_one_instance", true );
	REGISTER_SETTING( bool, Player_Min2Tray, "min_to_tray", false );
	REGISTER_SETTING( bool, Player_NotifyNewVersion, "notify_new_version", true );

	REGISTER_SETTING( QStringList, PL_Playlist, "playlist", QStringList() );
	REGISTER_SETTING( bool, PL_Load, "load_playlist", false );
	REGISTER_SETTING( bool, PL_LoadLastTrack, "load_last_track", false );
	REGISTER_SETTING( bool, PL_RememberTime, "remember_time", false );
	REGISTER_SETTING( bool, PL_StartPlaying, "start_playing", false );
	REGISTER_SETTING( int, PL_LastTrack, "last_track", -1 );
	REGISTER_SETTING( int, PL_LastTrackPos, "last_track_pos", 0 );
	REGISTER_SETTING( PlaylistMode, PL_Mode, "playlist_mode", PlaylistMode() );
	REGISTER_SETTING( bool, PL_ShowNumbers, "show_playlist_numbers", true );
	REGISTER_SETTING( bool, PL_SmallItems, "small_playlist_items", true );

	REGISTER_SETTING( bool, Notification_Show, "show_notifications", true );
	REGISTER_SETTING( int, Notification_Timeout, "notification_timeout", 5000 );
	REGISTER_SETTING( QString, Notification_Name, "notification_name", "libnotify" );
	REGISTER_SETTING( int, Notification_Scale, "notification_scale", 64 );

	REGISTER_SETTING( QString, Engine_Name, "sound_engine", QString() );
	REGISTER_SETTING( int, Engine_Vol, "volume", 50 );
	REGISTER_SETTING( int, Engine_ConvertQuality, "convert_quality", 0 );
	REGISTER_SETTING( QString, Engine_CovertTargetPath, "convert_target_path", QDir::homePath() );
	REGISTER_SETTING( bool, Engine_Gapless, "gapless_playback", false);

	REGISTER_SETTING( bool, Engine_SR_Active, "streamripper", false );
	REGISTER_SETTING( bool, Engine_SR_Warning, "streamripper_warning", true );
	REGISTER_SETTING( QString, Engine_SR_Path, "streamripper_path", QDir::homePath() );
	REGISTER_SETTING( bool, Engine_SR_CompleteTracks, "streamripper_complete_tracks", true );
	REGISTER_SETTING( bool, Engine_SR_SessionPath, "streamripper_session_path", true );

	REGISTER_SETTING( bool, Socket_Active, "socket_active", false );
	REGISTER_SETTING( int, Socket_From, "socket_from", 54055 );
	REGISTER_SETTING( int, Socket_To, "socket_to", 54056 );

	REGISTER_SETTING( int, Spectrum_Style, "spectrum_style", 0 );

	REGISTER_SETTING( bool, BroadCast_Active, "broadcast_active", false );
	REGISTER_SETTING( bool, Broadcast_Prompt, "broadcast_prompt", false );
	REGISTER_SETTING( int, Broadcast_Port, "broadcast_port", 54054 );

	return set->check_settings();
}

int main(int argc, char *argv[]) {

#ifdef Q_OS_UNIX

	signal(SIGSEGV, segfault_handler);

#endif

	if(!register_settings()){
		return 1;
	}

	Application app (argc, argv);
    Helper::set_bin_path(app.applicationDirPath());

    Settings* settings = Settings::getInstance();

    bool success = CDatabaseConnector::getInstance()->load_settings();
	
    if(!success) {
	qDebug() << "Database Error: Could not load settings";
	return 0;
    }


#ifdef Q_OS_UNIX
	bool one_instance = settings->get(Set::Player_OneInstance);

	if(one_instance) {
		int pid = check_for_another_instance_unix();
		if(pid > 0) {
			qDebug() << "another instance is already running";
			// other instance should pop up
			QString kill_cmd = "kill -s 28 " + QString::number(pid);
			int success = system(kill_cmd.toLocal8Bit());
			Q_UNUSED(success);
			return 0;
		}
	}
#endif


   	if(!QFile::exists(QDir::homePath() + QDir::separator() + ".Sayonara")) {
        QDir().mkdir(QDir::homePath() + QDir::separator() +  "/.Sayonara");
	}


    app.setApplicationName("Sayonara");
    app.setWindowIcon(Helper::getIcon("logo.png"));


    QStringList params;
    for(int i=1; i<argc; i++) {
        QString param(argv[i]);
        params.push_back(param);
    }

	QTranslator translator;
	QString language = Settings::getInstance()->get(Set::Player_Language);

    translator.load(language, Helper::getSharePath() + "translations");
    app.installTranslator(&translator);

    QFont font("DejaVu Sans", 9, 55,  false);
	font.setHintingPreference(QFont::PreferNoHinting);
	int strategy =  (QFont::PreferDefault | QFont::PreferQuality);
	font.setStyleStrategy((QFont::StyleStrategy) strategy  );
    app.setFont(font);

    app.init(params.size(), &translator);
    if(!app.is_initialized()) return 0;

    app.setFiles2Play(params);
    app.installTranslator(&translator);
    app.exec();

    return 0;
}
