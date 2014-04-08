/* GSTPipeline.h */

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



#ifndef GSTPIPELINE_H
#define GSTPIPELINE_H

#include <QObject>
#include <QString>
#include <gst/gst.h>
#include <gst/gstbuffer.h>

bool _test_and_error(void* element, QString errorstr);
bool _test_and_error_bool(bool b, QString errorstr);


class GSTPipeline : public QObject
{
	Q_OBJECT

public:
	GSTPipeline(QObject *parent = 0);
	~GSTPipeline();
	GstElement* get_pipeline();
	GstBus* get_bus();

	void play();
	void pause();
	void stop();
	gint64 seek_rel(float percent, gint64 ref_ns);
	gint64 seek_abs(gint64 ns );
	void set_volume(int vol);

	void enable_level(bool b);
	void enable_spectrum(bool b);

	gint64 get_duration_ns();
	guint get_bitrate();
	virtual bool set_uri(gchar* uri);
	virtual bool set_next_uri(gchar* uri);
	virtual gchar* get_next_uri();
	virtual gchar* get_uri();
	void set_eq_band(QString band_name, double val);
	void unmute();

	void set_track_finished();
	void set_gapless(bool gapless);
	bool get_gapless();


protected:

	GstBus*		_bus;
	GstElement* _pipeline;
	GstElement* _equalizer;
	GstElement* _eq_queue;
	GstElement* _volume;

	GstPad* _tee_app_pad;
	GstPad* _app_pad;

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

	gchar* _uri;
	gchar* _next_uri;
	bool  _gapless;

	int _vol;


};

#endif // GSTPIPELINE_H
