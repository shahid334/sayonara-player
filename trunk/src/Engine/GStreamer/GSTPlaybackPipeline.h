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

	virtual bool init(GstState state=GST_STATE_READY);
	virtual bool set_uri(gchar* uri);

public slots:

	void play();
	void pause();
	void stop();

	gint64 seek_rel(double percent, gint64 ref_ns);
	gint64 seek_abs(gint64 ns );

	void set_eq_band(QString band_name, double val);
	void unmute();

	void start_timer(qint64 ms);
	void set_speed(float f);
	void set_ready();

    void set_sr_path(QString session_path);


private:

	int			_vol;
	bool		_speed_active;
    float       _speed_val;

	GstElement* _audio_src;
	GstElement* _audio_convert;

	GstElement* _eq_queue;
	GstElement* _equalizer;

	GstElement* _volume;

	GstElement* _audio_sink;
	GstElement* _audio_bin;

	GstElement* _level, *_spectrum;

	GstElement* _level_sink, *_spectrum_sink;
	GstElement* _level_queue, *_spectrum_queue, *_lame_queue;

	GstElement* _lame;
	GstElement* _resampler;
	GstElement* _xingheader;
	GstElement* _id3mux;
	GstElement* _app_sink;
	GstElement* _fake_sink;

	GstElement* _file_queue;
	GstElement* _file_sink;
	GstElement* _file_resampler;
	GstElement* _file_fake_sink;
	GstElement* _file_lame;
	GstElement* _file_header;

	QString		_sr_path;


	GstElement* _tee;

	QTimer* _timer;

    bool _seek(gint64 ns);
	bool tee_connect(GstPadTemplate* tee_src_pad_template, GstElement* queue, QString queue_name);

	virtual bool create_elements();
	virtual bool add_and_link_elements();
	virtual bool configure_elements();


protected slots:
    virtual void _sl_broadcast_clients_changed();
	virtual void _sl_vol_changed();
	virtual void _sl_show_level_changed();
	virtual void _sl_show_spectrum_changed();



};

#endif
