/* id3.h */

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


#include "HelperStructs/MetaData/MetaData.h"

#include <QString>
#include <string>

	#include <taglib/tag.h>
	#include <taglib/taglib.h>
	#include <taglib/fileref.h>



namespace ID3{

    bool getMetaDataOfFile(MetaData& md);
	bool setMetaDataOfFile(MetaData& md);
}





