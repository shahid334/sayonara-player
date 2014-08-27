/* CoverLocation.h */

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



#ifndef COVERLOCATION_H
#define COVERLOCATION_H

#include "HelperStructs/MetaData.h"
class CoverLocation;

class CoverLocation
{

private:



public:

	QString google_url;
	QString cover_path;
	bool valid;

	CoverLocation();

	void print() const;

	static CoverLocation get_cover_location(const QString& album_name, const QString& artist_name, bool big=false);
	static CoverLocation get_cover_location(const QString& album_name, const QStringList& artists, bool big=false);
	static CoverLocation get_cover_location(int album_id, bool big=false);
	static CoverLocation get_cover_location(const Album& album, bool big=false);
	static CoverLocation get_cover_location(const Artist& artist, bool big=false);
	static CoverLocation get_cover_location(const QString& artist, bool big=false);
	static CoverLocation get_cover_location(const MetaData& md, bool big=false);
	static CoverLocation getInvalidLocation();

};

Q_DECLARE_METATYPE(CoverLocation)

#endif // COVERLOCATION_H
