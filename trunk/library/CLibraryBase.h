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

#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QObject>
#include <QThread>
#include <QFileSystemWatcher>
#include <library/ReloadThread.h>
#include <GUI/library/GUIImportFolder.h>

#include <vector>



class CLibraryBase : public QObject
{
    Q_OBJECT
public:
    explicit CLibraryBase(QObject *parent = 0);

    void loadDataFromDb ();


signals:
	void sig_playlist_created(QStringList&);
	void sig_track_mime_data_available(const vector<MetaData>&);
    void sig_metadata_loaded (vector<MetaData>&);
    void sig_all_albums_loaded(vector<Album>&);
    void sig_all_artists_loaded(vector<Artist>&);
    void sig_tracks_for_playlist_available(vector<MetaData>&);
    void sig_mp3s_loaded(int);

    void sig_should_reload_library();
    void sig_reload_library_finished();
    void sig_reloading_library(int);

    void sig_import_result(bool);
    void sig_change_id3_tags(const vector<MetaData>&);

    void sig_delete_answer(QString);




public slots:
    void baseDirSelected (const QString & baseDir);
    void insertMetaDataIntoDB(vector<MetaData>& in);

    void reloadLibrary();
    void importDirectory(QString);
    void importFiles(const vector<MetaData>&);

    void setLibraryPath(QString);
    void importDirectoryAccepted(const QString&, bool);


/* New way */
    void psl_selected_artists_changed(const QList<int>&);
    void psl_selected_albums_changed(const QList<int>&);
    void psl_selected_tracks_changed(const QList<int>&);

    void psl_prepare_album_for_playlist();
    void psl_prepare_artist_for_playlist();
    void psl_prepare_track_for_playlist(int idx);

    void psl_filter_changed(const Filter&);
    void psl_sortorder_changed(QString, QString, QString);
    void psl_change_id3_tags(const QList<int>& lst);

    void psl_delete_tracks();
    void psl_delete_certain_tracks(const QList<int>&);

private slots:
   void reload_thread_finished();
   void file_system_changed(const QString& path);
   void library_reloading_state_slot(int);


private:
    CDirectoryReader    m_reader;

    QString				m_library_path;
    ReloadThread* 		m_thread;
    QFileSystemWatcher*	m_watcher;
    QString				m_src_dir;
    GUI_ImportFolder*   m_import_dialog;



    CDatabaseConnector*	_db;
    vector<MetaData>	_vec_md;
    vector<Album>		_vec_albums;
    vector<Artist>		_vec_artists;

    QString				_track_sortorder;
    QString				_album_sortorder;
    QString				_artist_sortorder;

    Filter				_filter;

    void 				emit_stuff();
    void				gather_stuff();
    void				delete_tracks(vector<MetaData>& v_md);


};

#endif // CLIBRARYBASE_H
