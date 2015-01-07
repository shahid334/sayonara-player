/* CDirectoryReader.h */

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


#ifndef DIRECTORY_READER
#define DIRECTORY_READER
#include "HelperStructs/MetaData.h"

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFileInfo>


class CDirectoryReader {
    public:
        CDirectoryReader ();
        virtual ~CDirectoryReader ();

        /**
          * Set filter for files.
          * @param filter list of different filters
          */
		void set_filter(const QStringList& filter);

		// only real files are saved here
		void get_files_in_dir_rec (QDir baseDir, QStringList& files);

		// only real files are saved here

		QStringList get_files_in_dir (QDir baseDir);
		MetaDataList get_md_from_filelist(const QStringList& lst);

    private:
        QStringList m_filters;



};

#endif
