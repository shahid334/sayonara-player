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
#include <fft.h>

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
/*
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
*/





using namespace std;

MP3_Listen::MP3_Listen(QObject * parent) : QObject (parent){

	_state = STATE_STOP;
	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

	/*_f = new float[512];
	_m = new int[10];
	_img = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U, 1);
	cvNamedWindow("win");

	_ado = new Phonon::AudioDataOutput();
	_ado->setDataSize(512);*/


	qDebug() << "   1/4 phonon init output";
	_audio_output = new Phonon::AudioOutput(Phonon::MusicCategory, this);

	qDebug() << "   2/4 phonon init media object";
	_media_object = new Phonon::MediaObject(this);
	_media_object->setTickInterval(10);

	qDebug() << "   3/4 phonon create path";
	_audio_path = Phonon::createPath(_media_object, _audio_output);
	//_audio_path_ado = Phonon::createPath(_media_object, _ado);
	_eq_type = EQ_TYPE_NONE;


	qDebug() << "   4/4 phonon connections";
	connect(_media_object, SIGNAL(tick(qint64)), this, SLOT(timeChanged(qint64)) );
	connect(_media_object, SIGNAL(finished()), this, SLOT(finished()));
//	connect(_ado, SIGNAL(dataReady(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16>> &)), this, SLOT(handle_data(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > & ) ));


}

MP3_Listen::~MP3_Listen() {
	delete _audio_output;
	delete _media_object;
}

void MP3_Listen::play(){

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

    _audio_output->setVolume(vol/100);
}


qreal MP3_Listen::getVolume(){
    return _audio_output->volume();
}


void MP3_Listen::eq_changed(int band, int val){
	if(_eq == 0 || _eq_type == EQ_TYPE_NONE || _effect_parameters.size() == 0) return;

	if(_effect_parameters.size() < 10){
		band = band / (( 10 / _effect_parameters.size()) + 1);
	}

	double new_val = 0;
	if(_eq_type == EQ_TYPE_10B){ // -24 - +12
		if(val > 0){
			new_val = val * 0.33;
		}

		else new_val = val * 0.66;
	}

	else if(_eq_type == EQ_TYPE_KEQ){
		new_val = val * 0.33;
		if(_effect_parameters.size() > 10)
			band += _effect_parameters.size() - 10;
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


void MP3_Listen::load_equalizer(){


	QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
	QStringList availableEqualizers;


	int equalizerIdx = -1;
	for(int i=0; i<availableEffects.size(); i++){
		Phonon::EffectDescription desc =  availableEffects[i];

		if(desc.name() == "KEqualizer"  && equalizerIdx == -1){
			equalizerIdx = i;
			_eq_type = EQ_TYPE_KEQ;
			availableEqualizers.push_back(desc.name());
		}

		else if(desc.name() == "equalizer-10bands" && equalizerIdx == -1){
			equalizerIdx = i;
			_eq_type = EQ_TYPE_10B;
			availableEqualizers.push_back(desc.name());
		}
	}

	if(equalizerIdx != -1){

		_eq = new Phonon::Effect(availableEffects[equalizerIdx], this);

		_effect_parameters = _eq->parameters();
		_is_eq_enabled = true;

		_audio_path.insertEffect(_eq);

		CSettingsStorage * set = CSettingsStorage::getInstance();
		vector<EQ_Setting> vec;
		set->getEqualizerSettings(vec);

		emit eq_found(availableEqualizers);
		emit eq_presets_loaded(vec);
	}


	else {
		qDebug() << "Equalizer effect not available";
		_eq = 0;
		_is_eq_enabled = false;
	}
}



void MP3_Listen::handle_data(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > & data){
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
