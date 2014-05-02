#ifndef GSTCONVERTENGINE_H
#define GSTCONVERTENGINE_H

#include "Engine/Engine.h"
#include "Engine/GStreamer/GSTConvertPipeline.h"
#include "HelperStructs/CSettingsStorage.h"


class GSTConvertEngine : public Engine
{
	Q_OBJECT

public:
	explicit GSTConvertEngine(QObject *parent = 0);
	

	// public from Gstreamer Callbacks
		void		set_track_finished();

		virtual void init();


	private:

		CSettingsStorage* _settings;
		GSTConvertPipeline*	 _pipeline;

		// methods
		bool set_uri(const MetaData& md, bool* start_play);


	private slots:

		void set_cur_position_ms(qint64);


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
};

#endif // GSTCONVERTENGINE_H


