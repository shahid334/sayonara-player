/*
 * LyricServer.h
 *
 *  Created on: Feb 3, 2012
 *      Author: luke
 */

#ifndef LYRICSERVER_H_
#define LYRICSERVER_H_

#include <QString>
#include <QMap>

struct ServerTemplate {

	QString display_str;
	QString server_adress;
	QMap<QString, QString> replacements;
	QString call_policy;
	QString start_tag;
	QString end_tag;
	bool include_start_tag;
	bool include_end_tag;
	bool is_numeric;
	bool to_lower;
	QString error;


	void addReplacement(QString rep, QString rep_with){
		replacements[rep] = rep_with;
	}

	//QString start_tag;
	//QString end_tag;
};

#endif /* LYRICSERVER_H_ */
