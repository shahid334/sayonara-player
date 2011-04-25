#ifndef CLIBRARYBASE_H
#define CLIBRARYBASE_H

#include <QObject>
#include "library/CDirectoryReader.h"
#include "library/CDatabaseConnector.h"

class CLibraryBase : public QObject
{
    Q_OBJECT
public:
    explicit CLibraryBase(QObject *parent = 0);

    void loadDataFromDb ();

signals:
	void playlistCreated(QStringList&);
        void signalMetaDataLoaded (vector<MetaData>& in);

public slots:
    void baseDirSelected (const QString & baseDir);
    void insertMetaDataIntoDB(vector<MetaData>& in);

private:
    CDirectoryReader m_reader;
    CDatabaseConnector m_database;


};

#endif // CLIBRARYBASE_H
