
#ifndef LN_NOTIFICATION_H_
#define LN_NOTIFICATION_H_

#include <QString>
#include "Notification/Notification.h"
#include "HelperStructs/MetaData.h"



class LN_Notification : public Notification {

	Q_OBJECT
	Q_INTERFACES(Notification)

private:
    void* _not;

public:
	LN_Notification();
	virtual ~LN_Notification();

    virtual void notification_show(const MetaData& md);
    virtual void not_close();
	virtual QString get_name();
};


#endif



