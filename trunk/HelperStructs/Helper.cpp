/*
 * Helper.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#include "HelperStructs/Helper.h"

#include <string>
#include <iostream>
#include <sstream>
#include <QString>

#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <QDir>



using namespace std;


template<typename T>
string cvtNum2String(T num){
	stringstream sstr;
	sstr << num;
	return sstr.str();

}


template<typename T>
QString cvtSomething2QString(T sth){
	stringstream sstr;
	sstr << sth;
	return QString(sstr.str().c_str());
}


void cvtSecs2MinAndSecs(int secs, int* tgt_min, int* tgt_sec)
{

	*tgt_min = secs / 60;
	*tgt_sec = secs % 60;

}


string Helper::trim(string toTrim){

	if(toTrim.size() == 0) return toTrim;

	const char* arr_src = toTrim.c_str();

	int count_whitespaces = 0;
	for(int i=toTrim.size()-1; i>=0; i--){
		if(!isspace(arr_src[i])){
			break;
		}

		else count_whitespaces ++;
	}

	char* arr_dst = new char[toTrim.size() - count_whitespaces + 1];

	for(uint i=0; i<toTrim.size() - count_whitespaces; i++){
		arr_dst[i] = arr_src[i];
	}

	arr_dst[toTrim.size() - count_whitespaces] = '\0';

	string retStr = string(arr_dst);
	delete arr_dst;
	return retStr;


}


QString Helper::getIconPath(){

	QString path = "GUI/icons/";

	if(QFile::exists(QDir::homePath() + "/.Sayonara/images")) path = QDir::homePath() + "/.Sayonara/images/";
	cout << "Path = " << path.toStdString() << endl;
	return path;

}
