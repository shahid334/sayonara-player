#ifndef STREAMPLAYLIST_H
#define STREAMPLAYLIST_H

#include "playlist/StdPlaylist.h"

//class StreamPlaylist : public Playlist
class StreamPlaylist : public StdPlaylist
{
    Q_OBJECT
public:
    explicit StreamPlaylist(QObject *parent);

   /* virtual void play();
    virtual void pause();
    virtual void stop();
    virtual void fwd();
    virtual void bwd();
    virtual void next();
    virtual void change_track(int idx);
    virtual void create_playlist(const MetaDataList& lst, bool start_playing);
    virtual void create_playlist(const QStringList& lst, bool start_playing);
    virtual void clear();
    virtual void metadata_changed(const MetaDataList& md_list);

    virtual void save_for_reload();
    virtual void save_to_m3u_file(QString filepath, bool relative);
    virtual bool request_playlist_for_collection(MetaDataList& lst);
    
signals:
    
public slots:*/
    
};

#endif // STREAMPLAYLIST_H
