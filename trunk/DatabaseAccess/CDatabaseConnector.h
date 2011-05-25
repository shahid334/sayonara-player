#ifndef CDATABASECONNECTOR_H
#define CDATABASECONNECTOR_H

#include <QObject>
#include <QSqlDatabase>
#include <vector>
#include <HelperStructs/MetaData.h>
#include <HelperStructs/Equalizer_presets.h>


class CDatabaseConnector;
class CDatabaseConnector : public QObject
{
    Q_OBJECT
public:
    static CDatabaseConnector* getInstance();
    virtual ~CDatabaseConnector();

    /**
      * Check if db of m_databaseFileContainer is existent
      * @return true if we can load the db false if not
      */
    bool isExistent ();

    bool createDB();
    bool openDatabase ();
    bool load_settings();
    bool store_settings();







	/********************************************
  	 * LIBRARY
	 *******************************************/

		/*****************
		 *  ARTISTS
		 *****************/
			int getArtistID (const QString & artist);
			int getMaxArtistID();

			QString getArtistName (const int & id);

			void getAllArtists(vector<Artist>& result);

			void getAllArtistsByAlbum(int album, vector<Artist>& result);

			void getAllArtistsBySearchString(QString search, vector<Artist>& result);

			int insertArtistIntoDatabase (const QString & artist);
			int insertArtistIntoDatabase (const Artist & artist);


		/*****************
		 *  ALBUMS
		 *****************/
			int getAlbumID (const QString & album);
			int getMaxAlbumID();

			QString getAlbumName (const int & id);

			void getAllAlbums(vector<Album>& result);

			void getAllAlbumsByArtist(int artist, vector<Album>& result, QString filter=QString(""));

			void getAllAlbumsBySearchString(QString search, vector<Album>& result);

			int insertAlbumIntoDatabase (const QString & album);
			int insertAlbumIntoDatabase (const Album& album);



		/*****************
		 *  TRACKS
		 *****************/
			void getAllTracksByAlbum(int album, vector<MetaData>& result, QString filter=QString(""));

			void getAllTracksByArtist(int artist, vector<MetaData>& result, QString filter=QString(""));

			void getAllTracksBySearchString(QString search, vector<MetaData>& result);

			int insertTrackIntoDatabase (const MetaData & data,int artistID, int albumID);
			int updateTrack(MetaData& data);

			int getTracksFromDatabase (std::vector <MetaData> & returndata);


    void deleteTracksAlbumsArtists();



	/********************************************
	 * SETTINGS
	 *******************************************/
		void load_settings_eq();
		void store_settings_eq();
		void load_settings_lastfm();
	    void store_settings_lastfm ();



public slots:
    /**
      * Store all metadata inside db
      * @param in vector containing metadata
      * @return true on success false if failed
      */
    bool storeMetadata (std::vector<MetaData> & in);


protected:
    CDatabaseConnector();



private:
    CDatabaseConnector(const CDatabaseConnector&);
    const QString m_createScriptFileName;
    const QString m_databaseContainerFile;
    QSqlDatabase m_database;




};

#endif // CDATABASECONNECTOR_H
