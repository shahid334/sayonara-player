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
#include "HelperStructs/CSettingsStorage.h"
#include "Engine/GStreamer/GSTPlaybackPipeline.h"
#include "Engine/GStreamer/StreamRecorder.h"
#include "Engine/Engine.h"

#include <glib.h>
#include <gst/gst.h>
#include <gst/gstbuffer.h>

#include <QObject>
#include <QDebug>


#include <vector>

using namespace std;




class GSTPlaybackEngine : public Engine {

	Q_OBJECT


public:

	GSTPlaybackEngine();
	virtual ~GSTPlaybackEngine();



// public from Gstreamer Callbacks
	void        emit_buffer(float inv_array_elements, float scale);

	void		set_track_finished();

	void		update_bitrate(qint32 bitrate);
	void		set_about_to_finish(qint64 time2go);
	void		set_cur_position_ms(qint64 pos_ms);

	bool get_show_level();
	bool get_show_spectrum();

	MyCaps* get_caps();
	void do_jump_play();
	void unmute();

	virtual void init();


private:
	
	GSTPlaybackPipeline*	 _pipeline;
	GSTPlaybackPipeline*	 _other_pipeline;

	CSettingsStorage* _settings;
	StreamRecorder* _stream_recorder;

	LastTrack*  _last_track;
	MyCaps*     _caps;


    bool        _show_level;
    bool        _show_spectrum;
    int         _jump_play;
	bool		_wait_for_gapless_track;
	bool		_may_start_timer;

	// methods
	bool set_uri(const MetaData& md, bool* start_play);
	void change_track_gapless(const MetaData& md, int pos_sec=0, bool start_play=true);

public slots:
    virtual void play();
	virtual void stop();
	virtual void pause();
	virtual void set_volume(int vol);

	virtual void jump_abs_s(quint32);
	virtual void jump_abs_ms(quint64);
	virtual void jump_rel(quint32);

	virtual void change_track(const MetaData&, int pos_sec=-1, bool start_play=true);
	virtual void change_track(const QString&, int pos_sec=-1, bool start_play=true );

	virtual void eq_changed(int, int);
	virtual void eq_enable(bool);

	virtual void psl_calc_level(bool);
    virtual void psl_calc_spectrum(bool);
	virtual void psl_set_gapless(bool);

	virtual void psl_set_speed(float f);


};

#endif /* GSTENGINE_H_ */
