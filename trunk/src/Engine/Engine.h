/* Engine.h */

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



#ifndef ENGINE_H_
#define ENGINE_H_

#include "HelperStructs/MetaData.h"
#include <QObject>
#include <QStringList>

#include <vector>

using namespace std;

#define PLAYBACK_ENGINE "playback_engine"
#define CONVERT_ENGINE "convert_engine"


enum EngineState {
	StatePlay=0,
	StatePause=1,
	StateStop=2
};


enum CapsType {
	CapsTypeUnknown=0,
	CapsTypeInt=1,
	CapsTypeFloat=2
};


class Engine : public QObject {

	Q_OBJECT

protected:
	MetaData	_md;
	qint64		_cur_pos_ms;
	qint32      _vol;

	bool		_scrobbled;
	qint64		_scrobble_begin_ms;
	bool		_is_eq_enabled;
	int			_eq_type;
	EngineState	_state;
	QString		_name;

	bool 		_playing_stream;

	bool		_sr_active;
	bool		_sr_wanna_record;
	bool		_gapless;


public:
	virtual EngineState	getState() { return _state; }
	virtual QString	getName(){ return _name; }
	virtual void	init()=0;

	virtual void		set_track_finished(){}
	virtual void        set_level(float right, float left){ emit sig_level(right, left); }
	virtual void        set_spectrum(QList<float>& lst ){ emit sig_spectrum(lst); }
	virtual void		update_bitrate(qint32 bitrate){}
	virtual void		update_time(qint32 time){}
	virtual bool		get_show_level(){ return false; }
	virtual bool		get_show_spectrum(){ return false; }


	virtual void		do_jump_play(){}
	virtual void		unmute(){}


signals:
	void sig_dur_changed_ms(quint64);
	void sig_dur_changed_s(quint32);
	void sig_dur_changed(MetaData&);
	void sig_pos_changed_ms(quint64);
	void sig_pos_changed_s(quint32);

	void sig_track_finished();
	void sig_scrobble(const MetaData&);
	void sig_level(float, float);
    void sig_spectrum(QList<float>&);
	void sig_bitrate_changed(qint32);

private slots:
	virtual void sr_initialized(bool b){ if(b) play(); }
	virtual void sr_ended(){}
	virtual void sr_not_valid(){ emit sig_track_finished(); }
	virtual void set_about_to_finish(qint64 ms){}
	virtual void set_cur_position_ms(qint64 ms){emit sig_pos_changed_ms(ms);}



public slots:
	virtual void play()=0;
	virtual void stop()=0;
	virtual void pause()=0;
	virtual void set_volume(int vol)=0;

	virtual void jump_abs_s(quint32 where)=0;
	virtual void jump_abs_ms(quint64 where)=0;
	virtual void jump_rel(quint32 where)=0;

	virtual void change_track(const MetaData&, int pos_sec=0, bool start_play=true)=0;
	virtual void change_track(const QString&, int pos_sec=0, bool start_play=true )=0;

	virtual void eq_changed(int band, int value){ Q_UNUSED(band); Q_UNUSED(value); }
	virtual void eq_enable(bool b){ Q_UNUSED(b); }
	virtual void record_button_toggled(bool b){ _sr_wanna_record = b; }

	virtual void psl_sr_set_active(bool b ){ _sr_active = b; }
	virtual void psl_new_stream_session(){}
	virtual void psl_calc_level(bool b){}
	virtual void psl_calc_spectrum(bool b){}
	virtual void psl_set_gapless(bool b){ _gapless = b; }

	virtual void start_convert(){}
	virtual void end_convert(){}
	virtual void psl_set_speed(float f){ Q_UNUSED(f); }

};

extern Engine* gst_obj_ref;

Q_DECLARE_INTERFACE(Engine, "sayonara.engine/1.0")

#endif

