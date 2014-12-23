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
#include "GUI/engine/StyleTypes.h"
#include "library/Sorting.h"

#include <QObject>
#include <QSqlDatabase>
#include <QMap>
#include <QList>

using namespace Sort;


//#define DEBUG_DB

#define INDEX_SIZE 3

#define DB_TRY_OPEN(db) if (!this -> db->isOpen()) \
                            this -> db->open()

#define DB_RETURN_NOT_OPEN_VOID(db) DB_TRY_OPEN(db); \
									if (!this -> db->isOpen()) \
                                    return

#define DB_RETURN_NOT_OPEN_INT(db) DB_TRY_OPEN(db); \
									if (!this -> db->isOpen()) \
									return -1

#define DB_RETURN_NOT_OPEN_BOOL(db) DB_TRY_OPEN(db); \
									if (!this -> db->isOpen()) \
									return false

#define DB_RETURN_NOT_OPEN_STRING(db) DB_TRY_OPEN(db); \
										if(!this->db->isOpen()) \
										return ""



class CDatabaseConnector : public QObject
{
    Q_OBJECT
public:
    static CDatabaseConnector* getInstance();
    virtual ~CDatabaseConnector();
    void closeDatabase();


	/********************************************
  	 * LIBRARY
	 *******************************************/


		/*****************
		 *  ARTISTS
		 *****************/

			bool _db_fetch_artists(QSqlQuery& q, ArtistList& result);

			int getArtistID (const QString & artist);
            bool getArtistByID( const int& id, Artist& artist);
			int getMaxArtistID();

			void getAllArtists(ArtistList& result, SortOrder sortorder = ArtistNameAsc, bool also_empty=false);

			void getAllArtistsByAlbum(int album, ArtistList& result, SortOrder sortorder = ArtistNameAsc);

			void getAllArtistsBySearchString(Filter filter, ArtistList& result, SortOrder sortorder = ArtistNameAsc);

			int insertArtistIntoDatabase (const QString & artist);
			int insertArtistIntoDatabase (const Artist & artist);


		/*****************
		 *  ALBUMS
		 *****************/
			bool _db_fetch_albums(QSqlQuery& q, AlbumList& result);

            int getAlbumID (const QString& album);
			int getMaxAlbumID();

            bool getAlbumByID(const int& id, Album& album);

			void getAllAlbums(AlbumList& result, SortOrder sortorder=AlbumNameAsc, bool also_empty=false);

			void getAllAlbumsByArtist(int artist, AlbumList& result, Filter filter=Filter(), SortOrder sortorder = AlbumNameAsc);
			void getAllAlbumsByArtist(QList<int> artists, AlbumList& result, Filter filter=Filter(), SortOrder sortorder = AlbumNameAsc);

			void getAllAlbumsBySearchString(Filter filter, AlbumList& result, SortOrder sortorder = AlbumNameAsc);

			int insertAlbumIntoDatabase (const QString & album);
            int insertAlbumIntoDatabase (const Album& album);

			int updateAlbum(const Album& album);



		/*****************
		 *  TRACKS
		 *****************/

			bool _db_fetch_tracks(QSqlQuery& q, MetaDataList& result);

			void getAllTracksByAlbum(int album, MetaDataList& result, Filter filter=Filter(), SortOrder sortorder = TrackArtistAsc, int discnumber=-1);
			void getAllTracksByAlbum(QList<int> albums, MetaDataList& result, Filter filter=Filter(), SortOrder sortorder =TrackArtistAsc);

			void getAllTracksByArtist(int artist, MetaDataList& result, Filter filter=Filter(), SortOrder sortorder = TrackArtistAsc);
			void getAllTracksByArtist(QList<int> artists, MetaDataList& result, Filter filter=Filter(), SortOrder sortorder =TrackArtistAsc);

			void getAllTracksBySearchString(Filter filter, MetaDataList& result, SortOrder sortorder = TrackArtistAsc);

			int insertTrackIntoDatabase (const MetaData & data,int artistID, int albumID);
			bool updateTrack(const MetaData& data);
			int updateTracks(const MetaDataList& lst);

			int getTracksFromDatabase (MetaDataList& returndata, SortOrder sortorder = TrackArtistAsc);
			MetaData getTrackById(int id);
			MetaData getTrackByPath(QString path);
            void getMultipleTracksByPath(QStringList& paths, MetaDataList& v_md);

			bool deleteTrack(int id);

			int deleteTracks(const MetaDataList&);
			int deleteTracks(const QList<int>& ids);



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
            bool insertTrackIntoPlaylist(const MetaData& md, int playlist_id, int pos);
			bool getAllPlaylists(QMap<int, QString>& mapping);
			bool deleteFromAllPlaylists(int track_id);

			/*
			 * Streams
			 * */
			bool getAllStreams(QMap<QString, QString>& result);
			bool deleteStream(QString name);
			bool addStream(QString name, QString url);

            /*
             * Podcasts
             * */
            bool getAllPodcasts(QMap<QString, QString>& result);
            bool deletePodcast(QString name);
            bool addPodcast(QString name, QString url);

			/*
			 * Bookmarks
			 * */

			bool searchBookmarks(int track_id, QMap<quint32, QString>& bookmarks);
			bool insertBookmark(int track_id, quint32 time, QString text);
			bool removeBookmark(int track_id, quint32 time);
			bool removeAllBookmarks(int track_id);

    void deleteTracksAlbumsArtists();
	void show_error(const QString& error_str) const;



	/********************************************
	 * SETTINGS
	 *******************************************/


		bool load_setting(QString key, QString& val) const;
		bool store_setting(QString key, const QVariant& val) const;

   /************************************
    *styles
    ***********************************/

    QList<RawColorStyle> get_raw_color_styles();
    bool insert_raw_color_style_to_db(const RawColorStyle& rcs);
    bool update_raw_color_style(const RawColorStyle& rcs);
    bool raw_color_style_exists(QString name);
    bool delete_raw_color_style(QString name);



public slots:
    /**
      * Store all metadata inside db
      * @param in vector containing metadata
      * @return true on success false if failed
      */
    bool storeMetadata (MetaDataList & in);

    bool load_settings();
    bool store_settings();




protected:
    CDatabaseConnector();


private slots:



private:
    CDatabaseConnector(const CDatabaseConnector&);
	//Settings* _settings;

    QSqlDatabase* _database;
    QString _db_filename;

    QString append_track_sort_string(QString querytext, SortOrder sortorder);

    /**
      * Check if db of m_databaseFileContainer is existent
      * @return true if we can load the db false if not
      */
    bool isExistent ();

    bool createDB();
    bool openDatabase ();
    bool apply_fixes();
    bool check_and_insert_column(QString tablename, QString column, QString sqltype);
    bool check_and_create_table(QString tablename, QString sql_create_str);
    bool check_and_drop_table(QString tablename);

    bool updateAlbumCissearch();
    bool updateArtistCissearch();
    bool updateTrackCissearch();
};

#endif // CDATABASECONNECTOR_H
