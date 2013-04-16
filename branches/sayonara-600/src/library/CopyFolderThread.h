/* CopyFolderThread.h */

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



#ifndef COPYFOLDERTHREAD_H
#define COPYFOLDERTHREAD_H

#include <QThread>
#include <QString>
#include <QMap>
#include <QString>
#include <QStringList>

#include "HelperStructs/MetaData.h"

#define COPY_FOLDER_THREAD_COPY 0
#define COPY_FOLDER_THREAD_ROLLBACK 1

class CopyFolderThread : public QThread
{
    Q_OBJECT
public:
    explicit CopyFolderThread(QObject *parent = 0);
    void set_vars(QString chosen_dir, QString lib_dir, QString src_dir, QStringList& files, QMap<QString, MetaData>& map);
    int get_n_files();
    int get_copied_files();
    void get_metadata(MetaDataList& v_md);
    void set_cancelled();
    bool get_cancelled();
    void set_mode(int mode);
	int  get_mode();


private:
    QString        _lib_dir;
    QString        _src_dir;
    QString        _chosen_dir;
    QStringList    _files;
    QMap<QString, MetaData> _map;
    MetaDataList   _v_md;
    QStringList    _lst_copied_files;
    QStringList    _created_dirs;
    int            _n_files;
    int            _copied_files;
    int            _mode;
	int            _percent;
    bool           _cancelled;

    void copy();
    void rollback();

    
protected:
    void run();

signals:
    void sig_progress(int);
    
public slots:


    
};

#endif // COPYFOLDERTHREAD_H
