/* Phonon_Engine.cpp */

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
 * Phonon_Engine.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: luke
 */

#include "MP3_Listen/PhononEngine.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/id3.h"
#include "HelperStructs/CSettingsStorage.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <QtGui>
#include <QtCore>
#include <QPointer>
#include <QApplication>
#include <QString>
#include <QMap>
#include <QMapIterator>
#include <QVector>

#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <phonon/effect.h>
#include <phonon/effectparameter.h>
#include <phonon/path.h>
#include <phonon/audiodataoutput.h>
#include <phonon/videowidget.h>


using namespace std;

Phonon_Engine::Phonon_Engine() {
	_state = STATE_STOP;
	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

	_name = "Phonon Backend";
	_audio_output = 0;
	_media_object = 0;
}

Phonon_Engine::~Phonon_Engine() {
	if(_audio_output)
			delete _audio_output;
	if(_media_object)
		delete _media_object;
}

void Phonon_Engine::init(){



	qDebug() << "   1/4 phonon init output";
	_audio_output = new Phonon::AudioOutput(Phonon::VideoCategory, this);

	qDebug() << "   2/4 phonon init media object";
	_media_object = new Phonon::MediaObject(this);
	_media_object->setTickInterval(10);

	qDebug() << "   3/4 phonon create path";
	_audio_path = Phonon::createPath(_media_object, _audio_output);

	_video_widget = new Phonon::VideoWidget();

	Phonon::createPath(_media_object, _video_widget);
	_eq_type = EQ_TYPE_NONE;

	qDebug() << "   4/4 phonon connections";
	connect(_media_object, SIGNAL(tick(qint64)), this,
			SLOT(timeChanged(qint64)));
	connect(_media_object, SIGNAL(finished()), this, SLOT(finished()));
	connect(_media_object, SIGNAL(totalTimeChanged ( qint64)), this,
			SLOT(total_time_changed(qint64)));
	connect(_media_object, SIGNAL(seekableChanged(bool)), this, SLOT(seekableChanged(bool)));
	
	// just for debugging	
	/*connect(_media_object, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this,
		SLOT(stateChanged(Phonon::State, Phonon::State)));
	connect(_media_object, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
	connect(_media_object, SIGNAL(currentSourceChanged(const Phonon::MediaSource&)),
		this, SLOT(currentSourceChanged(const Phonon::MediaSource)));*/

}

void Phonon_Engine::play() {
	_media_object->play();
	_state = STATE_PLAY;
}

void Phonon_Engine::stop() {

	_media_object->stop();
	_state = STATE_STOP;

}

void Phonon_Engine::pause() {

	_media_object->pause();
	_state = STATE_PAUSE;
}

void Phonon_Engine::jump(int where, bool percent) {

	quint64 newtime_ms = where;
	if (percent)
		newtime_ms = _meta_data.length_ms * where / 100.0;

	_media_object->seek(newtime_ms);
	_seconds_started = newtime_ms / 1000;
	emit timeChangedSignal((quint32) (newtime_ms / 1000));

}

void Phonon_Engine::changeTrack(const QString & filepath) {

	_media_object->setCurrentSource(Phonon::MediaSource(filepath));
	MetaData md = ID3::getMetaDataOfFile(filepath);
	if (_media_object->hasVideo())
		md.length_ms = _meta_data.length_ms;
	_meta_data = md;

	//_media_object->play();
	_state = STATE_PLAY;
	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

	if (!_media_object->hasVideo())
		play();
	else
		_video_widget->show();

}

void Phonon_Engine::changeTrack(const MetaData & metadata) {


	_media_object->setCurrentSource(Phonon::MediaSource(metadata.filepath));
	MetaData md = ID3::getMetaDataOfFile(metadata.filepath);
	if (_media_object->hasVideo())
		md.length_ms = _meta_data.length_ms;
	_meta_data = md;

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

	if (!_media_object->hasVideo())
		play();
	else
		_video_widget->show();

}

void Phonon_Engine::total_time_changed(qint64 total_time) {

	emit total_time_changed_signal(total_time);
	_meta_data.length_ms = total_time;
	play();
}

void Phonon_Engine::seekableChanged(bool seekable) {
	qDebug() << "seekable ? " << seekable;
}

void Phonon_Engine::timeChanged(qint64 time) {

	_mseconds_now = time;
	_seconds_now = time / 1000;

	// scrobble after 25 sec or if half of the track is reached
	if (!_scrobbled
			&& (_seconds_now - _seconds_started == 25
					|| _seconds_now - _seconds_started
					== _meta_data.length_ms / 2000)) {
		emit scrobble_track(_meta_data);
		_scrobbled = true;

	}

	emit timeChangedSignal((quint32) (time / 1000));

}

void Phonon_Engine::finished() {

	emit track_finished();
}

void Phonon_Engine::setVolume(qreal vol) {

	_audio_output->setVolume(vol / 100.0);
}

qreal Phonon_Engine::getVolume() {
	return _audio_output->volume();
}

void Phonon_Engine::eq_changed(int band, int val) {
	if (_eq == 0 || _eq_type == EQ_TYPE_NONE || _effect_parameters.size() == 0)
		return;

	if (_effect_parameters.size() < 10 && _effect_parameters.size() > 0) {
		band = band / ((10 / _effect_parameters.size()) + 1);
	}

	double new_val = 0;
	if (_eq_type == EQ_TYPE_10B) { // -24 - +12
		if (val > 0) {
			new_val = val * 0.33;
		}

		else
			new_val = val * 0.66;
	}

	else if (_eq_type == EQ_TYPE_KEQ) {
		new_val = val * 0.33;
		if (_effect_parameters.size() > 10)
			band += _effect_parameters.size() - 10;
	}

	_eq->setParameterValue(_effect_parameters[band], new_val);
}

void Phonon_Engine::eq_enable(bool enable) {

	if (_eq == 0 || _eq_type == EQ_TYPE_NONE
	) return;
	if (!enable)
		_audio_path.removeEffect(_eq);

	else
		_audio_path.insertEffect(_eq);

	_is_eq_enabled = enable;

}

void Phonon_Engine::load_equalizer() {

	QList<Phonon::EffectDescription> availableEffects =
			Phonon::BackendCapabilities::availableAudioEffects();
	QStringList availableEqualizers;

	int equalizerIdx = -1;
	for (int i = 0; i < availableEffects.size(); i++) {
		Phonon::EffectDescription desc = availableEffects[i];

		if (desc.name() == "KEqualizer" && equalizerIdx == -1) {
			equalizerIdx = i;
			_eq_type = EQ_TYPE_KEQ;
			availableEqualizers.push_back(desc.name());
		}

		else if (desc.name() == "equalizer-10bands" && equalizerIdx == -1) {
			equalizerIdx = i;
			_eq_type = EQ_TYPE_10B;
			availableEqualizers.push_back(desc.name());
		}
	}

	if (equalizerIdx != -1) {

		_eq = new Phonon::Effect(availableEffects[equalizerIdx], this);

		_effect_parameters = _eq->parameters();
		_is_eq_enabled = true;

		_audio_path.insertEffect(_eq);

		CSettingsStorage* settings = CSettingsStorage::getInstance();
		vector<EQ_Setting> vec;
		settings->getEqualizerSettings(vec);

		emit eq_presets_loaded(vec);
	}

	else {
		qDebug() << "Equalizer effect not available";
		_eq = 0;
		_is_eq_enabled = false;
	}
}

void Phonon_Engine::handle_data(
		const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > & data) {

	Q_UNUSED(data);

	/*
	 cvZero(_img);
	 QList<QVector<short int> >	 vals = data.values();
	 foreach(QVector<short int> vec, vals){
	 if(vec.size() == 512){
	 for(int i=0; i<512; i++)
	 _f[i] = vec[i];

	 fftr1(_f, 512, 1);

	 for(int i=0; i<10; i++){
	 _m[i] = 0;
	 for(int j=51*i; j<51*(i+1); j++){
	 _m[i] += _f[j];
	 }

	 _m[i] /= 5100;
	 //qDebug() << i << " " << abs(_m[i]);
	 cvRectangle(_img, cvPoint(i * 10, 100), cvPoint((i+1)*10, 100- (_m[i] + 1000) / 20), cvScalarAll(255), -1);
	 }
	 }

	 }

	 cvShowImage("win", _img);
	 cvWaitKey(0);

	 */
}





void Phonon_Engine::stateChanged(Phonon::State newstate, Phonon::State oldstate){

	QString newstate_str;
	QString oldstate_str;
	switch(newstate){
		case Phonon::LoadingState:
			newstate_str = "Loading State";  break;

			case Phonon::StoppedState:
			newstate_str = "Stopped State";  break;

			case Phonon::PlayingState:
			newstate_str = "Playing State";  break;

			case Phonon::BufferingState:
			newstate_str = "Buffering State";  break;

			case Phonon::PausedState:
			newstate_str = "Paused State";  break;

			case Phonon::ErrorState:
				if (_media_object->errorType() == Phonon::FatalError) {
                 QMessageBox::warning(NULL, tr("Fatal Error"), _media_object->errorString());
             } else {
                 QMessageBox::warning(NULL, tr("Error"),
                 _media_object->errorString());
             }
			newstate_str = "Error State";  break;
		default: break;
	}
	switch(oldstate){
			
		case Phonon::LoadingState:
			oldstate_str = "Loading State";  break;

			case Phonon::StoppedState:
			oldstate_str = "Stopped State";  break;

			case Phonon::PlayingState:
			oldstate_str = "Playing State";  break;

			case Phonon::BufferingState:
			oldstate_str = "Buffering State";  break;

			case Phonon::PausedState:
			oldstate_str = "Paused State";  break;

			case Phonon::ErrorState:
			oldstate_str = "Error State";  break;
		default: break;
	}

	qDebug() << "****State: " << oldstate_str << " -> " << newstate_str;
}
void Phonon_Engine::currentSourceChanged(const Phonon::MediaSource& source){
	qDebug() << "****SOURCE CHANGED" << source.fileName();
	
	
}

void Phonon_Engine::metaDataChanged(){

}

int Phonon_Engine::getState(){
	return _state;
}

QString Phonon_Engine::getName(){
	return _name;
}

Q_EXPORT_PLUGIN2(sayonara_phonon, Phonon_Engine);
