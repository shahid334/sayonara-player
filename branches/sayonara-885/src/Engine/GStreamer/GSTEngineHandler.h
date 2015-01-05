/* GSTEngineHandler.h */

/* Copyright (C) 2014  Lucio Carreras
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

#ifndef ENGINEHANDLER_H_
#define ENGINEHANDLER_H_

#include "Engine/Engine.h"

#include <QVector>

class GSTEngineHandler : public Engine
{
	Q_OBJECT
	Q_INTERFACES(Engine)

public:
	explicit GSTEngineHandler(QObject* parent=0);
	virtual ~GSTEngineHandler();

	void fill_engines(const QVector<Engine*>& engines);
	void init();


public slots:

	void play();
	void stop();
	void pause();

	void jump_abs_s(quint32 where);
	void jump_abs_ms(quint64 where);
	void jump_rel(quint32 where);
    void jump_rel_ms(qint64 where);

	void change_track(const MetaData&);
	void change_track(const QString&);

	void eq_changed(int band, int value);
	void record_button_toggled(bool);

	void psl_new_stream_session();

    void psl_change_engine(QString name);

	void start_convert();
	void end_convert();

	void psl_set_speed(float f);


private slots:

	void sl_dur_changed_ms(quint64);
	void sl_dur_changed_s(quint32);
	void sl_dur_changed(const MetaData&);
	void sl_pos_changed_ms(quint64);
	void sl_pos_changed_s(quint32);

	void sl_track_finished();
	void sl_scrobble(const MetaData&);
	void sl_level(float, float);
	void sl_spectrum(QList<float>&);
	void sl_bitrate_changed(qint32);


private:

	Engine*          _cur_engine;
	QVector<Engine*>   _engines;

	bool configure_connections(Engine* old_engine, Engine* new_engine);


};

#endif


