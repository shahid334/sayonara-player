
#ifndef LN_NOTIFICATION_H_
#define LN_NOTIFICATION_H_

#include <QString>
#include "Notification/Notification.h"

class LN_Notification : public Notification {

	Q_OBJECT
	Q_INTERFACES(Notification)

public:
	LN_Notification();
	virtual ~LN_Notification();

	virtual void notification_show(QString title, QString text);
	virtual QString get_name();
};


#endif



