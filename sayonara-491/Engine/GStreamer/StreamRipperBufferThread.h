#ifndef STREAMRIPPERBUFFERTHREAD_H
#define STREAMRIPPERBUFFERTHREAD_H

#include <QThread>
#include <QString>
#include <QFile>

class StreamRipperBufferThread : public QThread
{
    Q_OBJECT
public:
    StreamRipperBufferThread(QObject *parent = 0);
    virtual ~StreamRipperBufferThread();
    
protected:
    void run();

public:
    void setUri(QString uri);
    QString getUri();
    qint64 getSize();

public slots:
    void terminate();



private:

    QString _uri;
    qint64 _size;
    QFile* _f;


    
};

#endif // STREAMRIPPERBUFFERTHREAD_H
