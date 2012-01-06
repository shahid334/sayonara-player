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

#include <QObject>
#include <QThread>
#include <QFileSystemWatcher>
#include <library/ReloadThread.h>
#include <GUI/library/GUIImportFolder.h>
#include "HelperStructs/CDirectoryReader.h"
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

    void library_should_be_reloaded();
    void reloading_library_finished();
    void reloading_library(int);

    void sig_import_result(bool);




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
    void importDirectory(QString);
    void importFiles(const vector<MetaData>&);

    void setLibraryPath(QString);
    void importDirectoryAccepted(const QString&, bool);

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




};

#endif // CLIBRARYBASE_H
