#ifndef GSTPIPELINEEXPERIMENTAL_H
#define GSTPIPELINEEXPERIMENTAL_H


#include <QObject>
#include <QTimer>
#include "GSTPipeline.h"

//#define ENGINE_OLD_PIPELINE



class GSTPipelineExperimental : public GSTPipeline
{
    Q_OBJECT


public:
    GSTPipelineExperimental(QObject* parent=0);
    virtual ~GSTPipelineExperimental();



};

#endif // GSTPIPELINEEXPERIMENTAL_H
