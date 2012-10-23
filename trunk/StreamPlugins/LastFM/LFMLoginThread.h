#ifndef LFMLOGINTHREAD_H
#define LFMLOGINTHREAD_H

#include <QThread>

#include <QString>
#include "HelperStructs/globals.h"

struct LFMLoginStuff{
    QString authToken;
    QString sessionKey;
    QString sessionKey2;
    bool logged_in;
};

class LFMLoginThread : public QThread
{
    Q_OBJECT

public:
    LFMLoginThread(QObject *parent = 0);
    virtual ~LFMLoginThread();
    
protected:
    void run();

public:
    void setup_login_thread(QString username, QString password);
    LFMLoginStuff getLoginStuff();



private:
    QString _username;
    QString _password;

    LFMLoginStuff _login_info;

};



#endif // LFMLOGINTHREAD_H
