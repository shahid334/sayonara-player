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


/*
#include <opencv/cv.h>
#include <opencv/highgui.h>
*/


class MP3_Listen : public QObject{

	Q_OBJECT

private:
	int				_state;
	std::string		_cur_MP3_file;
    MetaData		_meta_data;
    int				_seconds_started;
    int				_seconds_now;
    qint64			_mseconds_now;
    bool			_scrobbled;
   /* float*			_f;
    int*			_m;

    IplImage* _img;*/



	Phonon::AudioOutput *_audio_output;
	Phonon::MediaObject *_media_object;
	Phonon::Path		_audio_path;
	//Phonon::Path		_audio_path_ado;
	Phonon::Effect*		_eq;
	QList<Phonon::EffectParameter> _effect_parameters;
	//Phonon::AudioDataOutput*	_ado;
	bool				_is_eq_enabled;

	int					_eq_type;


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
		void eq_presets_loaded(const vector<EQ_Setting>&);
		void eq_found(const QStringList&);




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


	private slots:
		void seekableChanged(bool b);
        void timeChanged(qint64 time);
        void finished();
        void handle_data(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > & );


	public:
        void load_equalizer();

};

#endif /* MP3_LISTEN_H_ */
