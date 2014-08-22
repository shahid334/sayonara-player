/* LFMTrackChangedThread.h

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Jul 18, 2012 
 *
 */

#ifndef LFMTRACKCHANGEDTHREAD_H_
#define LFMTRACKCHANGEDTHREAD_H_

#include <QThread>
#include <QList>
#include <QMap>
#include <QString>
#include "StreamPlugins/LastFM/LFMGlobals.h"

#include "HelperStructs/MetaData.h"

#define LFM_THREAD_TASK_UPDATE_TRACK 		1<<0
#define LFM_THREAD_TASK_SIM_ARTISTS 		1<<1
#define LFM_THREAD_TASK_FETCH_TRACK_INFO 	1<<2
#define LFM_THREAD_TASK_FETCH_ARTIST_INFO 	1<<3
#define LFM_THREAD_TASK_FETCH_ALBUM_INFO 	1<<4
#define LFM_THREAD_TASK_FETCH_USER_INFO 	1<<5

enum Quality{
    Quality_Poor = 0,
    Quality_Well = 1,
    Quality_Very_Good = 2
};

struct ArtistMatch{

    static const int Quality_Poor = 0;
    static const int Quality_Well = 1;
    static const int Quality_Very_Good = 2;

    QMap<QString, double> very_good;
    QMap<QString, double> well;
    QMap<QString, double> poor;

    QString artist;

    bool operator ==(ArtistMatch am) {
        return (artist == am.artist);
    }

    void add(QString artist, double match) {
        if(match > 0.15) very_good[artist] = match;
        else if(match > 0.05) well[artist] = match;
        else poor[artist] = match;
    }

    QMap<QString, double> get(Quality q) {
        switch(q) {
            case Quality_Poor:
                    return poor;
            case Quality_Well:
                return well;
            case Quality_Very_Good:
                return very_good;
            default:
                return very_good;
        }

        return very_good;
    }


};

class LFMTrackChangedThread : public QThread{

	Q_OBJECT

	signals:
		void sig_corrected_data_available(const QString&);
		void sig_album_info_available(const QString&);
		void sig_artist_info_available(const QString&);
		void sig_user_info_available(const QString&);
		void sig_similar_artists_available(const QString&, const QList<int>&);



protected:
		void run();

public:
	LFMTrackChangedThread(QString target_class);
	virtual ~LFMTrackChangedThread();

	/*
	 *	required by
	 *	required LFM_THREAD_TASK_UPDATE_TRACK
	 *	required LFM_THREAD_TASK_SIM_ARTISTS
	 *	required LFM_THREAD_TASK_FETCH_TRACK_INFO
	 */
	void setTrackInfo(const MetaData& md);

	/*
	 *	required by
	 *	required LFM_THREAD_TASK_UPDATE_TRACK
	*/
	void setSessionKey(QString session_key);

	/*
	 *	required by
	 *	required LFM_THREAD_TASK_UPDATE_TRACK
	 *	required LFM_THREAD_TASK_FETCH_TRACK_INFO
	 *	required LFM_THREAD_TASK_FETCH_ALBUM_INFO
	 *	required LFM_THREAD_TASK_FETCH_ARTIST_INFO
	 */
	void setUsername(QString username);

	/*
	 * required by LFM_THREAD_TASK_ALBUM_INFO
	 * required by LFM_THREAD_TASK_ARTIST_INFO
	 */
	void setArtistName(QString artist);

	/*
	 * required by LFM_THREAD_TASK_ALBUM_INFO
	 */
	void setAlbumName(QString album);


	/*
	 * Ready to fetch function
	 */
	bool fetch_corrections(MetaData& md, bool& loved, bool& corrected);
	bool fetch_album_info(QMap<QString, QString>& info);
	bool fetch_artist_info(QMap<QString, QString>& info);

	void setThreadTask(int task);
	void setTargetClass(QString name);


private:

	QString		_target_class;
	int			_thread_tasks;

	MetaData 	_md;

	/* update track */
	QString		_username;
	QString 	_session_key;

	bool update_now_playing();


	/* similar artists */
	QList<int>	_chosen_ids;

	bool search_similar_artists();
    QMap<QString, int> filter_available_artists(QMap<QString, double>& artists);


	/* Track info */
	MetaData	_md_corrected;
	bool		_loved;
	bool		_corrected;
	bool		_corrected_success;


	QString		_album_name;
	QString		_artist_name;
	QMap<QString, QString>		_album_data;
	QMap<QString, QString>		_artist_data;
	QMap<QString, QString>		_user_data;
    QMap<QString, ArtistMatch >   _sim_artists_cache;

	bool 		get_corrected_track_info(MetaData& md, bool& loved, bool& corrected);
	bool 		get_artist_info(QString artist);
	bool	 	get_album_info(QString artist, QString album);

};

#endif /* LFMTRACKCHANGEDTHREAD_H_ */
