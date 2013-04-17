#ifndef IMPORT_COPY_THREAD_H
#define IMPORT_COPY_THREAD_H

#include <QThread>
#include <QString>
#include <QMap>
#include <QString>
#include <QStringList>

#include "HelperStructs/MetaData.h"

#define IMPORT_COPY_THREAD_COPY 0
#define IMPORT_COPY_THREAD_ROLLBACK 1

class ImportCopyThread : public QThread
{
    Q_OBJECT

public:

    explicit ImportCopyThread(QObject *parent = 0);
    void set_vars(QString chosen_dir, QString lib_dir, QStringList& files, QMap<QString, MetaData>& map);
    int get_n_files();
    int get_copied_files();
    void get_metadata(MetaDataList& v_md);
    void set_cancelled();
    bool get_cancelled();
    void set_mode(int mode);
    int  get_mode();


protected:
    QString        _lib_dir;
    QString        _chosen_dir;
    QStringList    _files;
    QMap<QString, MetaData> _map;
    MetaDataList   _v_md;
    QStringList    _lst_copied_files;
    int            _n_files;
    int            _copied_files;
    int            _mode;
    int            _percent;
    bool           _cancelled;

    
    void run();
    virtual void copy()=0;
    virtual void rollback()=0;

signals:
    void sig_progress(int);


};


#endif
