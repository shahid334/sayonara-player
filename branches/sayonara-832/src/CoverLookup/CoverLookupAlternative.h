#ifndef COVERLOOKUPALTERNATIVE_H
#define COVERLOOKUPALTERNATIVE_H

#include "CoverLookup/CoverLookup.h"

class CoverLookupAlternative : public CoverLookupInterface
{

    Q_OBJECT

private:

    CoverLookupAlternative(QObject* parent, int n_covers);

    CoverLookup*        _cl;
    QString             _album_name;
    QString             _artist_name;
    QStringList         _artists_name;
    int                 _album_id;
    Album               _album;
    Artist              _artist;

    int					_n_covers;
    bool				_run;


    enum SearchType {

        ST_Standard=0,
        ST_Sampler=1,
        ST_ByID=2,
        ST_ByAlbum=3,
        ST_ByArtistName = 4,
        ST_ByArtist = 5
    } _search_type;


public:

    CoverLookupAlternative(QObject* parent, const QString& album_name, const QString& artist_name, int n_covers);
    CoverLookupAlternative(QObject* parent, const QString& album_name, const QStringList& artists_name, int n_covers);
    CoverLookupAlternative(QObject* parent, const Album& album, int n_covers);
    CoverLookupAlternative(QObject* parent, int album_id, int n_covers);
    CoverLookupAlternative(QObject* parent, const QString& artist, int n_covers);
    CoverLookupAlternative(QObject* parent, const Artist& artist, int n_covers);

    ~CoverLookupAlternative();

    virtual void stop();
    void start();


private slots:
    void cover_found(QString);
    void finished(bool);
};


#endif // COVERLOOKUPALTERNATIVE_H
