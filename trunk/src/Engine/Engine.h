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
#include "HelperStructs/SayonaraClass.h"
#include <QObject>
#include <QStringList>

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


class Engine : public QObject, protected SayonaraClass {

	Q_OBJECT

protected:

	MetaData	_md;
	qint64		_cur_pos_ms;

	bool		_scrobbled;
	qint64		_scrobble_begin_ms;

	int			_eq_type;

	bool 		_playing_stream;
	bool		_sr_wanna_record;

	QString		_name;
	bool		_show_level;
	bool		_show_spectrum;
	bool		_broadcast_active;


public:

	Engine(QObject* parent=0) :
		QObject(parent),
		SayonaraClass()
	{
		REGISTER_LISTENER(Set::Engine_ShowLevel, _sl_show_level_changed);
		REGISTER_LISTENER(Set::Engine_ShowSpectrum, _sl_show_spectrum_changed);
		REGISTER_LISTENER(Set::BroadCast_Active, _sl_broadcast_active_changed);
	}

    virtual QString	getName(){return _name;}
	virtual void	init()=0;

	virtual void		set_track_finished(){}
	virtual void        set_level(float right, float left){ emit sig_level(right, left); }
	virtual void        set_spectrum(QList<float>& lst ){ emit sig_spectrum(lst); }

	virtual void		async_done(){}
	virtual void		update_bitrate(quint32 bitrate){}
	virtual void		update_duration(){}
	virtual void		update_time(qint32 time){}

	virtual void		unmute(){}
	virtual bool		get_show_level(){ return _show_level; }
	virtual bool		get_show_spectrum(){ return _show_spectrum; }
	virtual bool		get_broadcast_active() { return _broadcast_active; }

	virtual void		set_track_ready(){}


signals:
	void sig_md_changed(const MetaData&);

	void sig_pos_changed_ms(quint64);
	void sig_pos_changed_s(quint32);

	void sig_track_finished();
	void sig_scrobble(const MetaData&);
	void sig_level(float, float);
    void sig_spectrum(QList<float>&);
    void sig_download_progress(int);
	void sig_data(uchar*, quint64);


private slots:
	virtual void sr_initialized(bool b){ if(b) play(); }
	virtual void sr_ended(){}
	virtual void sr_not_valid(){ emit sig_track_finished(); }
	virtual void set_about_to_finish(qint64 ms){}
    virtual void set_cur_position_ms(qint64 ms){
        _cur_pos_ms = ms;
		emit sig_pos_changed_ms(ms);
	}


	virtual void _sl_show_level_changed(){
		_show_level = _settings->get(Set::Engine_ShowLevel);
	}

	virtual void _sl_show_spectrum_changed(){
		_show_spectrum = _settings->get(Set::Engine_ShowSpectrum);
	}

	virtual void _sl_broadcast_active_changed(){
		_broadcast_active = _settings->get(Set::BroadCast_Active);
	}

protected slots:
	virtual void new_data(uchar* data, quint64 size){
		emit sig_data(data, size);
	}


public slots:
	virtual void play()=0;
	virtual void stop()=0;
	virtual void pause()=0;

	virtual void jump_abs_ms(quint64 where)=0;
	virtual void jump_rel(double where)=0;

	virtual void change_track(const MetaData&)=0;
	virtual void change_track(const QString&)=0;

	virtual void eq_changed(int band, int value){ Q_UNUSED(band); Q_UNUSED(value); }
	virtual void record_button_toggled(bool b){ _sr_wanna_record = b; }

	virtual void psl_new_stream_session(){}

	virtual void start_convert(){}
	virtual void end_convert(){}
	virtual void psl_set_speed(float f){ Q_UNUSED(f); }

};

extern Engine* gst_obj_ref;

Q_DECLARE_INTERFACE(Engine, "sayonara.engine/1.0")

#endif

