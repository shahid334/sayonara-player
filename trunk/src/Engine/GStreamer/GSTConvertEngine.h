/* GSTConvertEngine.h */

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



#ifndef GSTCONVERTENGINE_H
#define GSTCONVERTENGINE_H

#include "Engine/Engine.h"
#include "Engine/GStreamer/GSTConvertPipeline.h"


class GSTConvertEngine : public Engine
{
	Q_OBJECT

public:
	explicit GSTConvertEngine(QObject *parent = 0);
	

	// public from Gstreamer Callbacks
		void		set_track_finished();

		virtual void init();


	private:

		GSTConvertPipeline*	 _pipeline;
		MetaData _md_target;

		// methods
		bool set_uri(const MetaData& md);


	private slots:

		void set_cur_position_ms(qint64);


	public slots:
		virtual void play();
		virtual void stop();
		virtual void pause();
		virtual void set_volume(int vol);


		virtual void jump_abs_ms(quint64);
		virtual void jump_rel(double);


		virtual void change_track(const MetaData&);
		virtual void change_track(const QString&);
};

#endif // GSTCONVERTENGINE_H


