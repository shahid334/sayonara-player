/*
 * LyricLookup.h
 *
 *  Created on: May 21, 2011
 *      Author: luke
 */

#ifndef LYRICLOOKUP_H_
#define LYRICLOOKUP_H_

#include <QString>

class LyricLookup {
public:
	LyricLookup();
	virtual ~LyricLookup();

	void find_lyrics(QString artist, QString song);
};

#endif /* LYRICLOOKUP_H_ */
