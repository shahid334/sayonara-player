#ifndef PLAYLISTMODE_H_
#define PLAYLISTMODE_H_


#include <iostream>
#include <QList>
#include <QVariant>
#include <QStringList>

using namespace std;

struct Playlist_Mode{

	bool				rep1;
	bool				repAll;
	bool				repNone;
	bool				append;
	bool				shuffle;
	bool				dynamic;

	Playlist_Mode(){
		rep1 = false;
		repAll = false;
		repNone = true;
		append = false;
		shuffle = false;

	}

	void print(){
		cout << "rep1 = " << rep1 << ", "
			<< "repAll = " << repAll << ", "
			<< "repNone = " << repNone << ", "
			<< "append = " << append <<", "
			<< "dynamic = " << dynamic << ","
			<< endl;
	}

	QString toString(){
		QString str;
		str += (append ? "1" : "0")  + QString(",");
		str += (repAll ? "1" : "0")  + QString(",");
		str += (rep1 ? "1" : "0")  + QString(",");
		str += (repNone ? "1" : "0")  + QString(",");
		str += (shuffle ? "1" : "0")  + QString(",");
		str += (dynamic ? "1" : "0");

		return str;
	}

	void fromString(QString str){
		QStringList list = str.split(',');
		if(list.size() != 6) return;

		append = list[0].toInt() == 1;
		repAll = list[1].toInt() == 1;
		rep1 = list[2].toInt() == 1;
		repNone = list[3].toInt() == 1;
		shuffle = list[4].toInt() == 1;
		dynamic = list[5].toInt() == 1;

	}




};

#endif
