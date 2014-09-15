/* LibraryImporter.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef LIBRARYIMPORTER_H
#define LIBRARYIMPORTER_H

#include <QObject>
#include <QWidget>
#include <QStringList>
#include "library/threads/ImportCachingThread.h"
#include "library/threads/ImportCopyThread.h"
#include "HelperStructs/CSettingsStorage.h"
#include "GUI/library/ImportFolderDialog/GUIImportFolder.h"
#include "DatabaseAccess/CDatabaseConnector.h"

class LibraryImporter : public QObject
{
    Q_OBJECT
public:
    explicit LibraryImporter(QWidget* main_window, QObject *parent = 0);
    
signals:
    void sig_lib_changes_allowed(bool);
    void sig_import_result(bool);
    
public slots:
    void psl_import_files(const QStringList&);
    void psl_import_dir(const QString&);

private slots:

    void copy_thread_finished();

    void caching_thread_finished();
    void caching_thread_done();

    void import_dialog_opened();
    void import_dialog_closed();

    void accept_import(const QString&, bool);
    void cancel_import();
    void import_progress(int);



private:
    GUI_ImportFolder*       _import_dialog;
    ImportCachingThread*    _caching_thread;
    ImportCopyThread*       _copy_thread;
    CSettingsStorage*       _settings;
    CDatabaseConnector*     _db;


    bool                    _copy_to_lib;
    QStringList             _src_files;
    QString                 _import_to;
    QString                 _lib_path;
    QWidget*                _main_window;

};

#endif // LIBRARYIMPORTER_H
