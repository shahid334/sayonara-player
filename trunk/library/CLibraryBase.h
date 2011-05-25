#ifndef CLIBRARYBASE_H
#define CLIBRARYBASE_H

#include <QObject>
#include "library/CDirectoryReader.h"
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


private:
    CDirectoryReader    m_reader;

    QString				m_library_path;


};

#endif // CLIBRARYBASE_H
