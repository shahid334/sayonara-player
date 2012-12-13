/* GSTEngine.h */

/* Copyright (C) 2012  Lucio Carreras
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

#define STATE_STOP 0
#define STATE_PLAY 1
#define STATE_PAUSE 2

#define EQ_TYPE_NONE -1
#define EQ_TYPE_KEQ 0
#define EQ_TYPE_10B 1



#ifndef GSTENGINE_H_
#define GSTENGINE_H_



#include "HelperStructs/MetaData.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include "Engine/GStreamer/StreamRecorder.h"
#include "Engine/Engine.h"

#include <gst/gst.h>


#include <QObject>
#include <QDebug>

#include <vector>

using namespace std;

class GST_Engine : public Engine {

	Q_OBJECT
	Q_INTERFACES(Engine)



public:

	GST_Engine();
	virtual ~GST_Engine();

	virtual void init();


private:



	GstElement* _pipeline;
	GstElement* _equalizer;
	GstElement* _volume;
	GstElement* _audio_bin;
	GstElement* _audio_sink;
	GstElement* _file_sink;
	GstPad*		_audio_pad;
	GstBus*		_bus;
	StreamRecorder* _stream_recorder;

	LastTrack*  _last_track;




private slots:
    virtual void sr_initialized(bool);
    virtual void sr_ended();
    virtual void sr_not_valid();
    


public slots:
	virtual void play();
	virtual void stop();
	virtual void pause();
	virtual void setVolume(int vol);

	virtual void jump(int where, bool percent=true);
	virtual void changeTrack(const MetaData&, int pos_sec=0);
	virtual void changeTrack(const QString&, int pos_sec=0 );
    virtual void psl_gapless_track(const MetaData&);
	virtual void eq_changed(int, int);
	virtual void eq_enable(bool);
    virtual void psl_new_stream_session();

 	virtual void record_button_toggled(bool);

    virtual void psl_sr_set_active(bool);



public:
	// callback -> class
	void		state_changed();
	void		set_cur_position(quint32);
	void		set_track_finished();
    void        set_about_to_finish();

	virtual void 	load_equalizer(vector<EQ_Setting>&);
	virtual int		getState();
	virtual QString	getName();


private:
	CSettingsStorage* _settings;

	void init_play_pipeline();







};

#endif /* GSTENGINE_H_ */
