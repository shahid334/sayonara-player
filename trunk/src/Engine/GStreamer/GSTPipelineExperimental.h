#ifndef GSTPIPELINEEXPERIMENTAL_H
#define GSTPIPELINEEXPERIMENTAL_H


#include <QObject>
#include "GSTPipeline.h"

class GSTPipelineExperimental : public GSTPipeline
{
    Q_OBJECT


public:
    GSTPipelineExperimental(QObject* parent=0);
    virtual ~GSTPipelineExperimental();

private:
    GstElement* _audio_src;
    GstElement* _decoder;
    GstElement* _audio_convert;

    virtual bool set_uri(gchar* uri);


};

#endif // GSTPIPELINEEXPERIMENTAL_H
