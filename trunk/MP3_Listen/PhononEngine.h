/* MP3_Listen.h */

/* Copyright (C) 2011  Lucio Carreras
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


/*
 * MP3_Listen.h
 *
 *  Created on: Mar 2, 2011
 *      Author: luke
 */

#ifndef MP3_LISTEN_H_
#define MP3_LISTEN_H_

#define STATE_STOP 0
#define STATE_PLAY 1
#define STATE_PAUSE 2

#define EQ_TYPE_NONE -1
#define EQ_TYPE_KEQ 0
#define EQ_TYPE_10B 1



#include "HelperStructs/MetaData.h"
#include "HelperStructs/Equalizer_presets.h"
#include "MP3_Listen/Engine.h"

#include <string>
#include <vector>

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>

#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <phonon/effectparameter.h>
#include <phonon/audiodataoutput.h>
#include <phonon/videowidget.h>



class Phonon_Engine : public Engine{

	Q_OBJECT


public:

	Phonon_Engine();
	virtual ~Phonon_Engine();

private:

	Phonon::AudioOutput *_audio_output;
	Phonon::MediaObject *_media_object;
	Phonon::Path		_audio_path;
	Phonon::Effect*		_eq;
	QList<Phonon::EffectParameter> _effect_parameters;
	Phonon::VideoWidget* _video_widget;


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


	private slots:
		void seekableChanged(bool b);
        void timeChanged(qint64 time);
        void finished();
        void handle_data(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > & );
        void total_time_changed(qint64);

		void stateChanged(Phonon::State, Phonon::State);
		void currentSourceChanged(const Phonon::MediaSource&);
		void metaDataChanged();


	public:
		virtual void 		load_equalizer();
		qreal 				getVolume();


};

#endif /* MP3_LISTEN_H_ */
