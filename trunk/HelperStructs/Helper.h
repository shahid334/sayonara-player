/*
 * Helper.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#ifndef _HELPER_H
#define _HELPER_H



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
		int sec = msec / 1000;
		int min = sec / 60;
		int secs = sec % 60;
		return cvtNum2String(min, 2) + ":" + cvtNum2String(secs, 2);

	}

	inline string replace(string input, string toReplace, string replaceWith){

                unsigned int i=0;
                uint pos = 0;
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

	QString get_cover_path(QString artist, QString album);

	QString calc_cover_google_adress(QString artist, QString album);
	QString calc_cover_lfm_adress(QString artist, QString album);
	QString calc_album_lfm_adress(QString album);
	QString calc_search_album_adress(QString album);
	QString calc_search_artist_adress(QString artist);

	QString calc_cover_token(QString artist, QString album);

	template <typename T> void randomize_list(QList<T>& list){
		srand ( time(NULL) );

		for(int i=0; i<list.size(); i++){

			list.swap(i, rand() % list.size());
		}

	}





};
#endif
