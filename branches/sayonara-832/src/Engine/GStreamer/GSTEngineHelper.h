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



gboolean
bus_state_changed(GstBus *bus, GstMessage *msg, gpointer data);


gboolean player_change_file(GstBin* pipeline, void* data);

void
new_buffer(GstElement* sink, gpointer data);

gboolean
level_handler (GstBus * bus, GstMessage * message, gpointer data);

gboolean
spectrum_handler (GstBus * bus, GstMessage * message, gpointer data);

gboolean
show_position(GstElement* pipeline);




#endif // GSTENGINECALLBACKS_H
