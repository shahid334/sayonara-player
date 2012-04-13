/*
 * GSTEngine.h
 *
 *  Created on: Jan 7, 2012
 *      Author: luke
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
#include "MP3_Listen/Engine.h"

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
	GstPad*		_audio_pad;
	GstBus*		_bus;


public slots:
	virtual void play();
	virtual void stop();
	virtual void pause();
	virtual void setVolume(qreal vol);

	virtual void jump(int where, bool percent=true);
	virtual void changeTrack(const MetaData& );
	virtual void changeTrack(const QString& );
	virtual void eq_changed(int, int);
	virtual void eq_enable(bool);


public:
	// callback -> class
	void		state_changed();
	void		set_cur_position(quint32);
	void		set_track_finished();

	virtual void 	load_equalizer();
	virtual int		getState();
	virtual QString	getName();




};

#endif /* GSTENGINE_H_ */
