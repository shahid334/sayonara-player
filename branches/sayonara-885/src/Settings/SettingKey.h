#ifndef SETTINGKEY_H
#define SETTINGKEY_H

#include <QString>
#include <QPoint>
#include <QSize>
#include "library/Sorting.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/PlaylistMode.h"



namespace SK{
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
		PL_LoadLastTrack,
		PL_RememberTime,
		PL_StartPlaying,
		PL_LastTrack,
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
		Engine_SR_CompleteTracks,
		Engine_SR_SessionPath,

		Socket_Active,
		Socket_From,
		Socket_To,

		Spectrum_Style,

		BroadCast_Active,
		Broadcast_Prompt,
		Broadcast_Port,
        Broadcast_Clients,

		Num_Setting_Keys
	};
}

template<typename T, SK::SettingKey S>
class SettingKey
{};


#define INST(type, settingkey) typedef SettingKey<type, SK:: settingkey> settingkey##_t; const settingkey##_t settingkey



namespace Set {
	INST(bool, LFM_Active);
	INST(QStringList, LFM_Login);

	INST(bool, LFM_Corrections);
	INST(bool, LFM_ShowErrors);
	INST(QString, LFM_SessionKey);

	INST(int, Eq_Last);
    INST(QList<EQ_Setting>, Eq_List);
    INST(bool, Eq_Gauss);

	INST(bool, Lib_Show);
	INST(QString, Lib_Path);
	INST(QList<int>, Lib_ColsTitle);
	INST(QList<int>, Lib_ColsArtist);
	INST(QList<int>, Lib_ColsAlbum);
	INST(bool, Lib_OnlyTracks);
	INST(bool, Lib_LiveSearch);
    INST(LibSortOrder, Lib_Sorting);

	INST(QString, Player_Version);
	INST(QString, Player_Language);
	INST(int, Player_Style);
	INST(QSize, Player_Size);
	INST(QPoint, Player_Pos);
	INST(bool, Player_Fullscreen);
	INST(bool, Player_Maximized);
	INST(QString, Player_ShownPlugin);
	INST(bool, Player_OneInstance);
	INST(bool, Player_Min2Tray);
	INST(bool, Player_NotifyNewVersion);

	INST(QStringList, PL_Playlist);
	INST(bool, PL_Load);
	INST(bool, PL_LoadLastTrack);
	INST(bool, PL_RememberTime);
	INST(bool, PL_StartPlaying);
	INST(int, PL_LastTrack);

	INST(PlaylistMode, PL_Mode);
	INST(bool, PL_ShowNumbers);
	INST(bool, PL_SmallItems);

	INST(bool, Notification_Show);
	INST(int, Notification_Timeout);
	INST(QString, Notification_Name);
	INST(int, Notification_Scale);

	INST(QString, Engine_Name);
	INST(int, Engine_Vol);
    INST(int, Engine_CurTrackPos_s);
	INST(int, Engine_ConvertQuality);
	INST(QString, Engine_CovertTargetPath);
	INST(bool, Engine_Gapless);
    INST(bool, Engine_ShowSpectrum);
    INST(bool, Engine_ShowLevel);


	INST(bool, Engine_SR_Active);
	INST(bool, Engine_SR_Warning);
	INST(QString, Engine_SR_Path);
	INST(bool, Engine_SR_CompleteTracks);
	INST(bool, Engine_SR_SessionPath);

	INST(bool, Socket_Active);
	INST(int, Socket_From);
	INST(int, Socket_To);
	INST(int, Spectrum_Style);
	INST(bool, BroadCast_Active);
	INST(bool, Broadcast_Prompt);
	INST(int, Broadcast_Port);

}


namespace SetNoDB{

    INST(int, Broadcast_Clients);

}

#endif // SETTINGKEY_H
