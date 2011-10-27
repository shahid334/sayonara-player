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

    // listen
    int	m_volume;
    int m_last_eq;


    vector<EQ_Setting> m_vec_eqSettings;

    QSize m_player_size;


    // library
    QString m_library_path;

    // playlist
    QString 		m_playlist;
    bool			m_loadPlaylist;
    Playlist_Mode 	m_playlistmode;

    // style
    int				m_style;

public:
    void getLastFMNameAndPW (QString & name, QString & pw);
    void setLastFMNameAndPW (const QString & name,const QString & pw);

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

};

#endif // CSettingsStorage_H
