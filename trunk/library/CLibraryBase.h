#ifndef CLIBRARYBASE_H
#define CLIBRARYBASE_H

#include <QObject>
#include "library/CDirectoryReader.h"
#include "library/CDatabaseConnector.h"

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

public slots:
    void baseDirSelected (const QString & baseDir);
    void insertMetaDataIntoDB(vector<MetaData>& in);

    void getAllArtists();
    void getArtistsByAlbumName(QString album);
    void getAllAlbums();
    void getAlbumsByArtistName(QString artist);
    void getTracksByAlbumAndArtistName(QString album, QString artist);


private:
    CDirectoryReader m_reader;
    CDatabaseConnector m_database;


};

#endif // CLIBRARYBASE_H
