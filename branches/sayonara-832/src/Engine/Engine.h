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
#include "HelperStructs/Equalizer_presets.h"
#include <QObject>
#include <QStringList>

#include <vector>

using namespace std;


enum EngineState {
	StatePlay=0,
	StatePause=1,
	StateStop=2
};


class Engine : public QObject{

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

public:
	virtual EngineState	getState() { return _state; }
	virtual QString	getName(){ return _name; }
	virtual void	init()=0;


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
	virtual void sr_initialized(bool)=0;
	virtual void sr_ended()=0;
	virtual void sr_not_valid()=0;


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

	virtual void eq_changed(int, int)=0;
	virtual void eq_enable(bool)=0;
	virtual void record_button_toggled(bool)=0;

    virtual void psl_sr_set_active(bool)=0;
    virtual void psl_new_stream_session()=0;
    virtual void psl_calc_level(bool)=0;
	virtual void psl_set_gapless(bool)=0;


};

Q_DECLARE_INTERFACE(Engine, "sayonara.engine/1.0")

#endif

