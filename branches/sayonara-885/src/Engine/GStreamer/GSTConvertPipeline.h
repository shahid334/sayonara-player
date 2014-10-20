/* GSTConvertPipeline.h */

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



#ifndef GSTCONVERTPIPELINE_H
#define GSTCONVERTPIPELINE_H

#include "HelperStructs/globals.h"
#include "HelperStructs/CSettingsStorage.h"
#include "Engine/GStreamer/GSTPipeline.h"
#include "Engine/Engine.h"



class GSTConvertPipeline : public GSTAbstractPipeline
{
	Q_OBJECT
public:
	explicit GSTConvertPipeline(Engine* engine, QObject *parent = 0);
	virtual ~GSTConvertPipeline();
	bool set_uri(gchar* uri);
	bool set_target_uri(gchar* uri);
	

public slots:
	void play();
	void pause();
	void stop();

	guint get_bitrate();

	void set_quality(LameBitrate quality);

private:
	GstElement* _audio_src;
	GstElement* _lame;
	GstElement* _decoder;
	GstElement* _audio_convert;
	GstElement* _audio_sink;
	GstElement* _resampler;
	GstElement* _id3mux;
	GstElement* _xingheader;
};

#endif // GSTCONVERTPIPELINE_H
