/* SoundcloudLibrary.h */

/* Copyright (C) 2014  Lucio Carreras
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


#ifndef SOUNDCLOUD_H
#define SOUNDCLOUD_H

#include "Library/LocalLibrary.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QThread>
#include <QStringList>
#include <QtXml>


class SoundcloudLibrary : public LocalLibrary
{
	Q_OBJECT

public:
	SoundcloudLibrary(QObject *parent = 0);

};

#endif // LocalLibrary_H
