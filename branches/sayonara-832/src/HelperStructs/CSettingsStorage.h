/* CSettingsStorage.h */

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


#ifndef CSettingsStorage_H
#define CSettingsStorage_H

#define SET_LFM_ACTIVE "LastFM_active"
#define SET_LFM_LOGIN "LastFM_login"
#define SET_LFM_CORRECTIONS "lfm_corrections"
#define SET_LFM_SESSION_KEY "lfm_session_key"
#define SET_LFM_SHOW_ERRORS "lfm_show_errors"


#define SET_EQ_LAST "eq_last"
#define SET_EQ_FLAT "EQ_pr_flat"
#define SET_EQ_ROCK "EQ_pr_rock"
#define SET_EQ_TREBLE "EQ_pr_treble"
#define SET_EQ_BASS "EQ_pr_bass"
#define SET_EQ_MID "EQ_pr_mid"
#define SET_EQ_LIGHT_ROCK "EQ_pr_light_rock"
#define SET_EQ_CUSTOM "EQ_pr_custom"


#define SET_LIB_SHOW "show_library"
#define SET_LIB_PATH "library_path"
#define SET_LIB_SHOWN_COLS_TITLE "lib_shown_cols_title"
#define SET_LIB_SHOWN_COLS_ARTIST "lib_shown_cols_artist"
#define SET_LIB_SHOWN_COLS_ALBUM "lib_shown_cols_album"
#define SET_LIB_SHOWN_ONLY_TRACKS "lib_shown_tracks"
#define SET_LIB_LIVE_SEARCH "lib_live_search"
#define SET_LIB_SORTING "lib_sortings"

#define SET_PLAYER_LANGUAGE "player_language"
#define SET_PLAYER_STYLE "player_style"
#define SET_PLAYER_SIZE "player_size"
#define SET_PLAYER_POS "player_pos"
#define SET_PLAYER_FULLSCREEN "player_fullscreen"
#define SET_PLAYER_MAXIMIZED "player_maximized"
#define SET_PLAYER_SHOWN_PLUGIN "shown_plugin"
#define SET_PLAYER_ONE_INSTANCE "only_one_instance"
#define SET_PLAYER_MIN_2_TRAY "min_to_tray"
#define SET_PLAYER_NOTIFY_NEW_VERSION "notify_new_version"


#define SET_PL "playlist"
#define SET_PL_LOAD "load_playlist"
#define SET_PL_LOAD_LAST_TRACK "load_last_track"
#define SET_PL_REMEMBER_TIME "remember_time"
#define SET_PL_START_PLAYING "start_playing"
#define SET_PL_LAST_TRACK "last_track"
#define SET_PL_MODE "playlist_mode"
#define SET_PL_SHOW_NUMBERS "show_playlist_numbers"
#define SET_PL_SMALL_ITEMS "small_playlist_items"

#define SET_NOTIFICATION_SHOW "show_notifications"
#define SET_NOTIFICATION_TIMEOUT "notification_timeout"
#define SET_NOTIFICATION_NAME "notification_name"
#define SET_NOTIFICATION_SCALE "notification_scale"

#define SET_ENGINE "sound_engine"
#define SET_ENGINE_VOL "volume"
#define SET_ENGINE_GAPLESS "gapless"
#define SET_ENGINE_CVT "convert_quality"
#define SET_ENGINE_CVT_TGT_PATH "convert_target_path"

#define SET_SR_ACTIVE "streamripper"
#define SET_SR_WARNING "streamripper_warning"
#define SET_SR_PATH "streamripper_path"
#define SET_SR_COMPLETE_TRACKS "streamripper_complete_tracks"
#define SET_SR_SESSION_PATH "streamripper_session_path"

#define SET_SOCKET_ACTIVE "socket_active"
#define SET_SOCKET_FROM "socket_from"
#define SET_SOCKET_TO "socket_to"

#define SET_SPECTRUM_STYLE "spectrum_style"


#include <QString>
#include <QThread>
#include <QPair>
#include <QSize>
#include <QPoint>
#include <vector>

#include <HelperStructs/Equalizer_presets.h>
#include <HelperStructs/PlaylistMode.h>
#include <HelperStructs/MetaData.h>

class CSettingsStorage;

using namespace std;



	class SettingsThread : public QThread {


	private:
		CSettingsStorage* _settings;
		bool _goon;
        QWidget* _parent;

	protected: 
		void run();

	public:
        SettingsThread(QWidget* parent);
		virtual ~SettingsThread();
		void stop();


};


/**
  * Class to check if we are running the first time.
  * If yes we are doing bootstrapping. It is a Singleton
  */
class CSettingsStorage : public QObject
{
Q_OBJECT

signals: 

	void sig_save(QString, QVariant);
	void sig_save_all();
public:
    static CSettingsStorage * getInstance();

    virtual ~CSettingsStorage ();

    /**
      * Check if sayonarra is run the first time
      * @return true if run first time
      */
    bool isRunFirstTime ();

    /**
      * Returns Filename of DB as absolute path
      */
    QString getDBFileName ();





private:

    void init();
    bool m_isInit;
    CSettingsStorage() {this -> init();}
    CSettingsStorage(const CSettingsStorage&);
    CSettingsStorage& operator=(const CSettingsStorage&);

    QString m_dbFile, m_sayonaraPath, m_dbSource;
    QString _version;
    bool _sth_changed;
	

    // last fm
    bool m_lfm_active;
    QPair<QString,QString> m_lastFm;
    QString m_lfm_sessionKey;
    bool m_lfm_corrections;
    bool m_lfm_show_errors;

    // listen
    int	m_volume;
    int m_last_eq;
	bool m_gapless;
    vector<EQ_Setting> m_vec_eqSettings;
    
    // player size
    QSize m_player_size;
    QPoint m_player_pos;
    bool m_player_fullscreen;
    bool m_player_maximized;


    // library
    QString m_library_path;
    bool m_show_library;
    QStringList m_lib_shown_cols_title;
    QStringList m_lib_shown_cols_artist;
    QStringList m_lib_shown_cols_album;
    bool m_show_only_tracks;
    bool m_lib_live_search;
    QList<int> m_lib_sortings;

    // playlist
    QStringList m_playlist;
    bool m_loadPlaylist;
    bool m_loadLastTrack;
	MetaData m_lastTrack;
    bool m_rememerTime;
    bool m_startPlaying;
    PlaylistMode m_playlistmode;

    /* style */
    int	m_style;
    QString m_language;

    /* shown plugin in player */
    QString m_shown_plugin;

    /* minimize the player to tray */
    bool m_minimize_to_tray;

    /* show small icons */
    bool m_show_small_playlist;

    bool m_show_playlist_numbers;

    /* activate stream ripper */
    bool m_streamripper;
    bool m_streamripper_warning;  
    QString m_streamripper_path;
    bool m_streamripper_complete_tracks;
    bool m_streamripper_session_path;

    /* sound engine */
    QString m_sound_engine;
	LameBitrate m_cvt_quality;
	QString m_cvt_tgt_path;

    /* notifications */
    bool    m_show_notifications;
    int     m_notification_timeout;
    QString m_notification_name;
    int     m_notification_scale;

    bool    m_notify_new_version;

    /* socket stuff */
    bool	m_socket_activated;
    int		m_socket_from;
    int		m_socket_to;

    bool	m_allow_only_one_instance;

    int     m_spectrum_style;

protected:
    void save_all();

public:


	void set_sth_changed(bool);

	bool get_sth_changed();
    QString getVersion();
    void setVersion(QString str);

    QPair<QString,QString> getLastFMNameAndPW ();
    bool getLastFMActive();
    void setLastFMActive(bool b);
    void getLastFMNameAndPW (QString & name, QString & pw);
    void setLastFMNameAndPW (const QString & name,const QString & pw);
    bool getLastFMShowErrors();
    void setLastFMShowErrors(bool b);

    void setLastFMCorrections(bool checked);
    bool getLastFMCorrections();

    int getVolume();
    void setVolume(int vol);

	bool getGapless();
	void setGapless(bool gapless);

	void setLastEqualizer(int);
	int getLastEqualizer();

	EQ_Setting getCustomEqualizer();
	void getEqualizerSettings(vector<EQ_Setting>& vec);
	void setEqualizerSettings(const vector<EQ_Setting>& vec);

	QString getLibraryPath();
	void setLibraryPath(QString path);

	QSize getPlayerSize();
	void setPlayerSize(QSize size);

    QPoint getPlayerPos();
    void setPlayerPos(QPoint p);


    bool getPlayerMaximized();
    void setPlayerMaximized(bool);

    bool getPlayerFullscreen();
    void setPlayerFullscreen(bool);

    QStringList getPlaylist();
    void setPlaylist(QStringList playlist);

	void setLoadPlaylist(bool b);
	bool getLoadPlaylist();

	bool getLoadLastTrack();
	void setLoadLastTrack(bool b);

	MetaData* getLastTrack();
	void setLastTrack(const MetaData& t);
	void updateLastTrack();

    bool getRememberTime();
    void setRememberTime(bool);

    bool getStartPlaying();
    void setStartPlaying(bool);

	void setPlaylistMode(const PlaylistMode& plmode);
	PlaylistMode getPlaylistMode();

	void setPlayerStyle(int);
	int getPlayerStyle();

	void setShowNotifications(bool);
	bool getShowNotification();

    void setNotificationTimout(int);
    int getNotificationTimeout();

    void setNotification(QString);
    QString getNotification();

    void setNotificationScale(int);
    int getNotificationScale();

	void setLastFMSessionKey(QString);
	QString getLastFMSessionKey();

	void setShowLibrary(bool);
	bool getShowLibrary();

    void setLibShownColsTitle(QStringList lst);
    QStringList getLibShownColsTitle();

    void setLibShownColsAlbum(QStringList lst);
    QStringList getLibShownColsAlbum();

    void setLibShownColsArtist(QStringList lst);
    QStringList getLibShownColsArtist();

    void setLibSorting(QList<int> lst);
    QList<int> getLibSorting();

    void setLibShowOnlyTracks(bool);
    bool getLibShowOnlyTracks();

    void setLibLiveSearch(bool);
    bool getLibLiveSheach();

    void setShownPlugin(QString);
    QString getShownPlugin();

	void setMinimizeToTray(bool);
	bool getMinimizeToTray();

	bool getShowSmallPlaylist();
	void setShowSmallPlaylist(bool);

	QString getSoundEngine();
	void setSoundEngine(QString engine);

	bool getStreamRipper();
	void setStreamRipper(bool b);

	bool getStreamRipperWarning();
	void setStreamRipperWarning(bool b);

	QString getStreamRipperPath();
	void setStreamRipperPath(QString path);

	bool getStreamRipperCompleteTracks();
	void setStreamRipperCompleteTracks(bool b);

    bool getStreamRipperSessionPath();
    void setStreamRipperSessionPath(bool b);

	LameBitrate getConvertQuality();
	void setConvertQuality(LameBitrate b);

	QString getConvertTgtPath();
	void setConvertTgtPath(QString str);

	bool getSocketActivated();
	void setSocketActivated(bool b);

	int getSocketFrom();
	void setSocketFrom(int val);

	int getSocketTo();
	void setSocketTo(int val);

	bool getPlaylistNumbers();
	void setPlaylistNumbers(bool b);

	bool getAllowOnlyOneInstance();
	void setAllowOnlyOneInstance(bool b);

    QString getLanguage();
    void setLanguage(QString lang);

    bool getNotifyNewVersion();
    void setNotifyNewVersion(bool b);

    int getSpectrumStyle();
    void setSpectrumStyle(int style);

	friend class SettingsThread;
};





#endif // CSettingsStorage_H
