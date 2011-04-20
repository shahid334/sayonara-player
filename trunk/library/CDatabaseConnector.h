#ifndef CDATABASECONNECTOR_H
#define CDATABASECONNECTOR_H

#include <QObject>
#include <QSqlDatabase>

class CDatabaseConnector : public QObject
{
    Q_OBJECT
public:
    explicit CDatabaseConnector(QObject *parent = 0);
    virtual ~CDatabaseConnector();

    /**
      * Check if db of m_databaseFileContainer is existent
      * @return true if we can load the db false if not
      */
    bool isExistent ();


    bool createDB();
    bool openDatabase ();



signals:

public slots:

private:
    const QString m_createScriptFileName;
    const QString m_databaseContainerFile;
    QSqlDatabase m_database;



};

#endif // CDATABASECONNECTOR_H
