#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/globals.h"
#include "DatabaseAccess/CDatabaseConnector.h"
//#include "playlist/PlaylistNew.h"

#include <QString>
#include <QList>




class Playlist : public QObject
{

    Q_OBJECT

signals:
    void sig_playlist_changed(const MetaDataList&, int);
    void sig_track_changed(const MetaData&, int);
    void sig_stopped();



protected:
    bool            _playlist_changed;
    int             _cur_play_idx;
    MetaDataList    _v_md;
    Playlist_Mode	_playlist_mode;
    PlaylistType    _playlist_type;
    bool            _reports_disabled;
    bool            _start_playing;

    void report_changes(bool pl_changed, bool track_changed);
    void enable_reports();
    void disable_reports();

public:
    Playlist(QObject* parent);

    virtual void play()=0;
    virtual void pause()=0;
    virtual void stop()=0;
    virtual void fwd()=0;
    virtual void bwd()=0;
    virtual void next()=0;
    virtual void change_track(int idx)=0;

    virtual void create_playlist(const MetaDataList& lst, bool start_playing=true)=0;
    virtual void create_playlist(const QStringList& lst, bool start_playing=true)=0;
    virtual void clear();

    virtual void metadata_changed(const MetaDataList& md_list)=0;

    virtual void move_track(const int, int tgt);
    virtual void move_tracks(const QList<int>& lst, int tgt);

    virtual void delete_track(const int idx);
    virtual void delete_tracks(const QList<int>& lst);

    virtual void insert_track(const MetaData& md, int tgt);
    virtual void insert_tracks(const MetaDataList& lst, int tgt);

    virtual void append_track(const MetaData& md);
    virtual void append_tracks(const MetaDataList& lst);

    virtual void replace_track(int idx, const MetaData& md);
    virtual void save_for_reload()=0;
    virtual void save_to_m3u_file(QString filepath, bool relative)=0;
    virtual bool request_playlist_for_collection(MetaDataList& lst)=0;

    virtual void selection_changed(const QList<int>&);

    int find_track_by_id(int id);
    int find_track_by_path(QString path);

    PlaylistType get_type();
    int get_cur_track();
    QStringList toStringList();

    void set_playlist_mode(Playlist_Mode mode);
};


#endif // PLAYLIST_H
