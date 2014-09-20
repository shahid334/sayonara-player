/* GSTEngineHelper.h */

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


#ifndef GSTENGINEHELPER_H
#define GSTENGINEHELPER_H


#include <QDebug>
#include <QString>
#include <gst/gst.h>
#include <gst/gstcaps.h>
#include "Engine/Engine.h"

#define ENGINE_DEBUG if(_debug) qDebug() << Q_FUNC_INFO << "; "

#define CHUNK_SIZE 1024
#define SAMPLE_RATE 44100 /* Samples per second we are sending */
#define AUDIO_CAPS "audio/x-raw-int,channels=2,rate=%d,signed=(boolean)true,width=16,depth=16,endianness=BYTE_ORDER"

#define LOOKUP_LOG(x) log_10[(int) (20000 * x)]

const qint64 MRD = 1000000000;
const qint64 MIO = 1000000;

extern bool _debug;

extern float log_10[20001];
extern float lo_128[128];
const int crop_spectrum_at = 75;

extern bool __start_at_beginning;
extern int __start_pos_beginning;


class MyCaps {

private:
	CapsType _type;
	bool    _sig;
	int     _width;
	int     _channels;
	bool    _parsed;

public:

	MyCaps(){
		_type = CapsTypeUnknown;
		_sig = false;
		_width = -1;
		_channels = -1;
		_parsed = false;
	}

	bool is_parsed(){ return _parsed; }
	void set_parsed(bool b){ _parsed = b; }

	CapsType get_type(){ return _type; }
	bool get_signed() {return _sig; }
	int get_width() { return _width; }
	int get_channels() { return _channels; }

	void parse(GstCaps* caps){
		QString info = gst_caps_to_string(caps);
		//qDebug() << info;

		QStringList lst = info.split(",");
		foreach(QString s, lst){

			s = s.trimmed();
			if(s.startsWith("audio", Qt::CaseInsensitive)){
				if(s.contains("int", Qt::CaseInsensitive)) _type = CapsTypeInt;
				else if(s.contains("float", Qt::CaseInsensitive)) _type = CapsTypeFloat;
				else _type = CapsTypeUnknown;
			}

			else if(s.startsWith("signed", Qt::CaseInsensitive)){
				if(s.contains("true", Qt::CaseInsensitive)) _sig = true;
				else _sig = false;
			}

			else if(s.startsWith("width", Qt::CaseInsensitive)){
				_width = s.right(2).toInt();
			}

			else if(s.startsWith("channels", Qt::CaseInsensitive)){
				_channels = s.right(1).toInt();
				if(_channels > 2) _channels = 2;
			}
		}
		_parsed = true;
	}
};




namespace EngineCallbacks {

	gboolean
	bus_state_changed(GstBus *bus, GstMessage *msg, gpointer data);

	gboolean
	level_handler (GstBus * bus, GstMessage * message, gpointer data);

	gboolean
	spectrum_handler (GstBus * bus, GstMessage * message, gpointer data);

	gboolean
	show_position(GstElement* pipeline);

}


#endif // GSTENGINECALLBACKS_H
