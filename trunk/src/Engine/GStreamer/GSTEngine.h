/* GSTEngine.h */

/* Copyright (C) 2012  Lucio Carreras
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
#include "HelperStructs/CSettingsStorage.h"
#include "Engine/GStreamer/GSTPipeline.h"
#include "Engine/GStreamer/StreamRecorder.h"
#include "Engine/Engine.h"

#include <gst/gst.h>
#include <gst/gstbuffer.h>

#include <QObject>
#include <QDebug>


#include <vector>

using namespace std;


enum CapsType {
    CapsTypeUnknown=0,
    CapsTypeInt=1,
    CapsTypeFloat=2
};

class MyCaps {

private:
    CapsType     _type;
    bool    _sig;
    int     _width;
    int     _channels;
    bool    _parsed;

public:

    MyCaps(){
        _type = CapsTypeUnknown;
        _sig = false;
        _width = -1;
        _channels = -1;
        _parsed = false;
    }

    bool is_parsed(){ return _parsed; }
    void set_parsed(bool b){ _parsed = b; }

    CapsType get_type(){ return _type; }
    bool get_signed() {return _sig; }
    int get_width() { return _width; }
    int get_channels() { return _channels; }

    void parse(GstCaps* caps){
        QString info = gst_caps_to_string(caps);
        //qDebug() << info;

        QStringList lst = info.split(",");
        foreach(QString s, lst){

            s = s.trimmed();
            if(s.startsWith("audio", Qt::CaseInsensitive)){
                if(s.contains("int", Qt::CaseInsensitive)) _type = CapsTypeInt;
                else if(s.contains("float", Qt::CaseInsensitive)) _type = CapsTypeFloat;
                else _type = CapsTypeUnknown;
            }

            else if(s.startsWith("signed", Qt::CaseInsensitive)){
                if(s.contains("true", Qt::CaseInsensitive)) _sig = true;
                else _sig = false;
            }

            else if(s.startsWith("width", Qt::CaseInsensitive)){
                _width = s.right(2).toInt();
            }

            else if(s.startsWith("channels", Qt::CaseInsensitive)){
                _channels = s.right(1).toInt();
                if(_channels > 2) _channels = 2;
            }
        }
        _parsed = true;
    }
};





class GST_Engine : public Engine {

	Q_OBJECT
	Q_INTERFACES(Engine)


public:

	GST_Engine();
	virtual ~GST_Engine();

	virtual void init();


private:
	
    GSTPipeline*    _pipeline;
	StreamRecorder* _stream_recorder;

	LastTrack*  _last_track;
    MyCaps*      _caps;


    bool        _show_level;
    bool        _show_spectrum;
    int         _jump_play;


private slots:
    virtual void sr_initialized(bool);
    virtual void sr_ended();
    virtual void sr_not_valid();
    void timeout();
    


public slots:
    virtual void play();
	virtual void stop();
	virtual void pause();
	virtual void setVolume(int vol);

	virtual void jump(int where, bool percent=true);
    virtual void changeTrack(const MetaData&, int pos_sec=0, bool start_play=true);
    virtual void changeTrack(const QString&, int pos_sec=0, bool start_play=true );

	virtual void eq_changed(int, int);
	virtual void eq_enable(bool);
    virtual void psl_new_stream_session();

 	virtual void record_button_toggled(bool);

    virtual void psl_sr_set_active(bool);
    virtual void psl_calc_level(bool);
    virtual void psl_calc_spectrum(bool);



public:
	// callback -> class
	void		state_changed();
	void		set_cur_position(quint32);
	void		set_track_finished();
    void        set_about_to_finish();
    void        emit_buffer(float inv_array_elements, float scale);

    void        set_level(float right, float left);
    void        set_spectrum(QList<float>&);

	virtual void 	load_equalizer(vector<EQ_Setting>&);
	virtual int		getState();
	virtual QString	getName();

    bool get_show_level();
    bool get_show_spectrum();
    MyCaps* get_caps();
    void do_jump_play();
    void unmute();


private:
	CSettingsStorage* _settings;

	void init_play_pipeline();
    bool set_uri(const MetaData& md, bool* start_play);

};

extern GST_Engine* gst_obj_ref;



#endif /* GSTENGINE_H_ */
