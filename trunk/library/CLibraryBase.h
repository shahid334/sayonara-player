#ifndef CLIBRARYBASE_H
#define CLIBRARYBASE_H

#include <QObject>
#include <QThread>
#include <QFileSystemWatcher>
#include <library/ReloadThread.h>
#include "HelperStructs/CDirectoryReader.h"
#include "DatabaseAccess/CDatabaseConnector.h"

class CLibraryBase : public QObject
{
    Q_OBJECT
public:
    explicit CLibraryBase(QObject *parent = 0);

    void loadDataFromDb ();

signals:
	void playlistCreated(QStringList&);
    void signalMetaDataLoaded (vector<MetaData>& in);
    void allAlbumsLoaded(vector<Album>&);
    void allArtistsLoaded(vector<Artist>&);
    void mp3s_loaded_signal(int);

    void library_should_be_reloaded();
    void reloading_library_finished();
    void reloading_library(int);


public slots:
    void baseDirSelected (const QString & baseDir);
    void insertMetaDataIntoDB(vector<MetaData>& in);
    void getAllArtistsAlbumsTracks();
    void getAllArtists();
    void getArtistsByAlbum(int album);
    void getAllAlbums();
    void getAlbumsByArtist(int artist);
    void getTracksByAlbum(int);
    void getTracksByArtist(int);
    void reloadLibrary();
    void setLibraryPath(QString);

private slots:
   void reload_thread_finished();
   void file_system_changed(const QString& path);
   void library_reloading_state_slot(int);


private:
    CDirectoryReader    m_reader;

    QString				m_library_path;
    ReloadThread* 		m_thread;
    QFileSystemWatcher*	m_watcher;




};

#endif // CLIBRARYBASE_H
