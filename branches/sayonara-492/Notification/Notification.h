#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_
#include <QObject>
#include <QString>


class Notification : public QObject {

Q_OBJECT

protected:

	bool	_initialized;
	QString _name;

public:

	virtual void notification_show(QString title, QString text)=0;
	virtual QString get_name()=0;

};
	
Q_DECLARE_INTERFACE(Notification, "sayonara.notification/1.0");


#endif


