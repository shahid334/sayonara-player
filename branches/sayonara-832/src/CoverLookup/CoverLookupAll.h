#ifndef COVERLOOKUPALL_H
#define COVERLOOKUPALL_H

#include "CoverLookup/CoverLookup.h"

class CoverLookupAll : public CoverLookupInterface
{
    Q_OBJECT

private:
    CoverLookup* _cl;
    AlbumList _album_list;
    bool _run;

private slots:
    void cover_found(QString);
    void finished(bool);

public:

    CoverLookupAll(QObject* parent, const AlbumList& album_list);
    ~CoverLookupAll();

    virtual void stop();
    void start();

protected:
    void run();
};

#endif // COVERLOOKUPALL_H
