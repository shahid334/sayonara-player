/* LFMPlaylist.h */

/* Copyright (C) 2013  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef LFMPLAYLIST_H
#define LFMPLAYLIST_H

#include "playlist/Playlist.h"
#include "HelperStructs/MetaData.h"
#include "StreamPlugins/LastFM/LastFM.h"

#include <QThread>

class NewLFMPlaylistThread : public QThread {

    Q_OBJECT

signals:
    void sig_finished(const MetaDataList&);

private slots:
    void playlist_fetched(const MetaDataList& v_md){
        emit sig_finished(v_md);
    }

public:
    NewLFMPlaylistThread(LastFM* lfm, QObject* parent=0 ) : QThread(parent){
        _lfm = lfm;
    }

    ~NewLFMPlaylistThread(){ this->terminate();}


protected:
    void run(){

        _lfm->psl_radio_playlist_request();

    }


private:
    LastFM* _lfm;

};


class LFMPlaylist : public Playlist
{
    Q_OBJECT

signals:
    void sig_need_more_radio();

public:
    LFMPlaylist(QObject* parent);
    ~LFMPlaylist();

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
    NewLFMPlaylistThread* _thread;


    
};

#endif // LFMPLAYLIST_H
