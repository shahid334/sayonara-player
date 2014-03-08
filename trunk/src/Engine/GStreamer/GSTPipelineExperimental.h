#ifndef GSTPIPELINEEXPERIMENTAL_H
#define GSTPIPELINEEXPERIMENTAL_H


#include <QObject>
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

private:
	GstElement* _audio_src;
    GstElement* _audio_convert;

    virtual bool set_uri(gchar* uri);

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
