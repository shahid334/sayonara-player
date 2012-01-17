#ifndef ENGINE_H_
#define ENGINE_H_

#include "HelperStructs/MetaData.h"
#include "HelperStructs/Equalizer_presets.h"
#include <QObject>
#include <QStringList>

#include <vector>

using namespace std;

class Engine : public QObject{

	Q_OBJECT

protected:
	MetaData	_meta_data;
	int			_seconds_started;
	int			_seconds_now;
	qint64		_mseconds_now;
	bool		_scrobbled;
	bool		_is_eq_enabled;
	int			_eq_type;
	int			_state;

public:
	virtual void 	load_equalizer()=0;

	int getState(){
		return _state;
	}

signals:
	void total_time_changed_signal(qint64);
	void timeChangedSignal(quint32);
	void track_finished();
	void scrobble_track(const MetaData&);
	void eq_presets_loaded(const vector<EQ_Setting>&);
	void eq_found(const QStringList&);

public slots:

	virtual void play()=0;
	virtual void stop()=0;
	virtual void pause()=0;
	virtual void setVolume(qreal vol)=0;

	virtual void jump(int where, bool percent=true)=0;
	virtual void changeTrack(const MetaData& )=0;
	virtual void changeTrack(const QString& )=0;
	virtual void eq_changed(int, int)=0;
	virtual void eq_enable(bool)=0;

};

#endif
