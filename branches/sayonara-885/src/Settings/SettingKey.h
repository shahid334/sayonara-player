#ifndef SETTINGKEY_H
#define SETTINGKEY_H

#include <QString>
#include <QPoint>
#include <QSize>
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
		Eq_Flat,
		Eq_Rock,
		Eq_Treble,
		Eq_Bass,
		Eq_Mid,
		Eq_LightRock,
		Eq_Custom,

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
		PL_LastTrackPos,
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

		Num_Setting_Keys
	};
}

template<typename T, SK::SettingKey S>
class SettingKey
{};

namespace Set {
	const SettingKey<QStringList, SK::LFM_Login> LFM_Login;
	const SettingKey<bool, SK::LFM_Active>LFM_Active;

	const SettingKey<bool, SK::LFM_Corrections> LFM_Corrections;
	const SettingKey<bool, SK::LFM_ShowErrors> LFM_ShowErrors;
	const SettingKey<QString, SK::LFM_SessionKey> LFM_SessionKey;

	const SettingKey<int, SK::Eq_Last> Eq_Last;
	const SettingKey<EQ_Setting, SK::Eq_Flat> Eq_Flat;
	const SettingKey<EQ_Setting, SK::Eq_Rock> Eq_Rock;
	const SettingKey<EQ_Setting, SK::Eq_Treble> Eq_Treble;
	const SettingKey<EQ_Setting, SK::Eq_Bass> Eq_Bass;
	const SettingKey<EQ_Setting, SK::Eq_Mid> Eq_Mid;
	const SettingKey<EQ_Setting, SK::Eq_LightRock> Eq_LightRock;
	const SettingKey<EQ_Setting, SK::Eq_Custom> Eq_Custom;

	const SettingKey<bool, SK::Lib_Show> Lib_Show;
	const SettingKey<QString, SK::Lib_Path> Lib_Path;
	const SettingKey<QList<int>, SK::Lib_ColsTitle> Lib_ColsTitle;
	const SettingKey<QList<int>, SK::Lib_ColsArtist> Lib_ColsArtist;
	const SettingKey<QList<int>, SK::Lib_ColsAlbum> Lib_ColsAlbum;
	const SettingKey<bool, SK::Lib_OnlyTracks> Lib_OnlyTracks;
	const SettingKey<bool, SK::Lib_LiveSearch> Lib_LiveSearch;
	const SettingKey<QList<int>, SK::Lib_Sorting> Lib_Sorting;

	const SettingKey<QString, SK::Player_Version> Player_Version;
	const SettingKey<QString, SK::Player_Language> Player_Language;
	const SettingKey<int, SK::Player_Style> Player_Style;
	const SettingKey<QSize, SK::Player_Size> Player_Size;
	const SettingKey<QPoint, SK::Player_Pos> Player_Pos;
	const SettingKey<bool, SK::Player_Fullscreen> Player_Fullscreen;
	const SettingKey<bool, SK::Player_Maximized> Player_Maximized;
	const SettingKey<QString, SK::Player_ShownPlugin> Player_ShownPlugin;
	const SettingKey<bool, SK::Player_OneInstance> Player_OneInstance;
	const SettingKey<bool, SK::Player_Min2Tray> Player_Min2Tray;
	const SettingKey<bool, SK::Player_NotifyNewVersion> Player_NotifyNewVersion;

	const SettingKey<QStringList, SK::PL_Playlist> PL_Playlist;
	const SettingKey<bool, SK::PL_Load> PL_Load;
	const SettingKey<bool, SK::PL_LoadLastTrack> PL_LoadLastTrack;
	const SettingKey<bool, SK::PL_RememberTime> PL_RememberTime;
	const SettingKey<bool, SK::PL_StartPlaying> PL_StartPlaying;
	const SettingKey<int, SK::PL_LastTrack> PL_LastTrack;
	const SettingKey<int, SK::PL_LastTrackPos> PL_LastTrackPos;
	const SettingKey<PlaylistMode, SK::PL_Mode> PL_Mode;
	const SettingKey<bool, SK::PL_ShowNumbers> PL_ShowNumbers;
	const SettingKey<bool, SK::PL_SmallItems> PL_SmallItems;

	const SettingKey<bool, SK::Notification_Show> Notification_Show;
	const SettingKey<int, SK::Notification_Timeout> Notification_Timeout;
	const SettingKey<QString, SK::Notification_Name> Notification_Name;
	const SettingKey<int, SK::Notification_Scale> Notification_Scale;

	const SettingKey<QString, SK::Engine_Name> Engine_Name;
	const SettingKey<int, SK::Engine_Vol> Engine_Vol;
	const SettingKey<int, SK::Engine_ConvertQuality> Engine_ConvertQuality;
	const SettingKey<QString, SK::Engine_CovertTargetPath> Engine_CovertTargetPath;

	const SettingKey<bool, SK::Engine_SR_Active> Engine_SR_Active;
	const SettingKey<bool, SK::Engine_SR_Warning> Engine_SR_Warning;
	const SettingKey<QString, SK::Engine_SR_Path> Engine_SR_Path;
	const SettingKey<bool, SK::Engine_SR_CompleteTracks> Engine_SR_CompleteTracks;
	const SettingKey<bool, SK::Engine_SR_SessionPath> Engine_SR_SessionPath;

	const SettingKey<bool, SK::Socket_Active> Socket_Active;
	const SettingKey<int, SK::Socket_From> Socket_From;
	const SettingKey<int, SK::Socket_To> Socket_To;
	const SettingKey<int, SK::Spectrum_Style> Spectrum_Style;
	const SettingKey<bool, SK::BroadCast_Active> BroadCast_Active;
	const SettingKey<bool, SK::Broadcast_Prompt> Broadcast_Prompt;
	const SettingKey<int, SK::Broadcast_Port> Broadcast_Port;
}




#endif // SETTINGKEY_H
