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

class Engine : public QObject{

	Q_OBJECT

protected:
	MetaData	_meta_data;
    MetaData    _md_gapless;
	int			_seconds_started;
	int			_seconds_now;
    int         _vol;
	qint64		_mseconds_now;

	bool		_scrobbled;
	bool		_is_eq_enabled;
	int			_eq_type;
	int			_state;
	QString		_name;
	bool		_track_finished;

	bool 		_playing_stream;
    bool        _gapless_track_available;

	bool		_sr_active;

	bool		_sr_wanna_record;


public:
	virtual void 	load_equalizer(vector<EQ_Setting>&)=0;
	virtual int		getState()=0;
	virtual QString	getName()=0;
	virtual void	init()=0;

signals:
	void total_time_changed_signal(qint64);
	void timeChangedSignal(quint32);
	void track_finished();
	void scrobble_track(const MetaData&);
	void eq_presets_loaded(const vector<EQ_Setting>&);
	void eq_found(const QStringList&);
	void sig_valid_strrec_track(const MetaData&);
    void wanna_gapless_track();

public slots:
    virtual void play(int pos_sec=0)=0;
	virtual void stop()=0;
	virtual void pause()=0;
	virtual void setVolume(int vol)=0;

	virtual void jump(int where, bool percent=true)=0;
    virtual void changeTrack(const MetaData&, int pos_sec=0, bool start_play=true)=0;
    virtual void changeTrack(const QString&, int pos_sec=0, bool start_play=true )=0;
    virtual void psl_gapless_track(const MetaData&)=0;
	virtual void eq_changed(int, int)=0;
	virtual void eq_enable(bool)=0;
	virtual void record_button_toggled(bool)=0;

    virtual void psl_sr_set_active(bool)=0;
    virtual void psl_new_stream_session()=0;

private slots:
    virtual void sr_initialized(bool)=0;
    virtual void sr_ended()=0;
    virtual void sr_not_valid()=0;




};

Q_DECLARE_INTERFACE(Engine, "sayonara.engine/1.0");



#endif
