/* Playlist.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "HelperStructs/Helper.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/SayonaraClass.h"
#include "HelperStructs/globals.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QString>
#include <QList>


class Playlist : public QObject, protected SayonaraClass
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
	MetaDataList	_selected_tracks;
    PlaylistMode	_playlist_mode;
	PlaylistMode	_playlist_mode_backup;
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

	virtual void metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new)=0;

    virtual void move_track(const int, int tgt);
    virtual void move_tracks(const QList<int>& lst, int tgt);

    virtual void delete_track(const int idx);
    virtual void delete_tracks(const QList<int>& lst);

    virtual void insert_track(const MetaData& md, int tgt);
    virtual void insert_tracks(const MetaDataList& lst, int tgt);

    virtual void append_track(const MetaData& md);
    virtual void append_tracks(const MetaDataList& lst);

    virtual void replace_track(int idx, const MetaData& md);
    virtual void save_to_m3u_file(QString filepath, bool relative)=0;
	virtual const MetaDataList& get_playlist(){return _v_md;}

    virtual void selection_changed(const QList<int>&);
	virtual const MetaDataList& get_selected_tracks(){return _selected_tracks;}

	virtual PlaylistMode playlist_mode_backup();
	virtual PlaylistMode playlist_mode_restore();

    bool is_empty();
    PlaylistType get_type();
    int get_cur_track();
    QStringList toStringList();

	QList<int> find_tracks(int id);
	QList<int> find_tracks(const QString& filepath);

	void set_playlist_mode(const PlaylistMode& mode);
};


#endif // PLAYLIST_H
