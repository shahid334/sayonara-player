/* CLibraryBase.h */

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


#ifndef CLIBRARYBASE_H
#define CLIBRARYBASE_H

#include "library/ReloadThread.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/library/ImportFolderDialog/GUIImportFolder.h"
#include "application.h"

#include <QThread>
#include <QStringList>
#include <QFileSystemWatcher>


class Application;
class CLibraryBase : public QObject
{
    Q_OBJECT
public:
    CLibraryBase(Application* app, QObject *parent = 0);

    void loadDataFromDb ();


signals:
	void sig_playlist_created(QStringList&);
	void sig_track_mime_data_available(const MetaDataList&);
    void sig_all_tracks_loaded (MetaDataList&);
    void sig_all_albums_loaded(AlbumList&);
    void sig_all_artists_loaded(ArtistList&);
    void sig_tracks_for_playlist_available(MetaDataList&);
    void sig_mp3s_loaded(int);

    void sig_should_reload_library();
    void sig_reload_library_finished();
    void sig_reloading_library(QString &);

    void sig_import_result(bool);
    void sig_change_id3_tags(const MetaDataList&);

    void sig_delete_answer(QString);
    void sig_play_next_tracks(const MetaDataList&);




public slots:
    void baseDirSelected (const QString & baseDir);
    void insertMetaDataIntoDB(MetaDataList& in);

    void reloadLibrary();
    void refresh();
    void importDirectory(QString);
    void importFiles(const MetaDataList&);

    void setLibraryPath(QString);
    void importDirectoryAccepted(const QString&, bool);


/* New way */
    void psl_selected_artists_changed(const QList<int>&);
    void psl_selected_albums_changed(const QList<int>&);
    void psl_selected_tracks_changed(const QList<int>&);

    void psl_prepare_album_for_playlist();
    void psl_prepare_artist_for_playlist();
    void psl_prepare_track_for_playlist(int idx);

    void psl_filter_changed(const Filter&, bool force=false);
    void psl_sortorder_changed(Sort::SortOrder, Sort::SortOrder, Sort::SortOrder);
    void psl_change_id3_tags(const QList<int>& lst);

    void psl_delete_tracks(int);
    void psl_delete_certain_tracks(const QList<int>&,int);

    void psl_play_next_all_tracks();
    void psl_play_next_tracks(const QList<int>&);

private slots:
   void reload_thread_finished();
   void file_system_changed(const QString& path);
   void library_reloading_state_slot(QString);
   void library_reloading_state_new_block();


private:
    Application*            m_app;
    CDirectoryReader    m_reader;

    QString				m_library_path;
    ReloadThread* 		m_thread;
    QFileSystemWatcher*	m_watcher;
    QString				m_src_dir;
    GUI_ImportFolder*   m_import_dialog;

	int					_reload_progress;

    CDatabaseConnector*	_db;
    MetaDataList        _vec_md;
    AlbumList			_vec_albums;
    ArtistList			_vec_artists;

    Sort::SortOrder		_track_sortorder;
    Sort::SortOrder		_album_sortorder;
    Sort::SortOrder		_artist_sortorder;

    QList<int>			_selected_artists;
    QList<int>			_selected_albums;
    QList<int>          _selected_tracks;

    Filter				_filter;

    void 				emit_stuff();
    void				delete_tracks(MetaDataList& v_md, int answer);



};

#endif // CLIBRARYBASE_H
