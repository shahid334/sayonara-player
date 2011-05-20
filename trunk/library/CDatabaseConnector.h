#ifndef CDATABASECONNECTOR_H
#define CDATABASECONNECTOR_H

#include <QObject>
#include <QSqlDatabase>
#include <vector>
#include <HelperStructs/MetaData.h>
#include <HelperStructs/Equalizer_presets.h>

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







	/********************************************
  	 * LIBRARY
	 *******************************************/

		/*****************
		 *  ARTISTS
		 *****************/
			int getArtistID (const QString & artist);

			QString getArtistName (const int & id);

			void getAllArtists(vector<Artist>& result);

			void getAllArtistsByAlbum(int album, vector<Artist>& result);

			void getAllArtistsBySearchString(QString search, vector<Artist>& result);

			int insertArtistIntoDatabase (const QString & artist);


		/*****************
		 *  ALBUMS
		 *****************/
			int getAlbumID (const QString & album);

			QString getAlbumName (const int & id);

			void getAllAlbums(vector<Album>& result);

			void getAllAlbumsByArtist(int artist, vector<Album>& result, QString filter=QString(""));

			void getAllAlbumsBySearchString(QString search, vector<Album>& result);

			int insertAlbumIntoDatabase (const QString & album);



		/*****************
		 *  TRACKS
		 *****************/
			void getAllTracksByAlbum(int album, vector<MetaData>& result, QString filter=QString(""));

			void getAllTracksByArtist(int artist, vector<MetaData>& result, QString filter=QString(""));

			void getAllTracksBySearchString(QString search, vector<MetaData>& result);

			int insertTrackIntoDatabase (const MetaData & data,int artistID, int albumID);

			int getTracksFromDatabase (std::vector <MetaData> & returndata);


    void deleteTracksAlbumsArtists();



	/********************************************
	 * SETTINGS
	 *******************************************/
		void load_settings_eq();
		void load_settings_lastfm();

	    /**
	      *Stores all settings inside Settings Singleton. The Settings Container will be fetched from singleton
	      */
	    void store_settings_lastfm ();


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
