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

#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/globals.h"

#include <QObject>
#include <QSqlDatabase>
#include <QMap>
#include <QList>


using namespace Sort;

#define INDEX_SIZE 3

#define DB_TRY_OPEN(db) if (!this -> db.isOpen()) \
                            this -> db.open()

#define DB_RETURN_NOT_OPEN_VOID(db) if (!this -> db.isOpen()) \
                                    return

#define DB_RETURN_NOT_OPEN_INT(db)if (!this -> db.isOpen()) \
                            return -1

#define DB_RETURN_NOT_OPEN_BOOL(db)if (!this -> db.isOpen()) \
                            return false


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
			Artist getArtistByID( const int& id);
			int getMaxArtistID();

			void getAllArtists(ArtistList& result, ArtistSort sortorder = ArtistNameAsc);

			void getAllArtistsByAlbum(int album, ArtistList& result, ArtistSort sortorder = ArtistNameAsc);

			void getAllArtistsBySearchString(Filter filter, ArtistList& result, ArtistSort sortorder = ArtistNameAsc);

			int insertArtistIntoDatabase (const QString & artist);
			int insertArtistIntoDatabase (const Artist & artist);


		/*****************
		 *  ALBUMS
		 *****************/


			int getAlbumID (const QString & album);
			int getMaxAlbumID();

			Album getAlbumByID(const int& id);

			void getAllAlbums(AlbumList& result, AlbumSort sortorder=AlbumNameAsc);

			void getAllAlbumsByArtist(int artist, AlbumList& result, Filter filter=Filter(), AlbumSort sortorder = AlbumNameAsc);
			void getAllAlbumsByArtist(QList<int> artists, AlbumList& result, Filter filter=Filter(), AlbumSort sortorder = AlbumNameAsc);

			void getAllAlbumsBySearchString(Filter filter, AlbumList& result, AlbumSort sortorder = AlbumNameAsc);

			int insertAlbumIntoDatabase (const QString & album);
			int insertAlbumIntoDatabase (const Album& album);



		/*****************
		 *  TRACKS
		 *****************/

			void getAllTracksByAlbum(int album, MetaDataList& result, Filter filter=Filter(), TrackSort sortorder = TrackArtistAsc);
			void getAllTracksByAlbum(QList<int> albums, MetaDataList& result, Filter filter=Filter(), TrackSort sortorder =TrackArtistAsc);

			void getAllTracksByArtist(int artist, MetaDataList& result, Filter filter=Filter(), TrackSort sortorder = TrackArtistAsc);
			void getAllTracksByArtist(QList<int> artists, MetaDataList& result, Filter filter=Filter(), TrackSort sortorder =TrackArtistAsc);

			void getAllTracksBySearchString(Filter filter, MetaDataList& result, TrackSort sortorder = TrackArtistAsc);

            int insertTrackIntoDatabase (MetaData & data,int artistID, int albumID);
            int updateTrack(MetaData& data);

			int getTracksFromDatabase (MetaDataList& returndata, TrackSort sortorder = TrackArtistAsc);
			MetaData getTrackById(int id);
			MetaData getTrackByPath(QString path);
            void getMultipleTracksByPath(QStringList& paths, MetaDataList& v_md);

			int deleteTracks(MetaDataList&);
			int deleteTrack(MetaData&);


			/*
			 * Playlist
			 * */
			int createPlaylist(QString playlist_name);
			QString getPlaylistNameById(int playlist_id);
			int getPlaylistIdByName(QString playlist_name);
			bool getPlaylistById(int playlist_id, CustomPlaylist& pl);
			bool storePlaylist(const MetaDataList& vec_md, QString playlist_name);
			bool storePlaylist(const MetaDataList& vec_md, int playlist_id);
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

		bool load_setting_bool(QString key, bool def=false);
		QString load_setting_string(QString key, QString def="");
		int load_setting_int(QString key, int def=0);
		QStringList load_setting_strlist(QString key, QChar sep=',');
		 




public slots:
    /**
      * Store all metadata inside db
      * @param in vector containing metadata
      * @return true on success false if failed
      */
    bool storeMetadata (MetaDataList & in);


protected:
    CDatabaseConnector();



private:
    CDatabaseConnector(const CDatabaseConnector&);
    const QString m_createScriptFileName;
    const QString m_databaseContainerFile;
    QSqlDatabase m_database;




    QString append_track_sort_string(QString querytext, TrackSort sortorder);




};

#endif // CDATABASECONNECTOR_H
