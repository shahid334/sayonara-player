/* GSTPlaybackPipeline.h */

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



#ifndef GSTPLAYBACKPIPELINE_H_
#define GSTPLAYBACKPIPELINE_H_

#include "Engine/GStreamer/GSTPipeline.h"
#include "Engine/Engine.h"

class GSTPlaybackPipeline : public GSTAbstractPipeline
{
	Q_OBJECT


public:
	GSTPlaybackPipeline(Engine* engine, QObject *parent = 0);
	virtual ~GSTPlaybackPipeline();

	bool set_uri(gchar* uri);

public slots:

	void play();
	void pause();
	void stop();

	gint64 seek_rel(float percent, gint64 ref_ns);
	gint64 seek_abs(gint64 ns );

	void set_volume(int vol);
	int get_volume();

	void enable_level(bool b);
	void enable_spectrum(bool b);

	void set_eq_band(QString band_name, double val);
	void unmute();

	void start_timer(qint64 ms);
	void set_speed(float f);


private:

	int			_vol;
	bool		_speed_active;
    float       _speed_val;

	GstElement* _audio_src;
	GstElement* _audio_convert;

	GstElement* _eq_queue;
	GstElement* _equalizer;

	GstElement* _volume;

	GstPad* _app_pad;
	GstPad* _tee_app_pad;

	GstElement* _audio_sink;
	GstElement* _audio_bin;

	GstPadTemplate* _tee_src_pad_template;

	GstElement* _level_audio_convert, *_spectrum_audio_convert;
	GstElement* _level, *_spectrum;

	GstPad*     _level_pad, *_spectrum_pad;
	GstPad*     _tee_level_pad, *_tee_spectrum_pad;

	GstElement* _level_sink, *_spectrum_sink;
	GstElement* _level_queue, *_spectrum_queue;


	GstElement* _tee;

	QTimer* _timer;

    bool _seek(gint64 ns);

};

#endif
