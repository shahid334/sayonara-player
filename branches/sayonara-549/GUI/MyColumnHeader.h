/* MyColumnHeader.h */

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



/*
 * MyColumnHeader.h
 *
 *  Created on: 19.12.2012
 *      Author: lugmair
 */

#ifndef MYCOLUMNHEADER_H_
#define MYCOLUMNHEADER_H_

#include <QString>
#include <QDebug>
#include "HelperStructs/globals.h"

#define COL_HEADER_SIZE_TYPE_ABS 0
#define COL_HEADER_SIZE_TYPE_REL 1

class ColumnHeader {

	private:
		QString 		_title;

		bool 			_switchable;
		bool 			_abs_size;

		int 			_preferred_size_abs;
		double			_preferred_size_rel;

		Sort::SortOrder _sort_asc;
		Sort::SortOrder _sort_desc;

	public:

        ColumnHeader(QString title, bool switchable, Sort::SortOrder sort_asc, Sort::SortOrder sort_desc, int preferred_size_abs){

			_title = title;
			_switchable = switchable;
			_abs_size = true;
			_sort_asc = sort_asc;
			_sort_desc = sort_desc;
            _preferred_size_abs = preferred_size_abs;
            _abs_size = true;
		}

        ColumnHeader(QString title, bool switchable, Sort::SortOrder sort_asc, Sort::SortOrder sort_desc, double preferred_size_rel, int min_size){

            _title = title;
            _switchable = switchable;
            _abs_size = true;
            _sort_asc = sort_asc;
            _sort_desc = sort_desc;
            _preferred_size_abs = min_size;
            _preferred_size_rel = preferred_size_rel;
            _abs_size = false;

        }

		void set_preferred_size_abs(int preferred_size){
			_preferred_size_abs = preferred_size;
			_preferred_size_rel = 0;
			_abs_size = true;
		}

		void set_preferred_size_rel(double preferred_size){
			_preferred_size_rel = preferred_size;
			_preferred_size_abs = 0;
			_abs_size = false;
		}

		QString getTitle(){ return _title; }
		bool getSwitchable(){ return _switchable; }
		int getSizeType(){ return (_abs_size ? COL_HEADER_SIZE_TYPE_ABS : COL_HEADER_SIZE_TYPE_REL); }
		int get_preferred_size_abs(){ return _preferred_size_abs; }
        double get_preferred_size_rel(){ return _preferred_size_rel; }
		Sort::SortOrder get_asc_sortorder() { return _sort_asc; }
		Sort::SortOrder get_desc_sortorder() { return _sort_desc; }

};



#endif /* MYCOLUMNHEADER_H_ */
