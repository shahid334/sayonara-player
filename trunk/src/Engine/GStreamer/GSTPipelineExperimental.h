#ifndef GSTPIPELINEEXPERIMENTAL_H
#define GSTPIPELINEEXPERIMENTAL_H


#include <QObject>
#include <QTimer>
#include "GSTPipeline.h"

//#define ENGINE_OLD_PIPELINE

enum GSTFileMode{
	GSTFileModeFile,
	GSTFileModeHttp

};


class GSTPipelineExperimental : public GSTPipeline
{
    Q_OBJECT


public:
    GSTPipelineExperimental(QObject* parent=0);
    virtual ~GSTPipelineExperimental();

	virtual bool set_uri(gchar* uri);
	virtual bool set_next_uri(gchar* uri);
	void start_timer(qint64 play_ms);
	void is_timer_running();

private:
	GstElement* _audio_src;
	GstElement* _audio_src_tmp;
    GstElement* _audio_convert;

	QTimer* _timer;

private slots:
	void start_play();



#ifdef ENGINE_OLD_PIPELINE

	GstElement* _audio_src_file;
	GstElement* _audio_src_http;

	GstElement* _decoder;
	GSTFileMode _filemode;

	bool check_and_switch_filemode(gchar* uri);
	GstElement* get_filemode_src(GSTFileMode mode);
#endif



};

#endif // GSTPIPELINEEXPERIMENTAL_H
