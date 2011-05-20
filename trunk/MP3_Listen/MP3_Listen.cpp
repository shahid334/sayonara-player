    /*
 * MP3_Listen.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: luke
 */

#include "MP3_Listen/MP3_Listen.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/id3.h"
#include "HelperStructs/CSettingsStorage.h"


#include <iostream>
#include <vector>

#include <QtGui>
#include <QtCore>
#include <QPointer>
#include <QApplication>
#include <QString>
#include <QMap>
#include <QMapIterator>

#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <phonon/effect.h>
#include <phonon/effectparameter.h>
#include <phonon/path.h>







using namespace std;

MP3_Listen::MP3_Listen(QObject * parent) : QObject (parent){

	_state = STATE_STOP;
	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

	qDebug() << "   phonon init output";
	_audio_output = new Phonon::AudioOutput(Phonon::MusicCategory, this);

	qDebug() << "   phonon init media object";
	_media_object = new Phonon::MediaObject(this);
	_media_object->setTickInterval(10);

	qDebug() << "   phonon create path";

	_audio_path = Phonon::createPath(_media_object, _audio_output);
	_eq_type = EQ_TYPE_NONE;

	QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();

	qDebug() << "Available Effects:";
	int equalizerIdx = -1;
	for(int i=0; i<availableEffects.size(); i++){
		Phonon::EffectDescription desc =  availableEffects[i];
		qDebug() << desc.name();

		if(desc.name() == "KEqualizer" && equalizerIdx == -1){
			equalizerIdx = i;
			_eq_type = EQ_TYPE_KEQ;
		}

		else if(desc.name() == "equalizer-10bands" && equalizerIdx == -1){
			equalizerIdx = i;
			_eq_type = EQ_TYPE_10B;
		}
	}

	if(equalizerIdx != -1){
		qDebug() << "Equalizer found (" << availableEffects[equalizerIdx] << ")";
		_eq = new Phonon::Effect(availableEffects[equalizerIdx], this);

		_effect_parameters = _eq->parameters();
		foreach(Phonon::EffectParameter param, _effect_parameters){
			qDebug() << param.name() << ": " << param.minimumValue() << ", " << param.maximumValue();
		//	qDebug() << param.description();
		}
		_is_eq_enabled = true;

		_audio_path.insertEffect(_eq);


	}

	else {
		qDebug() << "Equalizer effect not available";
		_eq = 0;
		_is_eq_enabled = false;
	}




	qDebug() << "   phonon connections";
	connect(_media_object, SIGNAL(tick(qint64)), this, SLOT(timeChanged(qint64)) );
	connect(_media_object, SIGNAL(finished()), this, SLOT(finished()));



}

MP3_Listen::~MP3_Listen() {
	delete _audio_output;
	delete _media_object;
}

void MP3_Listen::play(){

	//cout << "play " << endl;
	//if(_eq != 0) _audio_path.removeEffect(_eq);
	QList<Phonon::EffectDescription > devices = Phonon::BackendCapabilities::availableAudioEffects();

	//qDebug() << "Devices " << devices.size();
/*	for(int i=0; i<devices.size(); i++){
		qDebug() << "Device " << i << ": " << devices[i].name();
	}*/




	_media_object->play();


	_state = STATE_PLAY;
}

void MP3_Listen::stop(){

	_media_object->stop();
	_state= STATE_STOP;


}

void MP3_Listen::pause(){

	_media_object->pause();
	_state = STATE_PAUSE;
}


void MP3_Listen::jump(int where, bool percent){

	quint64 newtime_ms = where;
	if(percent) newtime_ms = _meta_data.length_ms * where / 100.0;

	_media_object->seek(newtime_ms);
	_seconds_started = newtime_ms / 1000;
	emit timeChangedSignal((quint32) (newtime_ms / 1000));

}



void MP3_Listen::changeTrack(const QString & filepath){

	
	_media_object->setCurrentSource( Phonon::MediaSource(filepath) );
	_meta_data = ID3::getMetaDataOfFile(filepath);

	//_media_object->play();
	_state = STATE_PLAY;
	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

	play();

}


void MP3_Listen::changeTrack(const MetaData & metadata){

	_media_object->setCurrentSource( Phonon::MediaSource(metadata.filepath) );
	_meta_data = ID3::getMetaDataOfFile( metadata.filepath );

	//_media_object->play();
	//_state = STATE_PLAY;


	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

	play();

}


const MetaData & MP3_Listen::getMetaData() const {
	return _meta_data;

}


void MP3_Listen::seekableChanged(bool){

}


void MP3_Listen::timeChanged(qint64 time){

	_mseconds_now = time;
	_seconds_now = time / 1000;

	// scrobble after 25 sec or if half of the track is reached
	if( !_scrobbled && (_seconds_now - _seconds_started == 25 || _seconds_now - _seconds_started == _meta_data.length_ms / 2000)){
		emit scrobble_track(_meta_data);
		_scrobbled = true;
	}

	emit timeChangedSignal((quint32) (time / 1000));

}

void MP3_Listen::finished(){

	emit track_finished();
}


void MP3_Listen::setVolume(qreal vol){
   // qDebug() << "Set Volume " << vol;
    _audio_output->setVolume(vol/100);
}


qreal MP3_Listen::getVolume(){
    return _audio_output->volume();
}


void MP3_Listen::eq_changed(int band, int val){
	if(_eq == 0 || _eq_type == EQ_TYPE_NONE) return;

	qint64 tmp_seconds = _mseconds_now;

	//_audio_path.removeEffect(_eq);

	double new_val = 0;

	if(_eq_type == EQ_TYPE_10B){
		if(val >= 0) new_val = val / 2.0;
		else new_val = val;
	}

	else if(_eq_type == EQ_TYPE_KEQ){
		new_val = val / 4.0 - 6.0;

	}

	_eq->setParameterValue(_effect_parameters[band], new_val);




}


void MP3_Listen::eq_enable(bool enable){

	if(_eq == 0 || _eq_type == EQ_TYPE_NONE) return;
	if(!enable)
		_audio_path.removeEffect(_eq);

	else
		_audio_path.insertEffect(_eq);

	_is_eq_enabled = enable;

}


void MP3_Listen::find_presets(){

	CSettingsStorage * set = CSettingsStorage::getInstance();
	vector<EQ_Setting> vec;
	set->getEqualizerSettings(vec);
	emit eq_presets_loaded(vec);

}
