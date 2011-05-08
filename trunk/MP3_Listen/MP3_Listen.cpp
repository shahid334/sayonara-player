    /*
 * MP3_Listen.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: luke
 */

#include "MP3_Listen/MP3_Listen.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/id3.h"


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
	Phonon::createPath(_media_object, _audio_output);

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


void MP3_Listen::jump(int where){

	quint64 newtime_ms = _meta_data.length_ms * where / 100.0;
	_media_object->seek(newtime_ms);
	_seconds_started = newtime_ms / 1000;
	emit timeChangedSignal((quint32) (newtime_ms / 1000));

}



void MP3_Listen::changeTrack(const QString & filepath){

	_media_source = new Phonon::MediaSource(filepath);
	_media_object->setCurrentSource( (*_media_source) );
	_meta_data = ID3::getMetaDataOfFile(filepath);

	_media_object->play();
	_state = STATE_PLAY;
	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

}


void MP3_Listen::changeTrack(const MetaData & metadata){

	_media_source = new Phonon::MediaSource(metadata.filepath);
	_media_object->setCurrentSource( (*_media_source) );
	_meta_data = ID3::getMetaDataOfFile(metadata.filepath);

	_media_object->play();
	_state = STATE_PLAY;

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

}


const MetaData & MP3_Listen::getMetaData() const {
	return _meta_data;

}


void MP3_Listen::seekableChanged(bool){

}


void MP3_Listen::timeChanged(qint64 time){

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


