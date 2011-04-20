#ifndef CLIBRARYBASE_H
#define CLIBRARYBASE_H

#include <QObject>
#include "library/CDirectoryReader.h"

class CLibraryBase : public QObject
{
    Q_OBJECT
public:
    explicit CLibraryBase(QObject *parent = 0);

signals:
	void playlistCreated(QStringList&);

public slots:
    void baseDirSelected (const QString & baseDir);

private:
    CDirectoryReader m_reader;


};

#endif // CLIBRARYBASE_H
