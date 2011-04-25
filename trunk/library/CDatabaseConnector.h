#ifndef CDATABASECONNECTOR_H
#define CDATABASECONNECTOR_H

#include <QObject>
#include <QSqlDatabase>
#include <vector>
#include <MetaData.h>

class CDatabaseConnector : public QObject
{
    Q_OBJECT
public:
    explicit CDatabaseConnector(QObject *parent = 0);
    virtual ~CDatabaseConnector();

    /**
      * Check if db of m_databaseFileContainer is existent
      * @return true if we can load the db false if not
      */
    bool isExistent ();

    bool createDB();
    bool openDatabase ();


    /**
      *Stores all settings inside Settings Singleton. The Settings Container will be fetched from singleton
      */
    void storeSettingsFromStorage ();

    /**
      * Stores all settings from inside db into singleton
      */
    void fillSettingsStorage ();



    /**
      * Function to check if artist is already present in database
      * @param artist name of artist
      * @return artist id or -1 if not found
      */
    int getArtistID (const QString & artist);
    QString getArtistName (const int & id);

    /**
      * Insert artist into database
      * @param artist name of artist
      * @return artist id or -1 if failed
      */
    int insertArtistIntoDatabase (const QString & artist);

    /**
      * Function to check if album is already present in database
      * @param album name of album
      * @return album id or -1 if not found
      */
    int getAlbumID (const QString & album);
    QString getAlbumName (const int & id);
    /**
      * Insert album into database
      * @param album name of artist
      * @return album id or -1 if failed
      */
    int insertAlbumIntoDatabase (const QString & album);

    int insertTrackIntoDatabase (const MetaData & data,int artistID, int albumID);

    int getTracksFromDatabase (std::vector <MetaData> & returndata);


public slots:
    /**
      * Store all metadata inside db
      * @param in vector containing metadata
      * @return true on success false if failed
      */
    bool storeMetadata (std::vector<MetaData> & in);

signals:

public slots:

private:
    const QString m_createScriptFileName;
    const QString m_databaseContainerFile;
    QSqlDatabase m_database;



};

#endif // CDATABASECONNECTOR_H
