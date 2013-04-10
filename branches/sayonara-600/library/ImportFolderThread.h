/* ImportFolderThread.h */

/* Copyright (C) 2013  Lucio Carreras
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



#ifndef IMPORTFOLDERTHREAD_H
#define IMPORTFOLDERTHREAD_H

#include <QThread>
#include <QMap>
#include <QString>
#include <QStringList>
#include "HelperStructs/MetaData.h"

#define IMPORT_MODE_EXTRACT 0
#define IMPORT_MODE_COPY 1

class ImportFolderThread : public QThread
{
    Q_OBJECT

public:
    explicit ImportFolderThread(QObject *parent = 0);
    void run();

    void set_src_dir(QString dir);
    void get_filelist(QStringList& lst);
    void set_may_terminate(bool);
    void set_cancelled();
    void get_md_map(QMap<QString, MetaData>& map);
    int get_n_tracks();


    
signals:
    void sig_done();
    void sig_progress(int);

public slots:

public:


private:
    QString                 _src_dir;
    QStringList             _filelist;
    bool                    _may_terminate;
    bool                    _cancelled;
    QMap<QString, MetaData> _map;





    
};

#endif // IMPORTFOLDERTHREAD_H
