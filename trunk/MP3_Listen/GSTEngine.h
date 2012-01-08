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

#include <gst/gst.h>

#include <QObject>
#include <QDebug>




class GST_Engine : public QObject {

	Q_OBJECT

public:

	GST_Engine(QObject * parent);
	virtual ~GST_Engine();

	bool		init();
	void		state_changed();
	void		set_cur_position(quint32);
	int 		getState();


private:
	GstElement* _pipeline;
	GstElement* _volume;
	GstBus* 	_bus;


	MetaData	_meta_data;
	int			_seconds_started;
	int			_seconds_now;
	qint64		_mseconds_now;
	bool		_scrobbled;

	bool		_is_eq_enabled;

	int			_eq_type;
	int			_state;


signals:
	void total_time_changed_signal(qint64);
	void timeChangedSignal(quint32);



public slots:

	void play();
	void stop();
	void pause();
    void setVolume(qreal vol);

	/**
	* TODO: Where in what? Percent, seconds or egss?
	*/
	void jump(int where, bool percent=true);
	void changeTrack(const MetaData& );
	void changeTrack(const QString& );
	void eq_changed(int, int);
	void eq_enable(bool);

};

#endif /* GSTENGINE_H_ */
