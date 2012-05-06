/* Helper.h */

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
 * Helper.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#ifndef _HELPER_H
#define _HELPER_H

#include <HelperStructs/MetaData.h>

#include <string>
#include <sstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <QString>
#include <vector>
#include <QList>
#include <ctime>

using namespace std;



namespace Helper{

	template<typename T>
	QString cvtNum2String(T num, uint digits=0){
		stringstream sstr;
		sstr << num;
		string tmpString=sstr.str();

		while(tmpString.size() < digits){
			tmpString = string("0") + tmpString;
		}

		return QString::fromStdString(tmpString);

	}


	template<typename T>
	QString cvtSomething2QString(T sth, uint digits=0){
		stringstream sstr;
		sstr << sth;

		string tmpString=sstr.str();

		while(tmpString.size() < digits){
			tmpString = string("0") + tmpString;
		}

		return QString(tmpString.c_str());
	}


	inline void cvtSecs2MinAndSecs(long int secs, int* tgt_min, int* tgt_sec)
	{

		*tgt_min = secs / 60;
		*tgt_sec = secs % 60;

	}


	inline QString cvtMsecs2TitleLengthString(long int msec){

		bool show_hrs = false;

		int sec = msec / 1000;
		int min = sec / 60;
		int secs = sec % 60;
		int hrs = min / 60;
		int days = hrs / 24;

		QString final_str;

		if(days > 0){
			final_str += QString::number(days) + "d ";
			hrs -= (days * 24);
			show_hrs = true;
		}

		if(hrs > 0 || show_hrs){
			final_str += QString::number(hrs) + "h ";
			min -= (hrs * 60);
		}

		final_str +=  cvtNum2String(min, 2) + ":" + cvtNum2String(secs, 2);

		return final_str;

	}

	inline string replace(string input, string toReplace, string replaceWith){

                unsigned int i=0;
                string::size_type pos = 0;
		while(true){

			pos = input.find_first_of(toReplace.c_str(), i);
			if(pos == input.npos) break;

			input.replace(pos, toReplace.length(), replaceWith);

			i = pos + toReplace.length();
			if(i > input.length()) break;
		}

		return input;
	}

	inline string replace_chars(string input, char c, char new_c){

		char* str = (char*) input.c_str();
		for(uint i=0; i<input.size(); i++){
			if(str[i] == c) str[i] = new_c;
		}

		return string(str);
	}


	inline string replace_whitespaces(string input, char new_c){

			char* str_src = (char*) input.c_str();
			char* str_dst = (char*) input.c_str();

			bool last_char_replaced = false;
			int jumped = 0;
			int cur_idx_dst = 0;
			for(uint i=0; i<input.size(); i++){
				if(isspace(str_src[i]) && !last_char_replaced) {
					str_dst[cur_idx_dst] = new_c;
					last_char_replaced = true;
					cur_idx_dst++;
				}

				else if(isspace(str_src[i]) && last_char_replaced){
					jumped++;
				}

				else {
					last_char_replaced = false;
					cur_idx_dst++;
				}
			}

			str_dst[cur_idx_dst] = '\0';

			cout << "str_dst = " << string(str_dst) << endl;

			return string(str_dst);
		}

        string trim(const string &toTrim);

        inline QString cvtQString2FirstUpper(QString str){

		QString retString = "";

		bool newword = true;
		for(int i=0; i<str.length(); i++){
			if(newword && str.at(i).isLower()){

				retString.append(str.at(i).toUpper());
			}

			else retString.append(str.at(i));

			if(str.at(i) == ' ') newword = true;
			else newword = false;
		}

		return retString;
	}

	QString getIconPath();

	QString get_artist_image_path(QString artist);
	QString get_cover_path(QString artist, QString album);

	QString calc_google_image_search_adress(QString searchstring,  QString size, QString filetype);
	QString calc_google_artist_adress(QString artist);
	QString calc_google_album_adress(QString artist, QString album);


	QString calc_lfm_artist_adress(QString artist);
	QString calc_lfm_album_adress(QString artist, QString album);


	QString calc_cover_token(QString artist, QString album);
	QStringList get_soundfile_extensions();
	bool is_soundfile(QString filename);

	template <typename T> void randomize_list(QList<T>& list){
		srand ( time(NULL) );

		for(int i=0; i<list.size(); i++){

			list.swap(i, rand() % list.size());
		}
	}


	/* Checks if track exists
	 * if track does not exist
	 * it is deleted from database
	 * For online tracks it always returns true
	 * ATTENTION:
	 * Wrap this method, because for sure library has to be notified
	 */
	bool checkTrack(const MetaData& md);


	bool read_file_into_str(QString filename, QString& content);
	bool read_http_into_str(QString url, QString& content);


	Album get_album_from_metadata(const MetaData& md);


	QString easy_tag_finder(QString tag, QString& xml_doc);




};
#endif
