/* GSTPlaybackEngine.h */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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


#ifndef GSTPLAYBACKENGINE_H_
#define GSTPLAYBACKENGINE_H_


#define EQ_TYPE_NONE -1
#define EQ_TYPE_KEQ 0
#define EQ_TYPE_10B 1


#include "HelperStructs/MetaData.h"
#include "HelperStructs/Equalizer_presets.h"
#include "Engine/GStreamer/GSTPlaybackPipeline.h"
#include "Engine/GStreamer/StreamRecorder.h"
#include "Engine/Engine.h"

#include <glib.h>
#include <gst/gst.h>
#include <gst/gstbuffer.h>


typedef enum {
	Gapless_NoGapless=0,
	Gapless_AboutToFinish,
	Gapless_TrackFetched,
	Gapless_Playing

} GaplessState;

class GSTPlaybackEngine : public Engine {

	Q_OBJECT

public:

	GSTPlaybackEngine(QObject* parent=0);
	virtual ~GSTPlaybackEngine();


// public from Gstreamer Callbacks
	void        emit_buffer(float inv_array_elements, float scale);

	void		set_track_finished();

	void		update_bitrate(quint32 bitrate);
	void		update_duration();

	void		set_track_ready();

	void		set_about_to_finish(qint64 time2go);
	void		set_cur_position_ms(qint64 pos_ms);


	MyCaps*		get_caps();
	void		unmute();

	virtual void init();


private:
	
	GSTPlaybackPipeline*	_pipeline;
	GSTPlaybackPipeline*	_other_pipeline;

	StreamRecorder*			_stream_recorder;

	MyCaps*					_caps;
	MetaData				_md_gapless;

	GaplessState			_gapless_state;

	bool					_sr_active;
	int						_jump_play_s;

	// methods
	bool set_uri(const MetaData& md);
	void change_track_gapless(const MetaData& md);

public slots:
    virtual void play();
	virtual void stop();
	virtual void pause();


	virtual void jump_abs_ms(quint64 pos_ms);
	virtual void jump_rel(double percent);


	virtual void change_track(const MetaData&);
	virtual void change_track(const QString&);

	virtual void eq_changed(int, int);

	virtual void psl_set_speed(float f);

private slots:
	void _change_sr_active();
	void _change_gapless();

};

#endif /* GSTENGINE_H_ */
