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

#include <QString>
#include <QPair>
#include <QSize>
#include <vector>

#include <HelperStructs/Equalizer_presets.h>
#include <HelperStructs/PlaylistMode.h>
using namespace std;

/**
  * Class to check if we are running the first time.
  * If yes we are doing bootstrapping. It is a Singleton
  */
class CSettingsStorage
{
public:
    static CSettingsStorage * getInstance();

    virtual ~CSettingsStorage ();

    /**
      * Check if sayonarra is run the first time
      * @return true if run first time
      */
    bool isRunFirstTime ();

    /**
      * Create directory .sayonara and db
      * @param deleteOld Dont care if config already exists
      * @return true if successfull
      */
    bool runFirstTime (bool deleteOld = false);


    /**
      * Returns Filename of DB as absolute path
      */
    QString getDBFileName ();




    //TODO hash
    QPair<QString,QString> getLastFMNameAndPW ();






private:

    void init();
    bool m_isInit;
    CSettingsStorage() {this -> init();}
    CSettingsStorage(const CSettingsStorage&);
    CSettingsStorage& operator=(const CSettingsStorage&);

    QString m_dbFile, m_sayonaraPath, m_dbSource;

    // last fm
    QPair<QString,QString> m_lastFm;
    QString m_lfm_sessionKey;
    bool m_lfm_corrections;

    // listen
    int	m_volume;
    int m_last_eq;

    bool m_show_notifications;


    vector<EQ_Setting> m_vec_eqSettings;

    QSize m_player_size;


    // library
    QString m_library_path;
    bool m_show_library;

    // playlist
    QString 		m_playlist;
    bool			m_loadPlaylist;
    Playlist_Mode 	m_playlistmode;

    /* style */
    int				m_style;

    /* shown plugin in player */
    int 			m_shown_plugin;

    /* minimize the player to tray */
    bool m_minimize_to_tray;

    /* show small icons */
    bool m_show_small_playlist;


    /* sound engine */
    QString m_sound_engine;

public:
    void getLastFMNameAndPW (QString & name, QString & pw);
    void setLastFMNameAndPW (const QString & name,const QString & pw);

    void setLastFMCorrections(bool checked);
    bool getLastFMCorrections();

    int getVolume();
    void setVolume(int vol);

	void setLastEqualizer(int);
	int getLastEqualizer();

	EQ_Setting getCustomEqualizer();
	void getEqualizerSettings(vector<EQ_Setting>& vec);
	void setEqualizerSettings(const vector<EQ_Setting>& vec);

	QString getLibraryPath();
	void setLibraryPath(QString path);

	QSize getPlayerSize();
	void setPlayerSize(QSize size);

	QString getPlaylist();
	void setPlaylist(QString playlist);

	void setLoadPlaylist(bool b);
	bool getLoadPlaylist();

	void setPlaylistMode(const Playlist_Mode& plmode);
	Playlist_Mode getPlaylistMode();

	void setPlayerStyle(int);
	int getPlayerStyle();

	void setShowNotifications(bool);
	bool getShowNotification();

	void setLastFMSessionKey(QString);
	QString getLastFMSessionKey();

	void setShowLibrary(bool);
	bool getShowLibrary();

	void setShownPlugin(int);
	int getShownPlugin();

	void setMinimizeToTray(bool);
	bool getMinimizeToTray();

	bool getShowSmallPlaylist();
	void setShowSmallPlaylist(bool);

	QString getSoundEngine();
	void setSoundEngine(QString engine);

};

#endif // CSettingsStorage_H
