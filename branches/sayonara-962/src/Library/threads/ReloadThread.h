/* ReloadThread.h */

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


/*
 * ReloadThread.h
 *
 *  Created on: Jun 19, 2011
 *      Author: luke
 */

#ifndef RELOADTHREAD_H_
#define RELOADTHREAD_H_


#include <QThread>
#include <QString>
#include "DatabaseAccess/CDatabaseConnector.h"
#include <HelperStructs/CDirectoryReader.h>
#include "HelperStructs/MetaData.h"


class ReloadThread : public QThread {

Q_OBJECT

signals:

    void sig_reloading_library(QString);
    void sig_new_block_saved();


public:
	ReloadThread();
	virtual ~ReloadThread();

	void set_lib_path(QString library_path);
	MetaDataList get_metadata();
	int getState();
    void pause();
    void goon();


protected:
    void run();

private:
    int		_state;
    QString _library_path;
    MetaDataList _v_metadata;
    bool    _paused;

    int get_and_save_all_files();
	void get_files_recursive (QDir base_dir, MetaDataList& v_md, int* n_files);

	CDatabaseConnector* _db;


};

#endif /* RELOADTHREAD_H_ */
