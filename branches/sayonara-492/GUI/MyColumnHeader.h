/*
 * MyColumnHeader.h
 *
 *  Created on: 19.12.2012
 *      Author: lugmair
 */

#ifndef MYCOLUMNHEADER_H_
#define MYCOLUMNHEADER_H_

#include <QString>
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

		ColumnHeader(QString title, bool switchable, Sort::SortOrder sort_asc, Sort::SortOrder sort_desc, int preferred_size_abs, double preferred_size_rel=-1){

			_title = title;
			_switchable = switchable;
			_preferred_size_abs = 25;
			_abs_size = true;
			_sort_asc = sort_asc;
			_sort_desc = sort_desc;

			if(preferred_size_rel < 0){
				_preferred_size_abs = 25;
				_abs_size = true;
			}

			else{
				_preferred_size_rel = preferred_size_rel;
				_abs_size = false;
			}
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
		int get_preferred_size_rel(){ return _preferred_size_rel; }
		Sort::SortOrder get_asc_sortorder() { return _sort_asc; }
		Sort::SortOrder get_desc_sortorder() { return _sort_desc; }

};



#endif /* MYCOLUMNHEADER_H_ */
