/*
 * PlaylistParser.h
 *
 *  Created on: Feb 12, 2012
 *      Author: luke
 */

#ifndef PLAYLISTPARSER_H_
#define PLAYLISTPARSER_H_

#include "HelperStructs/Helper.h"
#include <HelperStructs/MetaData.h>
#include <QStringList>
#include <QString>

#include <vector>

using namespace std;


namespace PlaylistParser {


	int parse_playlist(QString playlist_file, vector<MetaData>& md);
	bool is_supported_playlist(QString filename);


};

#endif /* PLAYLISTPARSER_H_ */
