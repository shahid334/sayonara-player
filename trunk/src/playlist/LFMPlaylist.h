#ifndef LFMPLAYLIST_H
#define LFMPLAYLIST_H

#include "playlist/Playlist.h"
#include "HelperStructs/MetaData.h"
#include "StreamPlugins/LastFM/LastFM.h"


class LFMPlaylist : public Playlist
{
    Q_OBJECT

signals:
    void sig_need_more_radio();

public:
    LFMPlaylist(QObject* parent);

    virtual void play();
    virtual void pause();
    virtual void stop();
    virtual void fwd();
    virtual void bwd();
    virtual void next();
    virtual void change_track(int idx);

    virtual void create_playlist(const MetaDataList& lst, bool start_playing);
    virtual void create_playlist(const QStringList& lst, bool start_playing);
    virtual void append_tracks(const MetaDataList& lst);
    virtual void insert_tracks(const MetaDataList& lst, int tgt);
    virtual void metadata_changed(const MetaDataList &md_list);
    virtual void clear();

    virtual void save_for_reload();
    virtual void save_to_m3u_file(QString filepath, bool relative);
    virtual bool request_playlist_for_collection(MetaDataList& lst);

public slots:
    void new_radio_playlist(const MetaDataList&);


private:
    MetaDataList _v_md_hidden;
    LastFM* _lfm;


    
};

#endif // LFMPLAYLIST_H
