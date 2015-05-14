/* GSTPipeline.h */

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



#ifndef GSTPIPELINE_H
#define GSTPIPELINE_H

#include <QString>
#include <QTimer>
#include <gst/gst.h>
#include <gst/gstbuffer.h>

#include "Engine/GStreamer/GSTEngineHelper.h"
#include "HelperStructs/SayonaraClass.h"


enum GSTFileMode{
	GSTFileModeFile,
	GSTFileModeHttp
};


bool
_test_and_error(void* element, QString errorstr);

bool
_test_and_error_bool(bool b, QString errorstr);


class GSTAbstractPipeline : public QObject, protected SayonaraClass {

	Q_OBJECT

	private:
		bool		_about_to_finish;
		bool		_initialized;
		Engine*		_engine;

	protected:

		QString		_name;

		GstBus*		_bus;
		GstElement* _pipeline;

		gchar*		_uri;

        qint64		_duration_ms;
        qint64		_position_ms;

		virtual bool create_element(GstElement** elem, const gchar* elem_name, const gchar* name="");
		virtual bool create_elements()=0;
		virtual bool add_and_link_elements()=0;
		virtual bool configure_elements()=0;



	signals:
		void sig_finished();
		void sig_about_to_finish(qint64);
		void sig_pos_changed_ms(qint64);
		void sig_data(uchar*, quint64);


	public slots:
		virtual void play()=0;
		virtual void pause()=0;
		virtual void stop()=0;

        virtual qint64 get_duration_ms() final;
        virtual qint64 get_position_ms() final;
		virtual void set_speed(float f);


	public:
		GSTAbstractPipeline(QString name, Engine* engine, QObject* parent=0);
		virtual ~GSTAbstractPipeline();

		virtual bool		init(GstState state=GST_STATE_READY);
		virtual GstElement* get_pipeline();
		virtual GstBus*		get_bus();
		virtual GstState	get_state();
		virtual void		refresh_position();
		virtual void		refresh_duration();
		virtual void		finished();
		virtual void		check_about_to_finish();
		virtual void		set_data(uchar* data, quint64 size);

		virtual bool		set_uri(gchar* uri);
		virtual gchar*		get_uri();
};


namespace PipelineCallbacks {

	void pad_added_handler(GstElement *src, GstPad *new_pad, gpointer data);
	gboolean show_position(gpointer data);
	GstFlowReturn new_buffer(GstElement *sink, gpointer data);

}

#endif // GSTPIPELINE_H
