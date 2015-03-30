/* PlayManager.h */

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



#ifndef PLAY_MANAGER_H
#define PLAY_MANAGER_H

#include <QObject>
#include "HelperStructs/globals.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/SayonaraClass.h"


class PlayManager : public QObject, protected SayonaraClass
{

	Q_OBJECT

	SINGLETON_QOBJECT(PlayManager)

	public:
	enum PlayState {
		PlayState_Playing=0,
		PlayState_Paused,
		PlayState_Stopped,
		PlayState_Wait
	};


	signals:
		void sig_playstate_changed(PlayManager::PlayState);
		void sig_next();
		void sig_previous();
		void sig_stopped();
		void sig_seeked_rel(double);
		void sig_seeked_abs_ms(quint64 ms);
		void sig_position_changed_ms(quint64 ms);
		void sig_track_changed(const MetaData& v_md);
		void sig_track_idx_changed(int idx);
		void sig_playlist_changed(int len);



	public slots:
		void wait();
		void play();
		void play_pause();
		void pause();
		void previous();
		void next();
		void stop();
		void seek_rel(double percent);
		void seek_abs_ms(quint64 ms);
		void set_position_ms(quint64 ms);
		void change_track(const MetaData& md);
		void change_track_idx(int idx);
		void playlist_changed(const MetaDataList& v_md);


	public:
		PlayState get_play_state();


	private:
		PlayState	_playstate;
		int			_cur_idx;
		int			_playlist_len;
		qint64		_start_position;

		bool		playlist_dead();

};

#endif

