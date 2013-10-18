#ifndef GSTPIPELINE_H
#define GSTPIPELINE_H

#include <QObject>
#include <QString>
#include <gst/gst.h>
#include <gst/gstbuffer.h>

bool _test_and_error(void* element, QString errorstr);
bool _test_and_error_bool(bool b, QString errorstr);


class GSTPipeline : public QObject
{
    Q_OBJECT
public:
    GSTPipeline(QObject *parent = 0);
    ~GSTPipeline();
    GstElement* get_pipeline();
    GstBus* get_bus();

    void play();
    void pause();
    void stop();
    gint64 seek_rel(float percent, gint64 ref_ns);
    gint64 seek_abs(gint64 ns );
    void set_volume(int vol);

    void enable_level(bool b);
    void enable_spectrum(bool b);

    gint64 get_duration_ns();
    guint get_bitrate();
    bool set_uri(gchar* uri);
    void set_eq_band(QString band_name, double val);


private:

    GstBus*		_bus;
    GstElement* _pipeline;
    GstElement* _equalizer;
    GstElement* _eq_queue;
    GstElement* _volume;

    GstPad* _tee_app_pad;
    GstPad* _app_pad;

    GstElement* _audio_sink;
    GstElement* _audio_bin;

    GstPadTemplate* _tee_src_pad_template;

    GstElement* _level_audio_convert, *_spectrum_audio_convert;
    GstElement* _level, *_spectrum;
    GstPad*     _level_pad, *_spectrum_pad;
    GstPad*     _tee_level_pad, *_tee_spectrum_pad;

    GstElement* _level_sink, *_spectrum_sink;
    GstElement* _level_queue, *_spectrum_queue;


    GstElement* _tee;

    
};

#endif // GSTPIPELINE_H
