#ifndef IMPORTFOLDERTHREAD_H
#define IMPORTFOLDERTHREAD_H

#include <QThread>
#include <QMap>
#include <QString>
#include <QStringList>
#include "HelperStructs/MetaData.h"

#define IMPORT_MODE_EXTRACT 0
#define IMPORT_MODE_COPY 1

class ImportFolderThread : public QThread
{
    Q_OBJECT

public:
    explicit ImportFolderThread(QObject *parent = 0);
    void run();

    void set_src_dir(QString dir);
    void get_filelist(QStringList& lst);
    void set_may_terminate(bool);
    void get_md_map(QMap<QString, MetaData>& map);
    int get_n_tracks();


    
signals:
    void sig_done();
    void sig_progress(int);

public slots:

public:


private:
    QString                 _src_dir;
    QStringList             _filelist;
    bool                    _may_terminate;
    QMap<QString, MetaData> _map;





    
};

#endif // IMPORTFOLDERTHREAD_H
