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

#include "HelperStructs/MetaData.h"

#include <string>
#include <vector>

#include <QObject>
#include <QString>

#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>





class MP3_Listen : public QObject{

	Q_OBJECT

private:
	int				_state;
	std::string		_cur_MP3_file;
    MetaData		_meta_data;
    int				_seconds_started;
    int				_seconds_now;
    bool			_scrobbled;

	Phonon::AudioOutput *_audio_output;
	Phonon::MediaObject *_media_object;
	Phonon::MediaSource *_media_source;




public:
	MP3_Listen(QObject * parent);
	virtual ~MP3_Listen();

	const MetaData & getMetaData() const;
	//int64_t getTime();

	int getState(){ return _state; }

	qreal 	getVolume();

	signals:
		void metaDataChangedSignal(const MetaData&);
		void metaDataForPlaylistReady(vector<MetaData>&);
		void timeChangedSignal(quint32);
		void scrobble_track(const MetaData &);
		void track_finished();




	public slots:
		void play();
		void stop();
		void pause();
        void setVolume(qreal vol);

		/**
		* TODO: Where in what? Percent, seconds or egss?
		*/
		void jump(int where);
		void changeTrack(const MetaData& );
		void changeTrack(const QString& );


	private slots:
		void seekableChanged(bool b);
        void timeChanged(qint64 time);
        void finished();

};

#endif /* MP3_LISTEN_H_ */
