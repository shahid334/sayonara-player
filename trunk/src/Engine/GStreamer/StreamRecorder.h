/* StreamRecorder.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef STREAMRECORDER_H
#define STREAMRECORDER_H

#include "HelperStructs/SayonaraClass.h"
#include "HelperStructs/MetaData/MetaData.h"

class StreamRecorder : public QObject, protected SayonaraClass
{
    Q_OBJECT

public:
    StreamRecorder(QObject *parent = 0);
    virtual ~StreamRecorder();

    void changeTrack(const MetaData& md);
    void activate(bool b);
    QString get_dst_file();


private:

    QString			_sr_recording_dst;
    QString			_pl_file_path;
    QString			_session_path;
    QString			_session_playlist_name;
    MetaDataList	_session_collector;
    MetaData		_md;

    bool            _record_on;
	int				_idx;

    QString check_session_path(QString sr_path);
    bool save();
    void new_session();


};

#endif // STREAMRECORDER_H
