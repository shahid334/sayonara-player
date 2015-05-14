/* SettingKey.h */

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



#ifndef SETTINGKEY_H
#define SETTINGKEY_H

#include <QString>
#include <QPoint>
#include <QSize>
#include "Library/Sorting.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/PlaylistMode.h"



namespace SK {
	enum SettingKey {

		LFM_Active,
		LFM_Login,
		LFM_Corrections,
		LFM_ShowErrors,
		LFM_SessionKey,

		Eq_Last,
        Eq_List,
        Eq_Gauss,

		Lib_Show,
		Lib_Path,
		Lib_ColsTitle,
		Lib_ColsArtist,
		Lib_ColsAlbum,
		Lib_OnlyTracks,
		Lib_LiveSearch,
		Lib_Sorting,

		Player_Version,
		Player_Language,
		Player_Style,
		Player_Size,
		Player_Pos,
		Player_Fullscreen,
		Player_Maximized,
		Player_ShownPlugin,
		Player_OneInstance,
		Player_Min2Tray,
		Player_NotifyNewVersion,

		PL_Playlist,
		PL_Load,
		PL_LoadSavedPlaylists,
		PL_LoadLastTrack,
		PL_RememberTime,
		PL_StartPlaying,
		PL_LastTrack,
		PL_LastPlaylist,
		PL_Mode,
		PL_ShowNumbers,
		PL_SmallItems,

		Notification_Show,
		Notification_Timeout,
		Notification_Name,
		Notification_Scale,

		Engine_Name,
		Engine_Vol,
		Engine_ConvertQuality,
		Engine_CovertTargetPath,
		Engine_Gapless,
		Engine_ShowSpectrum,
		Engine_ShowLevel,
        Engine_CurTrackPos_s,

		Engine_SR_Active,
		Engine_SR_Warning,
		Engine_SR_Path,
		Engine_SR_SessionPath,

		Socket_Active,
		Socket_From,
		Socket_To,

		Spectrum_Style,
		Level_Style,

		BroadCast_Active,
		Broadcast_Prompt,
		Broadcast_Port,
        Broadcast_Clients,

		MP3enc_found,
		Player_Quit,

		Num_Setting_Keys
	};
}

template<typename T, SK::SettingKey S>
class SettingKey
{
	public:
		SettingKey(){}
};


#define INST(type, settingkey) typedef SettingKey<type, SK:: settingkey> settingkey##_t; const settingkey##_t settingkey



namespace Set {
	INST(bool,					LFM_Active);					/* is lastFM active? */
	INST(QStringList,			LFM_Login);						/* 2-Tupel, username, password */

	INST(bool,					LFM_Corrections);				/* propose lfm corrections */
	INST(bool,					LFM_ShowErrors);				/* get error message, if there are lfm problems */
	INST(QString,				LFM_SessionKey);				/* lfm session key */

	INST(int,					Eq_Last);						/* last equalizer index */
	INST(QList<EQ_Setting>,		Eq_List);						/* All equalizers */
	INST(bool,					Eq_Gauss);						/* do curve, when changing eq setting */

	INST(bool,					Lib_Show);						/* show library */
	INST(QString,				Lib_Path);						/* library path */
	INST(QList<int>,			Lib_ColsTitle);					/* shown columns tracks */
	INST(QList<int>,			Lib_ColsArtist);				/* shown columns artist */
	INST(QList<int>,			Lib_ColsAlbum);					/* shown columns albums */
	INST(bool,					Lib_OnlyTracks);				/* show only tracks in library */
	INST(bool,					Lib_LiveSearch);				/* library live search */
	INST(LibSortOrder,			Lib_Sorting);					/* how to sort in lib */

	INST(QString,				Player_Version);				/* Version string of player */
	INST(QString,				Player_Language);				/* language of player */
	INST(int,					Player_Style);					/* dark or native: native = 0, dark = 1 */
	INST(QSize,					Player_Size);					/* player size */
	INST(QPoint,				Player_Pos);					/* player position */
	INST(bool,					Player_Fullscreen);				/* player fullscreen */
	INST(bool,					Player_Maximized);				/* player maximized */
	INST(QString,				Player_ShownPlugin);			/* current shown plugin in player, empty if none */
	INST(bool,					Player_OneInstance);			/* only one Sayonara instance is allowed */
	INST(bool,					Player_Min2Tray);				/* minimize Sayonara to tray */
	INST(bool,					Player_NotifyNewVersion);		/* check for new version on startup */

	INST(QStringList,			PL_Playlist);					/* old playlist: list of integers in case of library tracks, if no library track, filepath */
	INST(bool,					PL_Load);						/* load old playlist on startup */
	INST(bool,					PL_LoadSavedPlaylists);			/* load saved playlists on startup */
	INST(bool,					PL_LoadLastTrack);				/* load last track on startup */
	INST(bool,					PL_RememberTime);				/* remember time of last track */
	INST(bool,					PL_StartPlaying);				/* start playing immediatly when opening Sayonara */
	INST(int,					PL_LastTrack);					/* last track idx in playlist */
	INST(int,					PL_LastPlaylist);				/* last Playlist id, where LastTrack has been played */

	INST(PlaylistMode,			PL_Mode);						/* playlist mode: rep1, repAll, shuffle... */
	INST(bool,					PL_ShowNumbers);				/* show numbers in playlist */
	INST(bool,					PL_SmallItems);					/* show small items in playlist */

	INST(bool,					Notification_Show);				/* show notifications */
	INST(int,					Notification_Timeout);			/* notification timeout */
	INST(QString,				Notification_Name);				/* type of notifications: libnotify or empty for native baloons :( */
	INST(int,					Notification_Scale);			/* scaling of notification item (pixels) */

	INST(QString,				Engine_Name);					/* Deprecated: Engine name */
	INST(int,					Engine_Vol);					/* Volume */
	INST(int,					Engine_CurTrackPos_s);			/* position of track (used to load old position) */
	INST(int,					Engine_ConvertQuality);			/* Convert quality, 1-10 for variable, > 64 for fixed bitrate */
	INST(QString,				Engine_CovertTargetPath);		/* last convert path */
	INST(bool,					Engine_Gapless);				/* gapless playback */
	INST(bool,					Engine_ShowSpectrum);			/* show spectrum */
	INST(bool,					Engine_ShowLevel);				/* show level */


	INST(bool,					Engine_SR_Active);				/* Streamripper active */
	INST(bool,					Engine_SR_Warning);				/* streamripper warnings */
	INST(QString,				Engine_SR_Path);				/* streamripper paths */
	INST(bool,					Engine_SR_SessionPath);			/* streamripper session path */

	INST(bool,					Socket_Active);					/* remote control active */
	INST(int,					Socket_From);					/* Deprecated: First port that should be tried */
	INST(int,					Socket_To);						/* Deprecated: Last port that should be tried */
	INST(int,					Spectrum_Style);				/* index of spectrum style */
	INST(int,					Level_Style);					/* index of level style */
	INST(bool,					BroadCast_Active);				/* is broadcast active? */
	INST(bool,					Broadcast_Prompt);				/* prompt when new connection arrives? */
	INST(int,					Broadcast_Port);				/* broadcast port */

}

namespace SetNoDB{

	INST(int,					Broadcast_Clients);
	INST(bool,					MP3enc_found);
	INST(bool,					Player_Quit);

}

#endif // SETTINGKEY_H
