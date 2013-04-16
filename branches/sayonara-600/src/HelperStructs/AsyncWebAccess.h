#ifndef WEBACCESS_H_
#define WEBACCESS_H_

#include <QThread>
#include <QString>

class AsyncWebAccess : public QThread {

	Q_OBJECT

public:
	AsyncWebAccess(QObject* parent);
	virtual ~AsyncWebAccess();

	bool get_data(QString& data);
	void set_url(QString url);

protected:
	void run();

private:
	QString _url;
	QString _data;
	



};


#endif
