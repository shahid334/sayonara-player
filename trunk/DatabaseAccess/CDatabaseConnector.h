/* CDatabaseConnector.h */

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



#ifndef CDATABASECONNECTOR_H
#define CDATABASECONNECTOR_H

#include <QObject>
#include <QSqlDatabase>
#include <QMap>
#include <vector>
#include <HelperStructs/MetaData.h>
#include <HelperStructs/Equalizer_presets.h>


//class CDatabaseConnector;
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
    bool apply_fixes();







	/********************************************
  	 * LIBRARY
	 *******************************************/

		/*****************
		 *  ARTISTS
		 *****************/
			int getArtistID (const QString & artist);
			int getMaxArtistID();

			QString getArtistName (const int & id);

			void getAllArtists(vector<Artist>& result, QString sortorder = "name asc");

			void getAllArtistsByAlbum(int album, vector<Artist>& result, QString sortorder = "name asc");

			void getAllArtistsBySearchString(QString search, vector<Artist>& result, QString sortorder = "name asc");

			int insertArtistIntoDatabase (const QString & artist);
			int insertArtistIntoDatabase (const Artist & artist);


		/*****************
		 *  ALBUMS
		 *****************/
			int getAlbumID (const QString & album);
			int getMaxAlbumID();

			Album getAlbumByID(const int& id);

			QString getAlbumName (const int & id);

			void getAllAlbums(vector<Album>& result, QString sortorder = "name asc");

			void getAllAlbumsByArtist(int artist, vector<Album>& result, QString filter=QString(""), QString sortorder = "name asc");

			void getAllAlbumsBySearchString(QString search, vector<Album>& result, QString sortorder = "name asc");

			int insertAlbumIntoDatabase (const QString & album);
			int insertAlbumIntoDatabase (const Album& album);



		/*****************
		 *  TRACKS
		 *****************/
			void getAllTracksByAlbum(int album, vector<MetaData>& result, QString filter=QString(""), QString sort="artist asc");

			void getAllTracksByArtist(int artist, vector<MetaData>& result, QString filter=QString(""), QString sort="artist asc");

			void getAllTracksBySearchString(QString search, vector<MetaData>& result, QString sort="artist asc");

			int insertTrackIntoDatabase (MetaData & data,int artistID, int albumID);
			int updateTrack(MetaData& data);

			int getTracksFromDatabase (std::vector <MetaData> & returndata, QString sort="artist asc");
			MetaData getTrackById(int id);
			int getTrackByPath(QString path);

			int deleteTracks(std::vector<MetaData>&);


			/*
			 * Playlist
			 * */
			int createPlaylist(QString playlist_name);
			QString getPlaylistNameById(int playlist_id);
			int getPlaylistIdByName(QString playlist_name);
			bool getPlaylistById(int playlist_id, CustomPlaylist& pl);
			bool storePlaylist(const vector<MetaData>& vec_md, QString playlist_name);
			bool storePlaylist(const vector<MetaData>& vec_md, int playlist_id);
			bool deletePlaylist(int playlist_id);
			bool emptyPlaylist(int playlist_id);
			bool insertTrackIntoPlaylist(MetaData& md, int playlist_id, int pos);
			bool getAllPlaylists(QMap<int, QString>& mapping);
			bool deleteFromAllPlaylists(int track_id);

			/*
			 * Streams
			 * */
			bool getAllStreams(QMap<QString, QString>& result);
			bool deleteStream(QString name);
			bool addStream(QString name, QString url);

    void deleteTracksAlbumsArtists();



	/********************************************
	 * SETTINGS
	 *******************************************/
		void load_setting(QString key, QVariant& val);
		void store_setting(QString key, QVariant val);




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


    QString append_track_sort_string(QString querytext, QString sort);




};

#endif // CDATABASECONNECTOR_H
